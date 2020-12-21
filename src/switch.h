#ifndef _SWITCH_H_
#define _SWITCH_H_

/*
 * switch - manage two states switches of any kind
 *
 * Characteristics:
 * (1) User queries a switch to know its state (high,low) 
 *     whenever he needs. Switch triggers no interrupts.
 * (2) Queries are debounced by sampling switch state until
 *     stable. Sampling properties configurable in implementation.
 * (3) Sampling are conduced by TIMER0 interrupts. No blocked 
 *     waiting involved.
 * (4) Queries return two results: (a) switch state and (b) whether 
 *     state changed from last query.
 */

#include <stdint.h>
#include <stdlib.h>

/* `switch_t` is a handler to internal switch */
typedef void* switch_t;

/* Some functions can return error */
#define SWITCH_ERR NULL

/* setup the module */
void switch_setup(void);

/* 
 * Binds a switch object to a port+pin.
 * All query operations require a switch to be bound.
 * Returns `SWITCH_ERR` if no more switches allowed.
 */
switch_t switch_bind(volatile uint8_t *port, uint8_t pin);
/* 
 * Unbinds a switch object.
 * Post: port+pin in out mode and low state.
 */
void switch_unbind(switch_t i);


/*
 * Query operations.
 * Usage: (1) poll; (2) wait for answer available; (3) query results.
 *        (1) poll and wait; (2) query results.
 *
 * Rationale: 
 * You first instruct to poll the device. When answer is
 * available (answer is stable), you can ask for the switch status 
 * (get), or you can ask if the switch status changed 
 * from last time queried.
 * This allows for several useful patterns:
 * (a) Query the current status: get()
 * (b) Query if an edge (raising o falling) detected since last query:
 *     changed()
 * (c) Query if a rising edge occurred: state() && changed()
 * (d) Query id a falling edge occurred: !get() && changed()
 */

/* Instructs to query the switch `i` */
void switch_poll(switch_t i);
/* Returns `true` iff switch `i` results available after last poll */ 
bool switch_ready(switch_t i);
/* Poll the switch `i` and wait answer available. CPU bound waiting */
void switch_poll_wait(switch_t i);
/* Get the switch `i` state. Use only if switch results available */
bool switch_state(switch_t i);
/* Query of switch `i` state changed. 
 * Use only if switch results available */
bool switch_changed(switch_t i);


#endif
