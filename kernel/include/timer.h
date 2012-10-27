#ifndef __TIMER_H__
#define __TIMER_H__

#include <types.h>
#include <trap.h>

void do_timer (struct trapframe *tf);
void timer_delay (const uint64_t msec);

#endif /* __TIMER_H__ */
