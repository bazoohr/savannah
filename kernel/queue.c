/* =============================================== *
 * queue.c                                         *
 *                                                 *
 * Hamid R. Bazoobandi                             *
 * Aug. 23 2012 Amsterdam                          *
 * =============================================== */
#include <queue.h>
void 
qinit (queue_t *q)
{
	q->rear = q->front = 0;
}

__warn_unused_result bool
qempty (queue_t *q)
{
	return q->rear == q->front ? true : false;
}
__warn_unused_result void*
qpush (queue_t *q, void *p)
{
	int new_rear;

	new_rear = ((q->rear) + 1) % Q_SIZE;

	if (q->front == new_rear)
		return NULL;

	q->items[q->rear] = p;
	q->rear = new_rear;
	return p;
}

__warn_unused_result void*
qtop (queue_t *q)
{
	return !qempty (q) ? q->items[q->front] : NULL;
}

__warn_unused_result void*
qpop (queue_t *q)
{
	void *p;

	if (qempty (q))
		return NULL;

	p = q->items[q->front];
	q->front = ((q->front) + 1) % Q_SIZE;

	return p;
}
