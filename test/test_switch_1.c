#include <avr/interrupt.h> 
#include <avr/io.h> 
#include <util/delay.h>
#include "test_fixture.h"
#include "switch.h"



int main() {
  switch_t s1, s2;
  
  fixture_setup();
  switch_setup();
  sei();

  s1 = switch_bind(&PORTD, 3);
  s2 = switch_bind(&PORTD, 2);

  for(;;) {

    switch_poll_wait(s1);
    if (switch_state(s1))
      led_on(semaph2, red);

    switch_poll_wait(s2);
    if (switch_state(s2))
      led_off(semaph2, red);

    _delay_ms(50);
  }

  switch_unbind(s1);
  switch_unbind(s2);
  
  return 0;
}
  
  
