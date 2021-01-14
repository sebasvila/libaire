#ifndef QUEUE_H
#define QUEUE_H

/* 
 * This module implements a syncronized queue of i2c requests.
 * This queues is to be used by the i2c low level driver.
 * Operations are guaranteed to be atomic.
 */

#include <inttypes.h>
#include <stdbool.h>
#include "i2cr.h"

/* the queue max length */
#define I2CQ_L (10)


/* the queue structure */
typedef struct {
  i2cr_request_t t[I2CQ_L];
  uint8_t front, rear;
} i2cq_t;


/* Initializes `q` to empty  */
void i2cq_empty(i2cq_t *const q);

/* Returns true iff `q` is empty */
bool i2cq_is_empty(const i2cq_t *const q);

/* Return true iff `q` is full */
bool i2cq_is_full(const i2cq_t *const q);

/* Adds `v` to `q`. If `q` is full nothing is added */
void i2cq_enqueue(i2cq_t *const q, const i2cr_request_t *const v);

/* Remove the front element of `q`. 
 * If `q` is empty nothing is removed.
 */
void i2cq_dequeue(i2cq_t *const q);

/* 
 * Gets the front of `q`. 
 *
 * @returns A pointer to the i2c request in the queue front. This object must
 *          be considered constant and must not be modified. 
 *          If `q` is empty, the return value is undefined.
 */
const i2cr_request_t *i2cq_front(const i2cq_t *const q);


#endif
