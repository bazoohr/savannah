#ifndef __THREAD_H__
#define __THERAD_H__

#include <types.h>
#include <frame.h>

typedef struct thread {
  struct intr_stack_frame *regs;
  struct thread *nxt;
} thread_t;

extern thread_t *head;
extern thread_t *last;
extern thread_t *current;

void thread_create (thread_t *thread,
    void (*routine)(void*),
    void *stack,
    size_t stack_size);
void thread_set_scheduler (thread_t *(*new_scheduler)(void));
void thread_set_timer_freq (uint64_t ms);
void thread_init (void);

#endif /* __THREAD_H__ */
