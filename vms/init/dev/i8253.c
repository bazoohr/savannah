#include <asmfunc.h>

#define ONE_SHOT        0xB0	/* ccaammmb, a = access, m = mode, b = BCD */
#define TIMER0          0x40	/* I/O port for timer channel 0 */
#define TIMER2          0x42	/* I/O port for timer channel 2 */
#define TIMER_MODE      0x43	/* I/O port for timer mode control */
#define CHANNEL2        0x61  /* I/O port for controlling timer channel 2 */
void
i8253_delay (const uint64_t msec)
{
  int i;
  for (i = 0; i < msec; i++) {
    /* Controlling timer channel 2:
     * Bit 7.  RAM parity Error (Read Only)
     * Bit 6.  IO channel Error (Read Only)
     * Bit 5.  Out 2 status (Read Only)
     * Bit 4.  Out 1 status (Read Only)
     * Bit 3.  IO Channel checking enabled. (Read/Write)
     * Bit 2.  RAM parity checking enabled. (Read/Write)
     * Bit 1.  Speaker control (Read/Write)
     * Bit 0.  Gate 2 Control (Read/Write)
     */
    outb(((inb(CHANNEL2) & 0x0C) | 1), CHANNEL2);
    /* write command */
    outb(ONE_SHOT, TIMER_MODE);
    // 1193180 / 100 Hz = 11931 = 0x2e9b  (10 msec)
    // 1193180 / 1000 Hz = 1193 = 0x04A9  (1 msec)
    /* LSB counter */
    outb(0xA9, TIMER2);
    /* MSB counter */
    outb(0x04, TIMER2);
    /* Delay until OUT2 signal (bit 5 in channel 2) is activated */
    while (!(inb(CHANNEL2) & 0x20));
    /* Disable timer channel two */
    outb(inb(CHANNEL2) & 0x0C, CHANNEL2);
  }
}
