#include <apicreg.h>
#include <timer.h>
#include <asmfunc.h>
#include <debug.h>
#include <cpuinfo.h>
#include <panic.h>
#include <interrupt.h>
#include <lapic.h>
#include <vuos/vuos.h>

void
init_timer (void)
{
  const int vector = 32;

  lapic_write (LAPIC_LVTT, LAPIC_LVT_MASKED);
  lapic_write (LAPIC_PCINT, 4 << 8); /* NMI */
  lapic_write (LAPIC_LVINT0, LAPIC_LVT_MASKED);
  lapic_write (LAPIC_LVINT1, LAPIC_LVT_MASKED);
  /* No special priority for any task */
  lapic_write (LAPIC_TPRI, 0);

  /* Enable the lapic, and spurious interrupts go to vector 39 */
  lapic_write (LAPIC_SVR, 39 | LAPIC_SVR_ENABLE);
  /* Timer interrupts go to vector 32 & enable periodic mode*/
  lapic_write (LAPIC_LVTT, vector | LAPIC_LVT_PERIODIC);
  lapic_write (LAPIC_DCR_TIMER, LAPIC_DCRT_DIV16);

  add_irq (vector);
}

void
timer_on (uint64_t ms)
{
  uint64_t counter_value;

  if (unlikely (ms < 1)) {
    panic ("lib (%s:%d): Invalid time interval", __func__, __LINE__);
  }

  counter_value = (cpuinfo->bus_freq / 1000 ) * ms;
  lapic_write(LAPIC_LVTT, lapic_read (LAPIC_LVTT) & (~LAPIC_LVT_MASKED));
  lapic_write(LAPIC_ICR_TIMER, counter_value);
}

void
timer_off (void)
{
  lapic_write(LAPIC_ICR_TIMER, 0);
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);
}
