#ifndef __PRINTK_H
#define __PRINTK_H

#include <console.h>

void printk (const char *fmt, ...);
void cprintk (const char* fmt, int color, ...);

#endif
