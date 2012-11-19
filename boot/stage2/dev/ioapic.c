#include <debug.h>
#include <types.h>
#include <dev/ioapic.h>
#include <isr.h>
#include <asmfunc.h>
#include <dev/pic.h>
#include <dev/lapic.h>
#include <panic.h>

// The I/O APIC manages hardware interrupts for an SMP system.
// http://www.intel.com/design/chipsets/datashts/29056601.pdf

uint8_t *ioapicva;
uint32_t ioapicid;

#define REG_ID     0x00  // Register index: ID
#define REG_VER    0x01  // Register index: version
#define REG_TABLE  0x10  // Redirection table base

// The redirection table starts at REG_TABLE and uses
// two registers to configure each interrupt.  
// The first (low) register in a pair contains configuration bits.
// The second (high) register contains a bitmask telling which
// CPUs can serve that interrupt.
#define INT_DISABLED   0x00010000  // Interrupt disabled
#define INT_EXTINT     0x00002100  // Interrupt disabled
#define INT_LEVEL      0x00008000  // Level-triggered (vs edge-)
#define INT_ACTIVELOW  0x00002000  // Active low (vs high)
#define INT_LOGICAL    0x00000800  // Destination is CPU id (vs APIC ID)

volatile struct ioapic *ioapic;

// IO APIC MMIO structure: write reg, then read or write data.
struct ioapic {
  uint32_t reg;
  uint32_t pad[3];
  uint32_t data;
};

uint32_t
ioapic_read(uint32_t reg)
{
  ioapic->reg = reg;
  return ioapic->data;
}

static void
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
  ioapic_write(REG_TABLE+2*irq, IRQ_OFFSET + irq);
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
ioapic_init(void)
{
  int i, maxintr;
  uint32_t id;

  if (ioapicva == NULL)
	  panic ("ioapicva is NOT set");

  ioapic = (volatile struct ioapic*)ioapicva;
  maxintr = (ioapic_read(REG_VER) >> 16) & 0xFF;

  id = ioapic_read(REG_ID) >> 24;

/*  if (id != ioapicid)
    panic("CPU id is NOT equal to APIC id. NOT a MP architecture");
*/
  /*
   * Disable all interrupts, and then tell IOAPIC to NOT route any interrupt to any core
   */
  for (i = 0; i <= maxintr; i++) {
    ioapic_write(REG_TABLE+2*i, INT_DISABLED | (IRQ_OFFSET + i));
    ioapic_write(REG_TABLE+2*i+1, 0);
  }
}
