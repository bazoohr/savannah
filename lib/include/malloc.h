#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <cdef.h>
#include <types.h>

void * __warn_unused_result malloc (size_t size);
void free (void *ptr);

#endif /* __MALLOC_H__ */
