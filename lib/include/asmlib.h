#ifndef __ASMLIB_H__
#define __ASMLIB_H__

#include <cdef.h>
#include <types.h>

int __warn_unused_result read (int idevno, char *buf, size_t size);
int clone (void *(*thread_routine)(void *args), void *args, uint8_t *stack, size_t stack_size);
void unclone (void *status);
void * __warn_unused_result sbrk (size_t size);
void * brk (void *addr);

#endif  /* __ASMLIB_H__ */
