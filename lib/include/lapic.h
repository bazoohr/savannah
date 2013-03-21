#ifndef __LAPIC_H__
#define __LAPIC_H__

#include <types.h>
#include <apicreg.h>

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

#endif /* __LAPIC_H__ */
