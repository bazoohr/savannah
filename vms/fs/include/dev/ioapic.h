#ifndef __IOAPIC_H__
#define __IOAPIC_H__

#include <types.h>

extern uint8_t *ioapicva;
extern uint32_t ioapicid;

void ioapic_init(void);
void ioapic_enable(uint32_t irq, cpuid_t cpunum);
void ioapic_disable(uint32_t irq, cpuid_t cpunum);

#endif /* __IOAPIC_H__ */
