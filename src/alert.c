#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pin.h"
#include "config.h"
#include "alert.h"



#define DASH  1600
#define DOT    400
#define BLACK  450



static pin_t fatal_alert_led;


static void dash(void) {
  pin_w(fatal_alert_led, true);
  _delay_ms(DASH);
  pin_w(fatal_alert_led, false);
}


static void dot(void) {
  pin_w(fatal_alert_led, true);
  _delay_ms(DOT);
  pin_w(fatal_alert_led, false);
}



void alert_fatal(uint8_t e) {
  /* non return function */
  cli();
  /* reconfig port with no care to previous uses */
  fatal_alert_led = pin_bind(&FATAL_ALERT_PORT,
			     FATAL_ALERT_PIN,
			     Output);
  for(;;) {
    dash(); _delay_ms(BLACK); dash();
    _delay_ms(BLACK);
    for (uint8_t i=0; i<e; i++) {
      dot(); _delay_ms(BLACK);
    }
    _delay_ms(BLACK*2); 
  }
}
