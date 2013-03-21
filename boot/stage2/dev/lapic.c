#include <dev/lapic.h>
#include <asmfunc.h>
#include <isr.h>
#include <printk.h>
#include <dev/apicreg.h>
#include <timer.h>
#include <panic.h>
#include <nvram.h>
#include <isareg.h>
#include <cpu.h>
#include <mp.h>
#include <interrupt.h>
#include <cpuinfo.h>

uint32_t
lapic_read(uint32_t off)
{
  return *(volatile uint32_t *) ((uint8_t*)0xFEE00000 + off);
}

void
lapic_eoi(void)
{
  lapic_write(0x0b0, 0);
}

void
lapic_write(uint32_t off, uint32_t val)
{
  *(volatile uint32_t *) ((uint8_t*)0xFEE00000 + off) = val; /* FEE00000 should be fixed */
  lapic_read(LAPIC_ID);        // Wait for the write to finish, by reading
}
static int
lapic_icr_wait()
{
  uint32_t i = 100000;
  while ((lapic_read (LAPIC_ICRLO) & LAPIC_DLSTAT_BUSY) != 0) {
    nop_pause ();
    i--;
    if (i == 0) {
      cprintk ("apic_icr_wait: wedged?\n", 0x4);
      return -/*E_INVAL*/1;
    }
  }
  return 0;
}

static int
lapic_ipi_init(uint32_t apicid)
{
    // Intel MultiProcessor spec. section B.4.1
    lapic_write (LAPIC_ICRHI, apicid << LAPIC_ID_SHIFT);
    lapic_write (LAPIC_ICRLO, apicid | LAPIC_DLMODE_INIT | LAPIC_LVL_TRIG |
	       LAPIC_LVL_ASSERT);
    lapic_icr_wait ();
    timer_delay (10);	// 10ms

    lapic_write (LAPIC_ICRLO, apicid | LAPIC_DLMODE_INIT | LAPIC_LVL_TRIG |
	       LAPIC_LVL_DEASSERT);
    lapic_icr_wait ();

    int result = (lapic_read (LAPIC_ICRLO) & LAPIC_DLSTAT_BUSY);
    return (result) ? -1/*-E_BUSY*/ : 0;
}
/*
 * TODO:
 *     I think this file is not the best place for this
 *     function. Move it to a better place.
 */
#if 0
static void
boot_aps_tail (cpuid_t id)
{
  create_new_gdt (id);
  interrupt_init ();
  cpus[id].booted = 1;
  cprintk ("CPU number %d booted!\n", 0xE, id);
  halt ();
}
#endif
void __inline
lapic_startaps (cpuid_t cpuid)
{
  // Universal Start-up Algorithm from Intel MultiProcessor spec
  int r;
  uint16_t *dwordptr;
  uint32_t i;
/*  struct cpu **aps_info;
  void (**aps_enterance)(cpuid_t id);*/
  struct cpu_info **cpu_info;
  struct cpu_info *cpu;

  cpu = get_cpu_info (cpuid);

  if (!cpu) {
    panic ("Failed to get cpu information. boot/stage2/lapic.c");
  }

  if (cpu->msg_ready[0]) {
    panic ("Why do you try to boot a booted processor?!");
  }

  cpu_info = (struct cpu_info **)((uint8_t *)CPU_INFO_PTR_ADDR);
  //cpu_cr3 = (phys_addr_t *)((uint8_t *)0x9F000 + 520);

  // "The BSP must initialize CMOS shutdown code to 0Ah ..."
  outb (NVRAM_RESET, IO_RTC);
  outb (NVRAM_RESET_JUMP, IO_RTC + 1);

  // "and the warm reset vector (DWORD based at 40:67) to point
  // to the AP startup code ..."
  dwordptr = (uint16_t*)0x467;
  dwordptr[0] = 0;             /* code offset  */
  dwordptr[1] = BOOT_APS_BASE_ADDR >> 4;  /* code segment */
  /*
   * Let the application processor know its stack, and page tables
   */
  *cpu_info = cpu;
  //*cpu_cr3 = cpus[cpuid].page_tables;
  // ... prior to executing the following sequence:"
  if ((r = lapic_ipi_init(cpuid)) < 0)
    panic ("unable to send init error r");

  timer_delay (10);	// 10ms

  for (i = 0; i < 2; i++) {
    lapic_icr_wait();
    lapic_write (LAPIC_ICRHI, cpuid << LAPIC_ID_SHIFT);
    lapic_write (LAPIC_ICRLO, LAPIC_DLMODE_STARTUP | (BOOT_APS_BASE_ADDR>> 12));
    timer_delay (1);	// 1 ms
  }
}

uint64_t
get_bus_freq (void)
{
  uint64_t cpubusfreq;
  uint64_t tmp;

  lapic_write(LAPIC_DFR, 0xffffffff);
  lapic_write(LAPIC_LDR, ((lapic_read(LAPIC_LDR)&0x00ffffff)|1));
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_PCINT, 4<<8); /* NMI */
  lapic_write(LAPIC_LVINT0, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_LVINT1, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_TPRI, 0);

  lapic_write(LAPIC_SVR, 39 | LAPIC_SVR_ENABLE);
  lapic_write(LAPIC_LVTT, 32);
  lapic_write(LAPIC_DCR_TIMER, LAPIC_DCRT_DIV16);

  //initialize PIT Ch 2 in one-shot mode
  //waiting 1 sec could slow down boot time considerably,
  //so we'll wait 1/100 sec, and multiply the counted ticks
  outb((inb (0x61) & 0x0C) | 1, 0x61);
  outb (0xB0, 0x43);
  //1193180/100 Hz  = 11931 = 0x2E9B
  //1193180/1000 Hz = 1193  = 0x04A9
  outb (0x9B, 0x42);  //LSB
  inb (0x60); //short delay
  outb (0x2E, 0x42);  //MSB

  //reset PIT one-shot counter (start counting)
  tmp = inb(0x61) & 0xFE;
  outb ((uint8_t)tmp, 0x61);    //gate low
  outb ((uint8_t)tmp | 1, 0x61);    //gate high

  //reset APIC timer (set counter to -1)
  lapic_write(LAPIC_ICR_TIMER, 0xFFFFFFFF);

  //now wait until PIT counter reaches zero
  while(! (inb (0x61) & 0x20));

  //stop APIC timer
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);

  //reset APIC timer (set counter to -1)
#if 0
  lapic_write(LAPIC_ICR_TIMER, 0xFFFFFFFF);

  timer_delay (10);
  //stop APIC timer
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);
#endif
  //now do the math...
  cpubusfreq = ((0xFFFFFFFF - lapic_read (LAPIC_CCR_TIMER)) + 1) * 16 * 100;

  return cpubusfreq;
#if 0
  cprintk ("cpu bus frequency is %d hz\n", 0xE, cpubusfreq);
  halt ();
  tmp = cpubusfreq / quantum / 16;

  //sanity check, now tmp holds appropriate number of ticks, use it as APIC timer counter initializer
  lapic_write(LAPIC_ICR_TIMER, ((tmp < 16) ? 16 : tmp));
  //finally re-enable timer in periodic mode
  lapic_write(LAPIC_LVTT, 32 | LAPIC_LVT_PERIODIC);
  //setting divide value register again not needed by the manuals
  //although I have found buggy hardware that required it
  lapic_write(LAPIC_DCR_TIMER, LAPIC_DCRT_DIV16);
#endif
}
void
lapic_init(void)
{
  /*
   * Initialization of the Local APIC
   * http://wiki.osdev.org/APIC_timer
   */
#if 0
  /*
   * TODO:
   *     LAPIC_DFR & LAPIC_LDR are important for IPI messages, and need to be set
   *     in all cores appropriatly.
   */
  /* Program lapic's Destination Format Register into FLAT MODE */
  lapic_write(LAPIC_DFR, 0xffffffff);
  lapic_write(LAPIC_LDR, ((lapic_read(LAPIC_LDR)&0x00ffffff)|1));
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_PCINT, 4<<8); /* NMI */
  lapic_write(LAPIC_LVINT0, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_LVINT1, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_TPRI, 0);

  lapic_write(LAPIC_SVR, 39 | LAPIC_SVR_ENABLE); /* TODO: Write the 39 dummy */
  lapic_write(LAPIC_LVTT, 32);
  lapic_write(LAPIC_DCR_TIMER, LAPIC_DCRT_DIV16);

  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);

  lapic_write(LAPIC_ICR_TIMER, 16);
  lapic_write(LAPIC_LVTT, 32 | LAPIC_LVT_PERIODIC);
  lapic_write(LAPIC_DCR_TIMER, LAPIC_DCRT_DIV16);
#endif

  /*
   * initializing the local timer of BSP
   */
  lapic_write(LAPIC_DCR_TIMER, LAPIC_DCRT_DIV1);
  lapic_write(LAPIC_ICR_TIMER, 0x0fffffff);
  lapic_write(LAPIC_LVTT, LAPIC_LVT_PERIODIC | /*LAPIC_LVT_MASKED | */(IRQ_OFFSET + IRQ_TIMER));

  //uint64_t ccr0 = lapic_read(LAPIC_CCR_TIMER);
  //cprintk("crr0 = %x\n", 0xE, ccr0);
  //uint64_t cpubusfreq = ((0xFFFFFFFF - lapic_read(LAPIC_CCR_TIMER)) + 1) * 16 * 100;
  //cprintk("Cpu Bus Freq: %d\n", 0x5, cpubusfreq);

  //uint64_t ccr1 = lapic_read(LAPIC_CCR_TIMER);
  //cprintk("crr0 = %x\n", 0xE, ccr1);
}
