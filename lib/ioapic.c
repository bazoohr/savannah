#include <types.h>
#include <ioapic.h>
#include <lapic.h>
#include <panic.h>

// IO APIC MMIO structure: write reg, then read or write data.
struct ioapic {
  uint32_t reg;
  uint32_t pad[3];
  uint32_t data;
};

/*
 * GCC optimize WAY TOO much, so leave this structure as volatile.
 */
volatile static struct ioapic * ioapic = ((struct ioapic*)0xFEC00000);

uint32_t
ioapic_read(uint32_t reg)
{
  ioapic->reg = reg;
  return ioapic->data;
}

void
ioapic_write(uint32_t reg, uint32_t data)
{
  ioapic->reg = reg;
  ioapic->data = data;
}

void
ioapic_enable(uint32_t irq, cpuid_t cpunum)
{
  /*
   * TODO:
   *  Check that IRQ does not exceed the maximum number of
   *  supported hw interrupts
   */
  /*
   * HACK:
   *     First 16 IRQs are eadge triggered, and the rest need to be level triggered!
   */
  ioapic_write(REG_TABLE+2*irq, IRQ_OFFSET + (irq | (irq > 16 ? (1 << 15) : 0)));
  ioapic_write(REG_TABLE+2*irq+1, cpunum << 24);
}

void
ioapic_disable(uint32_t irq, cpuid_t cpunum)
{
  /*
   * TODO:
   *  Check that IRQ does not exceed the maximum number of
   *  supported hw interrupts
   */
  /* XXX:
   * What should we check irq here??
   * MIT guys are doing it! :-)
   */
  if (irq)
    ioapic_write(REG_TABLE+2*irq, INT_DISABLED | (IRQ_OFFSET + irq));
  ioapic_write(REG_TABLE+2*irq+1, 0);
}

void
ioapic_enable_pin (int pin)
{
  uint32_t lo;

  lo = ioapic_read (REG_TABLE + 2 * pin);
  lo &= ~IOAPIC_ICR_INT_MASK;

  ioapic_write (REG_TABLE + 2 * pin, lo);
}


void
ioapic_disable_pin (int pin)
{
  uint32_t lo;

  lo = ioapic_read (REG_TABLE + 2 * pin);
  lo |= IOAPIC_ICR_INT_MASK;

  ioapic_write (REG_TABLE + 2 * pin, lo);
}
