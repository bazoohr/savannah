#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __DEBUG__

#include <debug_console.h>

void print_debug_info (const char* fmt, int color, ...);
void snprintf (char *buf, const char *fmt, const size_t n, ...);
#define DEBUG(...) print_debug_info (__VA_ARGS__)

#else

#define DEBUG(...)

#endif

#endif
