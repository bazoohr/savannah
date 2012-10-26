#include <proc.h>
#include <dev/i8253.h>
#include <dev/ioapic.h>
#include <dev/pic.h>
#include <frame.h>
#include <printk.h>
#include <const.h>
#include <schedule.h>
#include <global.h>
#include <dev/lapic.h>
size_t ticks = 0;
/* ====================================== */
void
clock_init (void)
{
  i8253_init ();
//  pic_enable (0x20);
//  ioapic_enable (0x0, 0);
}
/* ====================================== */
void
clock_stop (void)
{
  i8253_stop ();
}
/* ====================================== */
void
do_timer (struct trapframe *tf)
{
  /*if (current->p_remained_quantum >= 0) {
    current->p_remained_quantum--;
  }
  schedule ();*/
  cprintk ("ticks = %d\n", 0x9, ticks++);
  lapic_eoi();
}
