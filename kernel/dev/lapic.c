#include <dev/lapic.h>
#include <asmfunc.h>
#include <isr.h>
#include <printk.h>

uint32_t
lapic_read(uint32_t off)
{   
  return *(volatile uint32_t *) (0xFEE00000 + off);
}

void
lapic_eoi(void)
{
  lapic_write(0x0b0, 0);
}

void
lapic_write(uint32_t off, uint32_t val)
{   
  *(volatile uint32_t *) (0xFEE00000 + off) = val; /* FEE00000 should be fixed */
  lapic_read(LAPIC_ID);        // Wait for the write to finish, by reading
}

void
lapic_init(void)
{
  /*
   * Initialization of the Local APIC
   * http://wiki.osdev.org/APIC_timer
   */
#if 0
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



  lapic_write(LAPIC_DCR_TIMER, LAPIC_DCRT_DIV1);
  lapic_write(LAPIC_ICR_TIMER, 0x0fffffff);
  lapic_write(LAPIC_LVTT, LAPIC_LVT_PERIODIC | /*LAPIC_LVT_MASKED | */(IRQ_OFFSET + IRQ_TIMER));

  //uint64_t ccr0 = lapic_read(LAPIC_CCR_TIMER);
  //cprintk("crr0 = %x\n", 0xE, ccr0);

  outb(((inb(0x61)%0xFD)|1), 0x61);
  outb(0xB2, 0x43);
  outb(0x9B, 0x42);
  // 1193180 / 100 Hz = 11931 = 0x2e9b
  inb(0x60);
  outb(0x42, 0x2E);

  uint8_t tmp = inb(0x61) & 0xfe;
  outb(tmp, 0x61);
  outb(tmp | 1, 0x61);

  while (!(inb(0x61) & 0x20));

  //uint64_t cpubusfreq = ((0xFFFFFFFF - lapic_read(LAPIC_CCR_TIMER)) + 1) * 16 * 100;
  //cprintk("Cpu Bus Freq: %d\n", 0x5, cpubusfreq);

  //uint64_t ccr1 = lapic_read(LAPIC_CCR_TIMER);
  //cprintk("crr0 = %x\n", 0xE, ccr1);
}
