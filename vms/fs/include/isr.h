#ifndef __ISR_H__
#define __ISR_H__

#define IRQ_TIMER  0         /* Timer: interrupt line number 0 */
#define IRQ_KBD    1         /* Keyboard: interrupt line number 1 */
#define IRQ_OFFSET 32

void timer_handler (void);
void kbd_handler (void);

#endif
