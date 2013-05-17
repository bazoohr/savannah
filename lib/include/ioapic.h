#ifndef __IOAPIC_H__
#define __IOAPIC_H__

#include <types.h>

// The I/O APIC manages hardware interrupts for an SMP system.
// http://www.intel.com/design/chipsets/datashts/29056601.pdf
#define IOAPIC_ICR_INT_MASK (1 << 16)
#define IRQ_OFFSET 32

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

extern uint8_t *ioapicva;
extern uint32_t ioapicid;

uint32_t ioapic_read(uint32_t reg);
void ioapic_write(uint32_t reg, uint32_t data);
uint8_t ioapic_version(void);
void ioapic_enable(uint32_t irq, cpuid_t cpunum);
void ioapic_disable(uint32_t irq, cpuid_t cpunum);
void ioapic_enable_pin (int pin);
void ioapic_disable_pin (int pin);
void ioapic_eoi (int vector);

#endif /* __IOAPIC_H__ */
