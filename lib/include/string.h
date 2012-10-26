#ifndef __STRING_H
#define __STRING_H

#include <types.h>

void bcopy (register const void *src, void *dst, size_t count);
void *memcpy (register void *dest, register const void *src, size_t size);
void *memset (register void *ptr, int c, size_t n);

#endif
