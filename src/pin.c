#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>
#include "pin.h"



/* static void disable_all_pullups(void) { */
/*   MCUCR |= _BV(PUD); */
/* } */


pin_t pin_bind(volatile uint8_t *port, uint8_t pin, pin_direction_t d) {
  pin_t p;
  
  /* Construct `pin_t` object.
   * `p`. `p.pin` stores the mask of the pin */
  p.port = port;
  p.mask = _BV(pin);
  /* configure port+pin direction */
  switch (d) {
  case Output:
    DDR(port)  |=  p.mask;
    PORT(port) &= ~p.mask;     // pin low by default
    break;
  case Input:
    DDR(port)  &= ~p.mask;
    PORT(port) &= ~p.mask;     // pull up inactive
    break;
  case InputPullup:
    DDR(port)  &= ~p.mask;
    PORT(port) |=  p.mask;     // pull up active
    break;
  }
  return p;
}

inline void pin_set_true(pin_t p);

inline void pin_set_false(pin_t p);

inline bool pin_r(pin_t p);

void pin_toggle(pin_t p);

void pin_w(pin_t p, bool v) {
  if (v) 
    pin_set_true(p);
  else
    pin_set_false(p);
}


void pin_unbind(pin_t *const p) {
  /* restore port+pin to output mode and low value */
  DDR(p->port)  &= ~(p->mask);
  PORT(p->port) &= ~(p->mask);
  /* next operation will fail unless `p` bound again */
  p->port = NULL;
}

