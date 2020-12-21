#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "pt.h"
#include "timer.h"
#include "pin.h"



static pin_t led;

static void toggle_led(void) {
  pin_toggle(led);
}


PT_THREAD(led1(struct pt *pt))
{
  static uint16_t steps;

  PT_BEGIN(pt);

  led = pin_bind(&PORTB, 5, Output);
  _delay_ms(1000);
  steps = 1000;
  timer_set_action(toggle_led);
  pin_toggle(led);
  for(;;) {
    timer_arm_once(steps); 
    PT_WAIT_WHILE(pt, timer_armed());
  }

  PT_END(pt);
}




int main(void) {
  /* context dels threads */
  struct pt context_led1;

  /* init modules */
  timer_setup(t15625);
  sei();
  
  /* init contexts */
  PT_INIT(&context_led1);

  /* do schedule */
  for(;;) {
    (void)PT_SCHEDULE(led1(&context_led1));
  }
}
