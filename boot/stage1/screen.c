#include <types.h>
#include "screen.h"

static int xpos;
static int ypos;
static volatile unsigned char *video = (uint8_t*)VIDEO;

/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void clrscr (void)
{
  int i;

  for (i = 0; i < COLUMNS * LINES * 2; i += 2) {
    *(video + i) = 0;
    *(video + i + 1) = 0xF;
  }

  xpos = 0;
  ypos = 0;
}
/* Put the character C on the screen. */
void putchar (int c)
{
  if (c == '\n' || c == '\r') {
newline:
    xpos = 0;
    ypos++;
    if (ypos >= LINES)
      ypos = 0;
    return;
  }

  *(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
  *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

  xpos++;
  if (xpos >= COLUMNS)
    goto newline;
}

