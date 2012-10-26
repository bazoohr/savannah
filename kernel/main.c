#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <kernel_args.h>
#include <printk.h>
#include <console.h>
#include <proc.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <clock.h>
#include <memory.h>
#include <queue.h>
#include <resource.h>
#include <dev/keyboard.h>
#include <mp.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <mtrr.h>
/* ========================================== */
static void
print_logo(void)
{
  cprintk("Welcome to\n", 0xB);
  cprintk("       v       v  u    u         oo      sss\n", 0xB);
  cprintk("        v     v   u    u        o  o    s\n", 0xB);
  cprintk("         v   v    u    u  ===   o  o     s\n", 0xB);
  cprintk("          v v     u    u        o  o      s\n", 0xB);
  cprintk("           v       uuuu          oo     sss\n", 0xB);
  cprintk("\n", 0xB);
}
void 
kmain (struct kernel_args *kargs)
{
  con_init ();
  mm_init (kargs->ka_kernel_end_addr, kargs->ka_kernel_cr3, kargs->ka_memsz);
  mp_init ();
  // Disable caching
  lcr0((rcr0() | CR0_CD) & ~CR0_NW);

  interrupt_init (); /*idt*/

  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();

  print_logo();

  __asm__ __volatile__ ("sti\n");

  for (;;);
}
