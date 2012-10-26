#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <types.h>
#include <cdef.h>

#define Q_SIZE 10

typedef struct {
  void *items[Q_SIZE];
  int front;
  int rear;
} queue_t;


void qinit (queue_t *q);
__warn_unused_result bool qempty (queue_t *q);
__warn_unused_result void *qpush (queue_t *q, void *p);
__warn_unused_result void *qtop (queue_t *q);
__warn_unused_result void *qpop (queue_t *q);

#endif /* __QUEUE_H__ */
