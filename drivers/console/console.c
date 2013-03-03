/* ======================================== *
 * console.c                                *
 *                                          *
 * Hamid R. Bazoobandi 2009                 *
 * a VERY simple driver for console         *
 * ======================================== */
#include <console.h>
#include <types.h>
#include <asmfunc.h>
/* ============================= */
static uint64_t y;
static uint64_t x;
/* ============================= */
// TODO: Fix this! It does not seems to work.
static void con_cursor ()
{
  uint16_t loc = (y * 80) + x;

  outb(0x0F, 0x3D4);
  outb((unsigned char)(loc & 0xFF), 0x3D5);
  outb(0x0E, 0x3D4);
  outb((unsigned char )((loc >> 8) & 0xFF), 0x3D5);
}
/* ============================= */
#if 0
static void con_clear ()
{
  int i;
  uint16_t *vga = (uint16_t*)SCR_START;

  for (i = 0 ; i < LINES * COLUMNS ; i++) {
    vga[i] = ' ';
  }
}
#endif
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
      vga[i] = ' ';
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
void putc (int ch, int color)
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
void puts (const char *str, int color)
{
  char ch;
  while ((ch = *str++) != '\0')
    putc (ch, color);
}
/* ============================= */
void con_init (uint32_t xpos, uint32_t ypos)
{
  y = ypos;
  x = xpos;
  con_cursor ();
}
