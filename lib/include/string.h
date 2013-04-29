#ifndef __STRING_H
#define __STRING_H

#include <cdef.h>
#include <types.h>

void *memcpy(void *dest , const void *src, asize_t count);
void *memset(void *s, int c, asize_t count);
int memcmp (const void *cs, const void *ct, size_t count);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, asize_t count);
int strlen(const char *s);
int strcmp(const char *cs, const char *ct);

#endif
