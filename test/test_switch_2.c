/* AMB SHIELD TIC */


#include <avr/interrupt.h> 
#include <avr/io.h> 
#include <util/delay.h>
#include "test_fixture.h"
#include "switch.h"



void rotate(int8_t d) {
  static int8_t state = -1;

  switch (state) {
  case -1:
    led_on(semaph2, green);
    state = 0;
    break;
  case 0:
    if (d) {
      led_off(semaph2, green);
      led_on(semaph2, yellow);
      state = 1;
    } else {
      led_off(semaph2, green);
      led_on(semaph2, red);
      state = 2;
    }
    break;
  case 1:
    if (d) {
      led_off(semaph2, yellow);
      led_on(semaph2, red);
      state = 2;
    } else {
      led_off(semaph2, yellow);
      led_on(semaph2, green);
      state = 0;
    }
    break;
  case 2:
    if (d) {
      led_off(semaph2, red);
      led_on(semaph2, green);
      state = 0;
    } else {
      led_off(semaph2, red);
      led_on(semaph2, yellow);
      state = 1;
    }
    break;
  }
}





int main() {
  switch_t s1, s2;
  
  fixture_setup();
  switch_setup();
  sei();

  s1 = switch_bind(&PORTD, 3);
  s2 = switch_bind(&PORTD, 2);
  rotate(0);

  for(;;) {

    switch_poll(s1);
    if (switch_state(s1) && switch_changed(s1)) rotate(1);

    switch_poll(s2);
    if (switch_state(s2) && switch_changed(s2)) rotate(0);

    _delay_ms(50);
  }

  switch_unbind(s1);
  switch_unbind(s2);
  
  return 0;
}
  
  
