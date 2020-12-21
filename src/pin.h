#ifndef PIN_H
#define PIN_H

/*
 * A `pin_t` is an abstraction of a pin. When a pin_t variable is
 * declared you have to bind this variable with a physical pin to the
 * AVR microcontroler. This is done with pin_create(). With this
 * function is defined the direction of the pin also. It is possible
 * to read or write the pin value. pin_destroy() is used in order to
 * unbind the pin and leave it as in reset state.
 */
#include <stdint.h>
#include <stdbool.h>


#define PORT(x) (*(x))
#define DDR(x)  (*(x-1))   // consider DDRy = PORTy - 1
#define PIN(x)  (*(x-2))   // consider PINy = PORTy - 2


typedef enum {Input, InputPullup, Output} pin_direction_t;

typedef struct {
  volatile uint8_t *port;
  uint8_t mask;  // the mask of the pin
} pin_t;


/* Create and bind the pin */
pin_t pin_bind(volatile uint8_t *port, uint8_t pin, pin_direction_t d);

/* Write value `v` in pin `p` */
void pin_w(pin_t p, bool v);

/* Read and return value `v` in pin `p` */
inline bool pin_r(pin_t p) {
  return PIN(p.port) & p.mask;
}

/* Sets pin `p` to true */
inline void pin_set_true(pin_t p) {
  PORT(p.port) |= p.mask;
}

/* Sets pin `p` to false */
inline void pin_set_false(pin_t p) {
  PORT(p.port) &= ~(p.mask);
}

/* Toggles value in pin `p` */
inline void pin_toggle(pin_t p) {
  PIN(p.port) |= (p.mask);
}

/* Destroy and unbind the pin */
void pin_unbind(pin_t *const p);





#endif
