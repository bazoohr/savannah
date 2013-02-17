#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <lib_mem.h>
#include <types.h>
#include <cdef.h>

void memory_init (const phys_addr_t last_allocated_byte, const size_t mem_size);
phys_addr_t __warn_unused_result alloc_mem_pages (size_t n);
phys_addr_t __warn_unused_result alloc_clean_mem_pages (size_t n);
phys_addr_t __warn_unused_result alloc_page_table (void);
void free_mem_pages (phys_addr_t addr);

#endif /* __MEMORY_H__ */
