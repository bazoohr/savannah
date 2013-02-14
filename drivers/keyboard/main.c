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
#include <gdt.h>
struct system_descriptor gdt[NGDT] __aligned (16);
/* ========================================== */
int
main (void)
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

  cprintk ("This is driver!!\n", 0xE);
  __asm__ __volatile__ ("sti\n");

  for (;;);
  return 0;
}
