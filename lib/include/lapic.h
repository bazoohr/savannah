#ifndef __LAPIC_H__
#define __LAPIC_H__

#include <types.h>
#include <apicreg.h>
#include <debug.h>  /* TODO: remove me */

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

static __inline void
lapic_on (void)
{
  /* Do we really need all these lines to enable APIC? */
  lapic_write(LAPIC_DFR, 0xffffffff);
  lapic_write(LAPIC_LDR, ((lapic_read(LAPIC_LDR)&0x00ffffff)|1));
  lapic_write(LAPIC_LVTT, LAPIC_LVT_MASKED);
  lapic_write(LAPIC_PCINT, 4 << 8); /* NMI */
  lapic_write(LAPIC_LVINT0, LAPIC_LVT_MASKED | (1 << 13));
  lapic_write(LAPIC_LVINT1, LAPIC_LVT_MASKED | (1 << 13));
  lapic_write(LAPIC_TPRI, 0);

  lapic_write(LAPIC_SVR, 39 | LAPIC_SVR_ENABLE);
}

static void __inline
lapic_send_ipi (unsigned int vector, cpuid_t cpuid)
{
  while (lapic_read (LAPIC_ICRLO) & LAPIC_DLSTAT_BUSY);
  lapic_write (LAPIC_ICRHI, cpuid << LAPIC_ID_SHIFT);
  lapic_write (LAPIC_ICRLO, vector);

}
#endif /* __LAPIC_H__ */
