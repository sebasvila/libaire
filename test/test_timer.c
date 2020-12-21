#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "pin.h"


static pin_t led;


static void toggle_led(void) {
  pin_toggle(led);
}



int main() {
  uint16_t steps;
  
  timer_setup(t15625);
  led = pin_bind(&PORTB, 5, Output);
  sei();

  _delay_ms(1000);

  steps = 20000;
  timer_set_action(toggle_led);
  pin_toggle(led);
  while (steps > 10) {
    timer_arm_once(steps); 
    steps -= 200;
    while (timer_armed());
  }


  return 0;
}
  

  
