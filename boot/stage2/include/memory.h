#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <types.h>
#include <cdef.h>

void memory_init (phys_addr_t last_allocated_byte, size_t mem_size);
__warn_unused_result void * malloc (size_t size);
__warn_unused_result void * malloc_align (size_t size, size_t alignment);
__warn_unused_result void * calloc (size_t nmemb, size_t size);
__warn_unused_result void * calloc_align (size_t nmemb, size_t size, size_t alignment);
#endif /* __MEMORY_H__ */
