#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <cdef.h>

void gotoxy (int x, int y);
void wherexy (int *x, int *y);
void puts (const char *str, int color);
void putc (char ch, int color);

#endif /* __SCREEN_H__ */
