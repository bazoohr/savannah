#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

void interrupt_init (void);
void add_irq (int vector_idx, void (*handler)(void));

#endif
