#include <cdef.h>
#include <dev/pic.h>
#include <types.h>
#include <printk.h>
#include <console.h>
#include <const.h>
#include <interrupt.h>
#include <asmfunc.h>
#include <dev/keyboard.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <panic.h>
#include <config.h>
#include <cpuinfo.h>
#include <gdt.h>
struct system_descriptor gdt[NGDT] __aligned (16);
/* ========================================== */
int
main (int argc, char **argv)
{
  con_init ();

  create_new_gdt (gdt, NGDT * sizeof (struct system_descriptor));
  interrupt_init ();

  /*
  pic_init ();
  ioapic_init ();
  lapic_init();
  kbd_init ();
  */

  int i;
  for (i = 0 ; i < cpuinfo->cpuid ; i++) printk("\n");
  cprintk ("This is the keyboard driver!! %d\n", 0xE, argc);
  for (i = 0 ; i < argc ; i++) {
    cprintk ("argv[%d] = %s\n", 0xE, i, argv[i]);
  }
  __asm__ __volatile__ ("sti\n");

  for (;;);
  return 0;
}
