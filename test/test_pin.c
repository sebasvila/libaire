#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pin.h"





int main() {
#if defined(ArduinoONE)
  pin_t led = pin_bind(&PORTB, 5, Output);
#elif defined(ArduinoMEGA)
  pin_t led = pin_bind(&PORTB, 7, Output);
#endif

  for(;;) {
    _delay_ms(1000);
    pin_set_true(led);
    _delay_ms(1000);
    pin_set_false(led);
  }

  return 0;
}
  

  
