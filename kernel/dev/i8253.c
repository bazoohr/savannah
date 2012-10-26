#include <asmfunc.h>

#define HZ  100               /* We will have 100 ticks per second */
#define TIMER_FREQ  1193182L	/* clock frequency for timer in PC and AT */
#define TIMER_COUNT ((unsigned int) (TIMER_FREQ / HZ)) /* initial value for counter*/

#define SQUARE_WAVE     0x36	/* ccaammmb, a = access, m = mode, b = BCD */
#define TIMER0          0x40	/* I/O port for timer channel 0 */
#define TIMER2          0x42	/* I/O port for timer channel 2 */
#define TIMER_MODE      0x43	/* I/O port for timer mode control */

void
i8253_init (void)
{
	outb (SQUARE_WAVE, TIMER_MODE);      /* Run continuously */
	outb (TIMER_COUNT & 0xFF, TIMER0);   /* LSB */
	outb (TIMER_COUNT >> 8, TIMER0);     /* MSB */
}

void
i8253_stop (void)
{
	outb (SQUARE_WAVE, TIMER_MODE);      /* Run continuously */
	outb (0, TIMER0);   /* LSB */
	outb (0, TIMER0);   /* MSB */
}
