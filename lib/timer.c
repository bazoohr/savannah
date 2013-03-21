#include <apicreg.h>
#include <timer.h>
#include <asmfunc.h>
#include <debug.h>

static __inline uint32_t
lapic_read(uint32_t off)
{
  return *(volatile uint32_t *) ((uint8_t*)0xFEE00000 + off);
}


static void __inline
lapic_write(uint32_t off, uint32_t val)
{
  *(volatile uint32_t *) ((uint8_t*)0xFEE00000 + off) = val; /* FEE00000 should be fixed */
  lapic_read(LAPIC_ID);        // Wait for the write to finish, by reading
}

static void __inline
lapic_eoi(void)
{
  lapic_write(0x0b0, 0);
}

void
do_timer (void)
{
  static uint64_t ticks = 0;
  DEBUG ("ticks = %d\n", 0x9, ticks++);
  lapic_eoi();
}

void
init_timer (uint64_t freq)
{
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_PCINT, 4 << 8); /* NMI */
  lapic_write(LAPIC_LVINT0, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_LVINT1, LAPIC_LVT_MASKED);
  /* No special priority for any task */
  lapic_write(LAPIC_TPRI, 0);

  /* Enable the lapic, and spurious interrupts go to vector 39 */
  lapic_write(LAPIC_SVR, 39 | LAPIC_SVR_ENABLE);
  /* Timer interrupts go to vector 32 & enable periodic mode*/
  lapic_write(LAPIC_LVTT, 32 | LAPIC_LVT_PERIODIC);
  lapic_write(LAPIC_DCR_TIMER, LAPIC_DCRT_DIV16);

}

void
timer_on (void)
{
  lapic_write(LAPIC_LVTT, lapic_read (LAPIC_LVTT) & (~LAPIC_LVT_MASKED));
  lapic_write(LAPIC_ICR_TIMER, 0xFFF);
}

void
timer_off (void)
{
  lapic_write(LAPIC_ICR_TIMER, 0);
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);
}

uint64_t
get_cpu_cycle (void)
{
  return rdtsc ();
}
