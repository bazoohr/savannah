#include <asmfunc.h>
#include <panic.h>
#include <printk.h>
#include <cdef.h>
#include <dev/ioapic.h>
#include <dev/lapic.h>
#include <isr.h>
#include <vuos/config.h>

#define NOP 0
#define KBD_DATA_PORT  0x60

#define	KBD_STAT_PORT		0x64	/* kbd controller status port(I) */
#define	KBD_STAT_DATA_IN_BUF	0x01	/* kbd data in buffer */

#define BUFMAX   64

static int buffer[BUFMAX];
static int bufpos = 0;
static int keymap[][8] = {
/*                                                       alt
 scan                        cntrl          alt    alt   cntrl
 code   base   shift  cntrl  shift   alt   shift  cntrl  shift
 ----------------------------------------------------------------- */
/*00*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },
/*01*/{ 0x1B,  0x1B,  0x1B,  0x1B,  0x1B,  0x1B,   NOP,  0x1B, },
/*02*/{  '1',   '!',   NOP,   NOP,   '1',   '!',   NOP,   NOP, },
/*03*/{  '2',   '@',  0x00,  0x00,   '2',   '@',  0x00,  0x00, },
/*04*/{  '3',   '#',   NOP,   NOP,   '3',   '#',   NOP,   NOP, },
/*05*/{  '4',   '$',   NOP,   NOP,   '4',   '$',   NOP,   NOP, },
/*06*/{  '5',   '%',   NOP,   NOP,   '5',   '%',   NOP,   NOP, },
/*07*/{  '6',   '^',  0x1E,  0x1E,   '6',   '^',  0x1E,  0x1E, },
/*08*/{  '7',   '&',   NOP,   NOP,   '7',   '&',   NOP,   NOP, },
/*09*/{  '8',   '*',   NOP,   NOP,   '8',   '*',   NOP,   NOP, },
/*0a*/{  '9',   '(',   NOP,   NOP,   '9',   '(',   NOP,   NOP, },
/*0b*/{  '0',   ')',   NOP,   NOP,   '0',   ')',   NOP,   NOP, },
/*0c*/{  '-',   '_',  0x1F,  0x1F,   '-',   '_',  0x1F,  0x1F, },
/*0d*/{  '=',   '+',   NOP,   NOP,   '=',   '+',   NOP,   NOP, },
/*0e*/{ 0x08,  0x08,  0x7F,  0x7F,  0x08,  0x08,  0x7F,  0x7F, },/* bksp  */
/*0f*/{ 0x09,  0xF00,  NOP,   NOP,  0x09, 0xF00,   NOP,   NOP, },/*  Tab  */
/*10*/{  'q',   'Q',  0x11,  0x11,   'q',   'Q',  0x11,  0x11, },
/*11*/{  'w',   'W',  0x17,  0x17,   'w',   'W',  0x17,  0x17, },
/*12*/{  'e',   'E',  0x05,  0x05,   'e',   'E',  0x05,  0x05, },
/*13*/{  'r',   'R',  0x12,  0x12,   'r',   'R',  0x12,  0x12, },
/*14*/{  't',   'T',  0x14,  0x14,   't',   'T',  0x14,  0x14, },
/*15*/{  'y',   'Y',  0x19,  0x19,   'y',   'Y',  0x19,  0x19, },
/*16*/{  'u',   'U',  0x15,  0x15,   'u',   'U',  0x15,  0x15, },
/*17*/{  'i',   'I',  0x09,  0x09,   'i',   'I',  0x09,  0x09, },
/*18*/{  'o',   'O',  0x0F,  0x0F,   'o',   'O',  0x0F,  0x0F, },
/*19*/{  'p',   'P',  0x10,  0x10,   'p',   'P',  0x10,  0x10, },
/*1a*/{  '[',   '{',  0x1B,  0x1B,   '[',   '{',  0x1B,  0x1B, },
/*1b*/{  ']',   '}',  0x1D,  0x1D,   ']',   '}',  0x1D,  0x1D, },
/*1c*/{ 0x0A,  0x0A,  0x0A,  0x0A,  0x0A,  0x0A,  0x0A,  0x0A, },
/*1d*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },/*  Ctrl  */
/*1e*/{  'a',   'A',  0x01,  0x01,   'a',   'A',  0x01,  0x01, },
/*1f*/{  's',   'S',  0x13,  0x13,   's',   'S',  0x13,  0x13, },
/*20*/{  'd',   'D',  0x04,  0x04,   'd',   'D',  0x04,  0x04, },
/*21*/{  'f',   'F',  0x06,  0x06,   'f',   'F',  0x06,  0x06, },
/*22*/{  'g',   'G',  0x07,  0x07,   'g',   'G',  0x07,  0x07, },
/*23*/{  'h',   'H',  0x08,  0x08,   'h',   'H',  0x08,  0x08, },
/*24*/{  'j',   'J',  0x0A,  0x0A,   'j',   'J',  0x0A,  0x0A, },
/*25*/{  'k',   'K',  0x0B,  0x0B,   'k',   'K',  0x0B,  0x0B, },
/*26*/{  'l',   'L',  0x0C,  0x0C,   'l',   'L',  0x0C,  0x0C, },
/*27*/{  ';',   ':',   NOP,   NOP,   ';',   ':',   NOP,   NOP, },
/*28*/{ '\'',   '"',   NOP,   NOP,  '\'',   '"',   NOP,   NOP, },
/*29*/{  '`',   '~',   NOP,   NOP,   '`',   '~',   NOP,   NOP, },
/*2a*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },/* Lshift */
/*2b*/{ '\\',   '|',  0x1C,  0x1C,  '\\',   '|',  0x1C,  0x1C, },
/*2c*/{  'z',   'Z',  0x1A,  0x1A,   'z',   'Z',  0x1A,  0x1A, },
/*2d*/{  'x',   'X',  0x18,  0x18,   'x',   'X',  0x18,  0x18, },
/*2e*/{  'c',   'C',  0x03,  0x03,   'c',   'C',  0x03,  0x03, },
/*2f*/{  'v',   'V',  0x16,  0x16,   'v',   'V',  0x16,  0x16, },
/*30*/{  'b',   'B',  0x02,  0x02,   'b',   'B',  0x02,  0x02, },
/*31*/{  'n',   'N',  0x0E,  0x0E,   'n',   'N',  0x0E,  0x0E, },
/*32*/{  'm',   'M',  0x0D,  0x0D,   'm',   'M',  0x0D,  0x0D, },
/*33*/{  ',',   '<',   NOP,   NOP,   ',',   '<',   NOP,   NOP, },
/*34*/{  '.',   '>',   NOP,   NOP,   '.',   '>',   NOP,   NOP, },
/*35*/{  '/',   '?',   NOP,   NOP,   '/',   '?',   NOP,   NOP, },
/*36*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },/* Rshift */
/*37*/{  '*',   '*',   '*',   '*',   '*',   '*',   '*',   '*', },/*   *    */
/*38*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },/*  alt   */
/*39*/{  ' ',   ' ',  0x00,   ' ',   ' ',   ' ',   NOP,   ' ', },/* space  */
/*3a*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },/*  caps  */
/*3b*/{0x3B00,0x5400,0x5E00,  NOP, 0x6800,  NOP,   NOP,   NOP, },/*   F1   */
/*3c*/{0x3C00,0x5500,0x5F00,  NOP, 0x6900,  NOP,   NOP,   NOP, },/*   F2   */
/*3d*/{0x3D00,0x5600,0x6000,  NOP, 0x6A00,  NOP,   NOP,   NOP, },/*   F3   */
/*3e*/{0x3E00,0x5700,0x6100,  NOP, 0x6B00,  NOP,   NOP,   NOP, },/*   F4   */
/*3f*/{0x3F00,0x5800,0x6200,  NOP, 0x6C00,  NOP,   NOP,   NOP, },/*   F5   */
/*40*/{0x4000,0x5900,0x6300,  NOP, 0x6D00,  NOP,   NOP,   NOP, },/*   F6   */
/*41*/{0x4100,0x5A00,0x6400,  NOP, 0x6E00,  NOP,   NOP,   NOP, },/*   F7   */
/*42*/{0x4200,0x5B00,0x6500,  NOP, 0x6F00,  NOP,   NOP,   NOP, },/*   F8   */
/*43*/{0x4300,0x5C00,0x6600,  NOP, 0x7000,  NOP,   NOP,   NOP, },/*   F9   */
/*44*/{0x4400,0x5D00,0x6700,  NOP, 0x7100,  NOP,   NOP,   NOP, },/*  F10   */
/*45*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },/*  NUM   */
/*46*/{  NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP,   NOP, },/*  SCR   */
/*47*/{0x4700,  '7',   '7',   '7',   '7',   '7',   '7',   '7', },/*  HOME  */
/*48*/{0x4800,  '8',   '8',   '8',   '8',   '8',   '8',   '8', },/*  up    */
/*49*/{0x4900,  '9',   '9',   '9',   '9',   '9',   '9',   '9', },/* pgup   */
/*4a*/{   '-',  '-',   '-',   '-',   '-',   '-',   '-',   '-', },/*   -    */
/*4b*/{0x4B00,  '4',   '4',   '4',   '4',   '4',   '4',   '4', },/* Left   */
/*4c*/{0x4C00,  '5',   '5',   '5',   '5',   '5',   '5',   '5', },/* Center */
/*4d*/{0x4D00,  '6',   '6',   '6',   '6',   '6',   '6',   '6', },/* Right  */
/*4e*/{   '+',  '+',   '+',   '+',   '+',   '+',   '+',   '+', },/*   +    */
/*4f*/{0x4F00,  '1',   '1',   '1',   '1',   '1',   '1',   '1', },/*  End   */
/*50*/{0x5000,  '2',   '2',   '2',   '2',   '2',   '2',   '2', },/*  down  */
/*51*/{0x5100,  '3',   '3',   '3',   '3',   '3',   '3',   '3', },/*  pgdn  */
/*52*/{0x5200,  '0',   '0',   '0',   '0',   '0',   '0',   '0', },/*  ins   */
/*53*/{0x5300,  '.',   '.',   '.',   '.',   '.',   NOP,   NOP, },/*  del   */
};

static void __inline
kbd_add_buf (int data)
{
  if (bufpos >= BUFMAX) {
    bufpos = 0;
  }
  buffer[bufpos++] = data;
  cprintk ("Keyboard: %c\n", 0xE, data);
}

void
kbd_proc_data (void)
{
  static bool shift_pressed = false;
  static bool ctrl_pressed  = false;
  static bool alt_pressed   = false;

  int scan_code;
  int asci_code;

  if ((inb (KBD_STAT_PORT) & KBD_STAT_DATA_IN_BUF) == 0) {
    panic ("Data can't be read from keyboard buffer\n");
  }

  scan_code = inb (KBD_DATA_PORT);

  if (scan_code == 0xE0)
	  goto eoi;

  switch (scan_code) {
    case 0x1D:
      ctrl_pressed = true;
      goto eoi;
    case 0x38:
      alt_pressed = true;
      goto eoi;
    case 0x2A:
    case 0x36:
      shift_pressed = true;
      goto eoi;
    case 0x1D | 0x80:
      ctrl_pressed = false;
      goto eoi;
    case 0x38 | 0x80:
      alt_pressed = false;
      goto eoi;
    case 0x2A | 0x80:
    case 0x36 | 0x80:
      shift_pressed = false;
      goto eoi;
    default:
      break;
  }

  if (alt_pressed && ctrl_pressed && scan_code == 0x53) {  /* ALT + CTRL + DEL */
    halt ();
  }

  if (scan_code & 0x80)
	  goto eoi;

  asci_code = keymap[scan_code][(ctrl_pressed<<1) | (shift_pressed) | (alt_pressed<<2)];
  kbd_add_buf (asci_code);

eoi:
  /* Set the EOI for local APIC */
  lapic_eoi();
  return;
}

void kbd_init (void)
{
  ioapic_enable(IRQ_KBD, KBD);
}
