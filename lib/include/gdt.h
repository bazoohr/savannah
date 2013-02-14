#ifndef __GDT_H___
#define __GDT_H__

#include <types.h>
#include <cdef.h>
#include <cpu.h>

void create_new_gdt (struct system_descriptor *gdt, size_t sizeof_gdt);
void reload_gdt (struct descriptor_register *addr, int code_sel, int data_sel);
#endif
