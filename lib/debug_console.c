/* ======================================== *
 * console.c                                *
 *                                          *
 * Hamid R. Bazoobandi 2009                 *
 * a VERY simple driver for console         *
 * ======================================== */
#include <debug_console.h>
#include <types.h>
/* ============================= */
static uint64_t y;
static uint64_t x;
static uint64_t pos;
/* ============================= */
static void scroll ()
{
	uint64_t temp1 = SCR_START;
	uint64_t temp2 = SCR_START + COLUMNS * 2;
	while (temp2 <= SCR_END)
		*(char*)temp1++ = *(char*)temp2++;
	while (temp1 < SCR_END)
		*(long*)temp1++ = 0;
	y = LINES - 1;
	x = 0;
	pos = SCR_END - COLUMNS * 2;
}
/* ============================= */
static void put_newline ()
{
	pos = SCR_START + (++y * COLUMNS * 2);
	x = 0;
	if (pos >= SCR_END)
		scroll ();
}
/* ============================= */
static void put_tab ()
{
	pos += TAB_SIZE << 1;
	if (pos >= SCR_END)
		scroll ();
	*(char*)pos = '\t';
	x += TAB_SIZE;
}
/* ============================= */
static void put_normal (int ch, int color)
{
	if (pos + 2 >= SCR_END)
		scroll ();

  *(uint16_t*)pos = ((uint8_t)ch | (((uint8_t)color) << 8));

  pos += 2;
	x++;
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
}
/* =============================== */
void debug_con_puts (const char *str, int color)
{
	char ch;
	while ((ch = *str++) != '\0')
		debug_con_putc (ch, color);
}
/* ============================= */
void change_cursor_pos (int new_x, int new_y)
{
  y = new_y;
  x = new_x;
	pos = SCR_START + (x << 1) + y * (COLUMNS << 1);
}
/* ============================= */
void get_cursor_pos (virt_addr_t xptr, virt_addr_t yptr)
{
	*(uint32_t*)xptr = x;
	*(uint32_t*)yptr = y;
}
/* ============================= */
void debug_con_init ()
{
	y = 0;
	x = 0;
	pos = SCR_START;
}
