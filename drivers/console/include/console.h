#define __COLSOL_H
#ifndef __CONSOL_H

#define TEXT_COLOR_WHITE 0x7
#define TEXT_COLOR_RED   0x4

#define TAB_SIZE 8
#define LINES 25
#define COLUMNS 80
#define SCR_START 0xB8000
#define SCR_END (SCR_START + LINES * COLUMNS * 2)

#include <types.h>

void putc (int ch, int color);
void puts (const char *str, int color);
void con_init (uint32_t xpos, uint32_t ypos);
void change_cursor_pos (int new_x, int new_y);
void get_cursor_pos (phys_addr_t xptr, phys_addr_t yptr);

#endif
