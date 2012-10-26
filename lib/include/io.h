#ifndef __IO_H__
#define __IO_H__

#include <types.h>
#include <screen.h>

#define TEXT_COLOR_WHITE 0x7
#define TEXT_COLOR_RED   0x4
#define TEXT_COLOR_GREEN 0x2

void printf (const char* fmt, ...);
void cprintf (const char* fmt, int color, ...);
int getch (void);

#endif /* __IO_H__ */
