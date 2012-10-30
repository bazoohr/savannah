#ifndef __KLIB_H__
#define __KLIB_H__

#include <types.h>
#include <segment.h>
#include <cdef.h>

void __always_inline cli (void);
void cpuid (register_t *rax, register_t *rbx, register_t *rcx, register_t *rdx);
uint64_t rdmsr(uint32_t reg);
void wrmsr(uint32_t reg, uint64_t val);
//uint64_t rdmsr (uint32_t msr_addr);
uint8_t inb (uint16_t port);
void outb (uint8_t data, uint16_t port);
void reload_gdt (struct descriptor_register *addr, int kcode_sel, int kdata_sel);
void lidt (struct descriptor_register *addr);
void load_cr3 (phys_addr_t addr);
void * __inline fast_memcpy (void *dest, void *src, size_t size);
void * __inline fast_memset (void *ptr, int c, size_t size);


void lcr0 (uint64_t val);
uint64_t rcr0 (void);
uint64_t rcr2 (void);
void lcr3 (uint64_t val);
uint64_t rcr3 (void);
void lcr4 (uint64_t val);
uint64_t rcr4 (void);

void cache_flush (void);
void tlb_flush_global(void);

void nop_pause(void);

void halt(void);
#endif /* __KLIB_H__ */
