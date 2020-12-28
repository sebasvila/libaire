/*
 * Test switch 4 
 *
 * Requires: 
 * - shield TIC in Arduino ONE
 *
 * Runs a rotation of semaph1 leds. Time with no light (nt) and
 * time of led on (lt) can be regulated by a single potentiometer.
 * Potentiometer function is selected by the switch s1. Switch s2
 * sets the potentiometer value.
 */


#include <avr/interrupt.h> 
#include <avr/io.h> 
#include <util/delay.h>
#include "pt.h"
#include "pt-delay.h"
#include "test_fixture.h"
#include "adc.h"
#include "switch.h"


/* centiseconds between led changes */
static uint16_t nt;
/* centiseconds led on */
static uint16_t lt;
/* button automat state */
static int8_t state;


/*
 * Runs a rotating led at periods given by `lt` and `nt`
 */
PT_THREAD(sem(struct pt *pt))
{
  PT_BEGIN(pt);

  for(;;) {
    led_toggle(semaph1, green);
    PT_DELAY(pt, lt);
    led_toggle(semaph1, green);

    PT_DELAY(pt, nt);
    
    led_toggle(semaph1, yellow);
    PT_DELAY(pt, lt);
    led_toggle(semaph1, yellow);
    
    PT_DELAY(pt, nt);

    led_toggle(semaph1, red);
    PT_DELAY(pt, lt);
    led_toggle(semaph1, red);

    PT_DELAY(pt, nt);
  }
  
  PT_END(pt);
}


/*
 *  Polls the potentiometer and changes parameter
 *  according to state when s2 pushed.
 */
#define MAX 300
#define MIN 1
PT_THREAD(ui_pot(struct pt *pt))
{
  static switch_t s2;
  static uint16_t v;
  static adc_channel pot;
  
  PT_BEGIN(pt);

  s2 = switch_bind(&PORTD, 2); // down
  pot = adc_bind(1, Vcc);
  
  for(;;) {
    /* read pushbutton s2 */
    switch_poll(s2);
    PT_WAIT_UNTIL(pt, switch_ready(s2));
    if (switch_state(s2) && switch_changed(s2)) {
      /* read potentiometer and set parameter */
      adc_start_conversion(pot);
      PT_WAIT_WHILE(pt, adc_converting());
      v = ADC_VALUE(adc_get(), MAX, MIN);
      //v = adc_get()/2;
      if (state == 0)
	nt = v;
      else
	lt = v;
    }

    /* polling period */
    PT_DELAY(pt, 5);
  }

  PT_END(pt);
}




PT_THREAD(ui_aut(struct pt *pt))
{
  static switch_t s1;
  static uint8_t i;
  
  PT_BEGIN(pt);

  s1 = switch_bind(&PORTD, 3);
  led_on(semaph2, green); // state=0
  
  for(;;) {
    /* read pushbutton s1 */
    switch_poll(s1);
    PT_WAIT_UNTIL(pt, switch_ready(s1));
    if (switch_state(s1) && switch_changed(s1)) {
      state = (state)?(0):(1);
      // give feedback
      if (state) {
	// new state 1
	for (i=0; i<5; i++) {
	  led_toggle(semaph2, yellow);
	  PT_DELAY(pt, 8);
	}
	led_toggle(semaph2, green);
      } else {
	// new state 0
	for (i=0; i<5; i++) {
	  led_toggle(semaph2, green);
	  PT_DELAY(pt, 8);
	}
	led_toggle(semaph2, yellow);
      }
    }
    /* polling period */
    PT_DELAY(pt, 5);
  }

  PT_END(pt);
}




int main(void) {
  /* context dels threads */
  struct pt sem_ctx, aut_ctx, pot_ctx;

  /* init modules */
  ticker_setup();
  ticker_start();
  fixture_setup();
  switch_setup();
  adc_setup();
  sei();
  
  /* init contexts */
  PT_INIT(&sem_ctx);
  PT_INIT(&pot_ctx);
  PT_INIT(&aut_ctx);  

  /* initial parameters in [1,300] range */
  lt = nt = 100;
  /* initial state */
  state = 0;

  
  /* do schedule of threads */
  for(;;) {
    (void)PT_SCHEDULE(sem(&sem_ctx));
    (void)PT_SCHEDULE(ui_aut(&aut_ctx));
    (void)PT_SCHEDULE(ui_pot(&pot_ctx));
    /* red led to show free time of scheduler */
    led_toggle(semaph2, red);
    _delay_ms(0.5);
    led_toggle(semaph2, red);
  }
}
