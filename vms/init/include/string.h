#ifndef __STRING_H
#define __STRING_H

#include <types.h>
#include <cdef.h>

void * memcpy (void *dst, const void *src, size_t count);
void * memset (void *ptr, int c, size_t n);
int memcmp(const void *cs, const void *ct, size_t count);

#endif
