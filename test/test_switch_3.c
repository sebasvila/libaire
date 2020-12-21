/* AMB SHIELD TIC */


#include <avr/interrupt.h> 
#include <avr/io.h> 
#include <util/delay.h>
#include "pt.h"
#include "pt-delay.h"
#include "ticker.h"
#include "shielditic.h"
#include "switch.h"

/* ticks (10ms) between led changes */
static uint16_t offset;


/*
 * Runs a rotating led at periods of `offset`.  `offset` cannot be 0,
 * otherwise thread do not yields never and becomes non-collaborative,
 * starvating other threads.
 */
PT_THREAD(sem(struct pt *pt))
{
  PT_BEGIN(pt);

  for(;;) {
    led_toggle(semaph1, green);
    /* 
     * To cope with ticker overflow: compare always duration, not
     * timestamp
     */
    PT_DELAY(pt, offset);
    led_toggle(semaph1, green);
    
    led_toggle(semaph1, yellow);
    PT_DELAY(pt, offset);
    led_toggle(semaph1, yellow);
    
    led_toggle(semaph1, red);
    /* a `<=` guarantees that thread yields although offset == 0 */
    PT_DELAY(pt, offset);
    led_toggle(semaph1, red);
  }
  
  PT_END(pt);
}


/*
 * Polls two switches acting as a regulator
 * 50ms <= offset <= 600ms 
 */
#define MAX 60
#define MIN 1
PT_THREAD(switch_pot(struct pt *pt))
{
  static switch_t s1, s2;
  
  PT_BEGIN(pt);

  s1 = switch_bind(&PORTD, 3); // up
  s2 = switch_bind(&PORTD, 2); // down
  offset = (MIN+MAX)/2;
  
  for(;;) {
    /* read up button */
    if (offset < MAX) {
      switch_poll(s1);
      PT_WAIT_UNTIL(pt, switch_ready(s1));
      if (switch_state(s1)) offset++;
    }

    /* read down button */
    if (offset > MIN) {
      switch_poll(s2);
      PT_WAIT_UNTIL(pt, switch_ready(s2));
      if (switch_state(s2)) offset--;
    }

    /* polling time of switches */
    PT_DELAY(pt, 10);
  }

  PT_END(pt);
}




int main(void) {
  /* context dels threads */
  struct pt sem_ctx, switch_pot_ctx;

  /* init modules */
  ticker_setup();
  ticker_start();
  shielditic_setup();
  switch_setup();
  sei();
  
  /* init contexts */
  PT_INIT(&sem_ctx);
  PT_INIT(&switch_pot_ctx);

  /* read initial position of potentiometer */
  offset = 100;
  
  /* do schedule of threads */
  for(;;) {
    (void)PT_SCHEDULE(sem(&sem_ctx));
    (void)PT_SCHEDULE(switch_pot(&switch_pot_ctx));
    led_toggle(semaph2, green);
  }
}
