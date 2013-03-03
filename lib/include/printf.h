#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <types.h>

void putc (int ch);
void puts (char *str);
void snprintf (char *buf, const char *fmt, const size_t n, ...);
void printf (const char* fmt, ...);

#endif /* __PRINTF_H__ */
