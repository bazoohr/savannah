#ifndef __STRING_H
#define __STRING_H

#include <types.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *s, int c, size_t count);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
int strlen(const char *s);
int strcmp(const char *cs, const char *ct);

#endif
