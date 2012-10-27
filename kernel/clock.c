#include <dev/i8253.h>
#include <dev/ioapic.h>
#include <frame.h>
#include <printk.h>
#include <const.h>
#include <dev/lapic.h>
size_t ticks = 0;
/* ====================================== */
void
clock_init (void)
{
  i8253_init ();
}
/* ====================================== */
void
do_timer (struct trapframe *tf)
{
  cprintk ("ticks = %d\n", 0x9, ticks++);
  lapic_eoi();
}
