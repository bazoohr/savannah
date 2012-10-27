#ifndef __STRING_H
#define __STRING_H

void * memcpy (void *dst, const void *src, size_t count);
void bcopy (const void *src, void *dst, size_t count);
void *memset (void *ptr, int c, size_t n);

#endif
