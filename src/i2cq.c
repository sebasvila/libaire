#include <inttypes.h>
#include <stdbool.h>
#include "i2cq.h"

/*
 * We implement here a circular queue. `front` and `rear` are two
 * indexes pointing to the first element of the queue and to the first
 * empty cell respectively. Then the queue elements fill the cells
 * indexed by the interval [front,rear).
 *
 * There are two special states of the queue to consider:
 *  (a) The queue is empty. In this case front == rear
 *  (b) The queue is full. In this case last+1 (mod L) == rear
 *
 * Queue is meant to be used exclusively in the i2c module. Therefore,
 * atomic sections correctness should be considered only for this
 * working context. **Other uses will require a review of them.**
 */


static uint8_t inc(uint8_t v) {
  return (++v == I2CQ_L) ? 0 : v;
}

void i2cq_empty(i2cq_t *const q) {
  q->front = q->rear = 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

bool i2cq_is_empty(const i2cq_t *const q) {
  return q->front == q->rear;
}

bool i2cq_is_full(const i2cq_t *const q) {
  return inc(q->rear) == q->front;
}


const i2cr_request_t *i2cq_front(const i2cq_t *const q) {
  return &(q->t[q->front]);
}

#pragma GCC diagnostic pop

void i2cq_enqueue(i2cq_t *const q, const i2cr_request_t *const v) {
  if (!i2cq_is_full(q)) {
    q->t[q->rear] = *v;
    q->rear = inc(q->rear);
  }
}

void i2cq_dequeue(i2cq_t *const q) {
  if (!i2cq_is_empty(q)) {
    q->front = inc(q->front);
  }
}

