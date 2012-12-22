#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <kernel_args.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <memory.h>
#include <dev/keyboard.h>
#include <mp.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <cpu.h>
/* ========================================== */
void
print_logo(void)
{
  cprintk("Welcome to VMM\n", 0xE);
  cprintk("       v       v  u    u         oo      sss\n", 0xE);
  cprintk("        v     v   u    u        o  o    s\n", 0xE);
  cprintk("         v   v    u    u  ===   o  o     s\n", 0xE);
  cprintk("          v v     u    u        o  o      s\n", 0xE);
  cprintk("           v       uuuu          oo     sss\n", 0xE);
  cprintk("\n", 0xE);
}

void 
vmm_main (struct cpu *cpuinfo)
{
  con_init ();
  cprintk ("My cpuid is = %d\n", 0xA, cpuinfo->cpuid);
  create_new_gdt (cpuinfo->cpuid);
  interrupt_init ();
#if 0
  mm_init (kargs->ka_kernel_end_addr, kargs->ka_kernel_cr3, kargs->ka_memsz);
  uint32_t eax, ebx, ecx, edx;
  cpuid (1, &eax, NULL, &ecx, NULL);
  if (ecx & (1 << 5)) {
    cprintk ("VMX is supported!\n", 0xA);
  } else {
    cprintk ("VMX is NOT supported!\n", 0x4);
  }
  cpuid (1, &eax, NULL, &ecx, NULL);
  if (ecx & (1 << 3)) {
    cprintk ("Monitor/Mwait is supported!\n", 0xA);
  } else {
    cprintk ("Monitor/Mwait is NOT supported!\n", 0x4);
  }
	cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
	cprintk ("Physical Address Bits: %d\n", 0xE, eax & 0x000000FF);
	cprintk ("Cores per Die: %d\n", 0xE, (ecx & 0x000000FF) + 1);
  mp_init ();

  interrupt_init ();

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();

#endif
  print_logo();
  cpuinfo->booted = 1;
  /*
  mp_bootothers ();

  __asm__ __volatile__ ("sti\n");
*/
  for (;;) {__asm__ __volatile__ ("cli;hlt\n\t");}
}
