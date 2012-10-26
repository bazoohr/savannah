#ifndef __SCREEN_H
#define __SCREEN_H

#define COLUMNS       80  /* Number of columns */
#define LINES         24  /* Number of lines */
#define ATTRIBUTE     7   /* Attribute */
#define VIDEO         0xB8000  /* Video memory address */

void clrscr (void);
void putchar (int c);

#endif
