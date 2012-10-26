#ifndef __THREAD_H__
#define __THREAD_H__

#include <cdef.h>

int thread_create (void * (*thread_routine)(void *args), void *args);
void thread_join (int tid);

#endif  /* __THREDA_H__ */
