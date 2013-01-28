#ifndef __CGA_H__
#define __CGA_H__

#include <types.h>

#define TEXT_COLOR_WHITE 0x7
#define TEXT_COLOR_RED   0x4

#define TAB_SIZE 8
#define LINES 25union
#define COLUMNS 80
#define SCR_START 0xB8000
#define SCR_END (SCR_START + LINES * COLUMNS * 2)

void cga_putc (int ch, int color);
void cga_puts (const char *str, int color);
void cga_change_cursor_pos (int new_x, int new_y);
void cga_get_cursor_pos (virt_addr_t xptr, virt_addr_t yptr);
void cga_init ();

#endif /* __CGA_H__ */
