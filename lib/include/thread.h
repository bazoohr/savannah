#ifndef __THREAD_H__
#define __THERAD_H__

#include <types.h>
#include <frame.h>

typedef struct thread {
  struct intr_stack_frame *regs;
  struct thread *nxt;
} thread_t;

void thread_create (thread_t *thread,
    void (*routine)(void*),
    void *stack,
    size_t stack_size);
void thread_init (void);

#endif /* __THREAD_H__ */
