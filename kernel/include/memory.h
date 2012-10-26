#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <types.h>
#include <cdef.h>

void mm_init (const phys_addr_t first_free_byte, const phys_addr_t kcr3, const size_t memsz);
phys_addr_t __warn_unused_result alloc_mem_pages (size_t n);
void free_mem_pages (phys_addr_t addr);

#endif /* __MEMORY_H__ */
