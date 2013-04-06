#ifndef __TIMER_H__
#define __TIMER_H__

#include <types.h>
void init_timer (void);
void timer_on (uint64_t ms);
void timer_off (void);

#endif /* __TIMER_H__ */
