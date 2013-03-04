/* ======================================== *
 * console.c                                *
 *                                          *
 * Hamid R. Bazoobandi 2009                 *
 * a VERY simple driver for console         *
 * ======================================== */
#include <debug_console.h>
#include <types.h>
#include <asmfunc.h>
/* ============================= */
static uint64_t y;
static uint64_t x;
static uint64_t pos;
/* ============================= */
static void con_cursor ()
{
  uint16_t loc = (y * COLUMNS) + x;

  outb(0x0F, 0x3D4);
  outb((unsigned char)(loc & 0xFF), 0x3D5);
  outb(0x0E, 0x3D4);
  outb((unsigned char )((loc >> 8) & 0xFF), 0x3D5);
}
/* ============================= */
static void scroll ()
{
  int i;
  uint16_t *vga = (uint16_t*)SCR_START;

  if (y >= LINES) {
    for (i = 0 ; i < (LINES - 1) * COLUMNS ; i++) {
      vga[i] = vga[i + COLUMNS];
    }

    for (i = (LINES - 1) * COLUMNS ; i < LINES * COLUMNS ; i++) {
      vga[i] = (uint16_t)(' ' | (0xF << 8));
    }

    y = LINES - 1;
    x = 0;
  }
}
/* ============================= */
static void put_newline ()
{
  x = 0;
  y += 1;

  scroll();
}
/* ============================= */
static void put_tab ()
{
  x += TAB_SIZE;

  scroll();
}
/* ============================= */
static void put_normal (int ch, int color)
{
  uint16_t *vga = (uint16_t*)SCR_START;
  int i;

  if (x >= COLUMNS) {
    x = 0;
    y += 1;
  }

  scroll();

  i = (y * COLUMNS) + x;

  vga[i] = ch | color << 8;
  x += 1;
}
/* ============================= */
void debug_con_putc (int ch, int color)
{
	switch (ch){
		case '\n':
			put_newline ();
			break;
		case '\t':
			put_tab ();
			break;
		default:
			put_normal (ch, color);
			break;
	}
  con_cursor ();
}
/* =============================== */
void debug_con_puts (const char *str, int color)
{
	char ch;
	while ((ch = *str++) != '\0')
		debug_con_putc (ch, color);
}
/* ============================= */
void debug_change_cursor_pos (int new_x, int new_y)
{
  y = new_y;
  x = new_x;
  pos = SCR_START + (x << 1) + y * (COLUMNS << 1);
}
/* ============================= */
void debug_get_cursor_pos (uint32_t *xptr, uint32_t *yptr)
{
	*xptr = x;
	*yptr = y;
}
/* ============================= */
void debug_con_init ()
{
	y = 0;
	x = 0;
	pos = SCR_START;
}
