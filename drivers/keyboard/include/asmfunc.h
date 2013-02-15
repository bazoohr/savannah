#ifndef __KLIB_H__
#define __KLIB_H__

#include <types.h>
#include <cpu.h>
#include <cdef.h>

void __always_inline cli (void);
//void cpuid (register_t *rax, register_t *rbx, register_t *rcx, register_t *rdx);
uint64_t rdmsr (uint32_t reg);
void wrmsr (uint32_t reg, uint64_t val);
//uint64_t rdmsr (uint32_t msr_addr);
uint8_t inb (uint16_t port);
void outb (uint8_t data, uint16_t port);
void lidt (struct descriptor_register *addr);
void load_cr3 (phys_addr_t addr);

void lcr0 (uint64_t val);
uint64_t rcr0 (void);
uint64_t rcr2 (void);
void lcr3 (uint64_t val);
uint64_t rcr3 (void);
void lcr4 (uint64_t val);
uint64_t rcr4 (void);

void cache_flush (void);
void tlb_flush_global (void);

void nop_pause (void);
void cpuid (uint32_t function, uint32_t *eaxp, uint32_t *ebxp, uint32_t *ecxp, uint32_t *edxp);
void halt (void);
#endif /* __KLIB_H__ */
