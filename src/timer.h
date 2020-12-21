#ifndef _TIMER_H_
#define _TIMER_H_

/*
 * Abstracts a basic hardware timer
 *
 * After setting up the timer with clock at a given freq (in clock pulses per s)
 * an action can be installed. This action will be called:
 *  -  If we call `timer_arm_once(c)`, action will be 
 *     called a single time after `c` clock pulses. Timer will be
 *     automatically disarmed after this cycle.
 */

#include <stdbool.h>
#include <stdint.h>

/* timer freq in Hz */
typedef enum {t0=0, t16000000=1, t2000000=2,
	      t250000=3, t62500=4, t15625=5} timer_freq_t;

/* timer action type */
typedef void timer_action_t(void);



/* sets up the timer with the clock at `f` frequency */
void timer_setup(timer_freq_t s);

/* arm the timer to fire action one time after `c` counts */
inline void timer_arm_once(uint16_t c) {
  // Configure MAX=`c`. 16b register with special access order
  OCR1A = c;
  // Clear counter
  TCNT1 = UINT16_C(0);
  // Reset the clock to synchronize. Avoids sync error
  // this impacts to TIMER 0 and TIMER 1 !!
  GTCCR = _BV(PSRSYNC); 
  // Enable interrupts
  TIMSK1 = _BV(OCIE1A);
}


/* disarm the timer */
void timer_disarm(void);

/* returns true if timer is armed */
inline bool timer_armed(void) {
  return TIMSK1 & _BV(OCIE1A);
}


/* set the action to be fired by the timer */
void timer_set_action(timer_action_t a);


#endif
