#ifndef __TIMER_H__
#define __TIMER_H__

#include <types.h>
uint64_t get_cpu_cycle (void);
void init_timer (uint64_t freq);
void timer_on (void);
void timer_off (void);

#endif /* __TIMER_H__ */
