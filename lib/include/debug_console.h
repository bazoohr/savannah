#define __DEBUG_COLSOL_H
#ifndef __DEBUG_CONSOL_H

#define TEXT_COLOR_WHITE 0x7
#define TEXT_COLOR_RED   0x4

#define TAB_SIZE 8
#define LINES 25
#define COLUMNS 80
#define SCR_START 0xB8000
#define SCR_END (SCR_START + LINES * COLUMNS * 2)

#include <types.h>

void debug_con_putc (int ch, int color);
void debug_con_puts (const char *str, int color);
void debug_con_init ();
void debug_change_cursor_pos (int new_x, int new_y);
void debug_get_cursor_pos (uint32_t *xptr, uint32_t *yptr);

#endif  /* __DEBUG_CONSOL_H */
