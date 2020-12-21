#include <inttypes.h>
#include <stdbool.h>
#include <util/atomic.h>
#include "queue.h"

/*
 * We implement here a circular queue. `front` and `rear` are two
 * indexes pointing to the first element of the queue and to the first
 * empty cell respectively. Then the queue elements fill the cells
 * indexed by the interval [front,rear).
 *
 * There are two special states of the queue to consider:
 *  (a) The queue is empty. In this case front == rear
 *  (b) The queue is full. In this case last+1 (mod L) == rear
 */


static uint8_t inc(uint8_t v) {
  return (++v == QL) ? 0 : v;
}

void queue_empty(queue_t *const q) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    q->front = q->rear = 0;
  }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

bool queue_is_empty(const queue_t *const q) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    return q->front == q->rear;
  }
}

bool queue_is_full(const queue_t *const q) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    return inc(q->rear) == q->front;
  }
}


uint8_t queue_front(const queue_t *const q) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    return  q->t[q->front];
  }
}

#pragma GCC diagnostic pop

void queue_enqueue(queue_t *const q, uint8_t v) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (!queue_is_full(q)) {
      q->t[ q->rear ] = v;
      q->rear = inc(q->rear);
    }
  }
}

void queue_dequeue(queue_t *const q) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (!queue_is_empty(q)) {
      q->front = inc(q->front);
    }
  }
}

