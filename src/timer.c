#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "timer.h"


static timer_action_t *action;



void timer_setup(timer_freq_t s) {
  // Configure timer to mode CTC, no output, and no clock
  TCCR1A = 0;
  TCCR1B = _BV(WGM12); 
  TCCR1C = 0;
  // No interrupts from timer
  TIMSK1 = 0;
  // Select 's' prescaler and start counting if not 0
  TCCR1B |= (s & 0x07);
}


extern void timer_arm_once(uint16_t c);


extern void timer_disarm(void) {
  // Disable interrupts
  TIMSK1 = 0;
}


extern bool timer_armed(void);


void timer_set_action(timer_action_t *const a) {
  action = a;
}


/*****************************************************************
 * Interrupt routine
 *****************************************************************/

ISR(TIMER1_COMPA_vect) {
  timer_disarm(); /* only one action done */
  action();
}




/*
 * Comments:
 * 
 * - we can choose to inhibit timer by (a) disabling interrupts
 *   or (b) selecting no clock source. This implementations uses
 *   interrupts to this purpose.
 *
 */
