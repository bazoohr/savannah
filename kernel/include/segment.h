#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <types.h>
#include <cdef.h>
#include <cpu.h>


void __inline gdt_update_tss (struct system_descriptor *tss_desc);
void __inline gdt_update_ldt (struct system_descriptor *ldt_desc);
void create_ldt_descriptor (struct ldt *ldt_addr, struct system_descriptor *ldt_desc);
void create_tss_descriptor (struct tss *tss_addr, struct system_descriptor *tss_desc);
void create_ldt (struct ldt *ldt, int dpl);
void create_new_gdt (cpuid_t cpuid);
#endif
