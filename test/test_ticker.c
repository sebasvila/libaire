#include <stdbool.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ticker.h"
#include "shielditic.h"


int main() {
  uint16_t ticks;
  
  ticker_setup();
  shielditic_setup();
  sei();

  /* signal start of test */
  _delay_ms(1000);
  led_toggle(semaph2,red);
  _delay_ms(400);
  led_toggle(semaph2,red);
  _delay_ms(400);
  led_toggle(semaph2,red);
  _delay_ms(400);
  led_toggle(semaph2,red);

  /* test */
  ticker_start();
  led_on(semaph1, green);
  ticks = ticker_get() + ticker_tps()/2;
  while (ticker_get() < ticks);
  led_on(semaph1, yellow);
  ticks = ticker_get() + 1*ticker_tps();
  while (ticker_get() < ticks);
  led_on(semaph1, red);
  ticks = ticker_get() + 2*ticker_tps();
  while (ticker_get() < ticks);
  led_off(semaph1, green);
  led_off(semaph1, yellow);
  led_off(semaph1, red);

  for (uint8_t i=0; i<100; i++) {
    led_toggle(semaph1, yellow);
    ticks = ticker_get() + ticker_tps()/10;
    while (ticker_get() < ticks);
  }
  led_off(semaph1, yellow);


  for (uint8_t i=0; i<200; i++) {
    led_toggle(semaph1, yellow);
    ticks = ticker_get() + ticker_tps()/20;
    while (ticker_get() < ticks);
  }
  led_off(semaph1, yellow);


  for (uint8_t i=0; i<200; i++) {
    led_toggle(semaph1, yellow);
    ticks = ticker_get() + ticker_tps()/40;
    while (ticker_get() < ticks);
  }
  led_off(semaph1, yellow);

  
  ticker_stop();
  





  /* signal end of test */
  _delay_ms(1000);
  led_toggle(semaph2,red);
  _delay_ms(400);
  led_toggle(semaph2,red);
  _delay_ms(400);
  led_toggle(semaph2,red);
  _delay_ms(400);
  led_toggle(semaph2,red);


  return 0;
}
