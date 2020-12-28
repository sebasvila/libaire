#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "pt.h"
#include "pt-delay.h"
#include "ticker.h"
#include "test_fixture.h"
#include "adc.h"

static uint16_t offset;

/*
 * Runs a rotating led at periods of `offset`.  `offset` cannot be 0,
 * otherwise thread do not yields never and becomes non-collaborative,
 * starvating other threads.
 */
PT_THREAD(sem(struct pt *pt))
{
  static uint16_t chronos;

  PT_BEGIN(pt);

  for(;;) {
    led_toggle(semaph1, green);
    /* 
     * To cope with ticker overflow: compare always duration, not
     * timestamp
     */
    chronos = ticker_get();
    PT_WAIT_WHILE(pt, ticker_get() - chronos <= offset);
    led_toggle(semaph1, green);
    
    led_toggle(semaph1, yellow);
    chronos = ticker_get();
    PT_WAIT_WHILE(pt, ticker_get() - chronos <= offset);
    led_toggle(semaph1, yellow);
    
    led_toggle(semaph1, red);
    chronos = ticker_get();
    /* a `<=` guarantees that thread yields although offset == 0 */
    PT_WAIT_WHILE(pt, ticker_get() - chronos <= offset);
    led_toggle(semaph1, red);
  }

  
  
  PT_END(pt);
}


/*
 * Polls potentiometer at 0.1 s freq and updates `offset`
 * accordingly.
 */
PT_THREAD(pot(struct pt *pt))
{
  static adc_channel ch;

  PT_BEGIN(pt);

  ch = adc_bind(POT_CHANNEL, POT_REFERENCE);
  adc_prepare(ch);
  
  for(;;) {
    /* non-blocking adc read from shield itic potentiometer (ch 1) */
    adc_start_oversample();
    PT_WAIT_WHILE(pt, adc_oversampling());
    offset = adc_get_oversample() / 2;
    
    /* polling time of potentiometer */
    PT_DELAY(pt, 3);
  }

  PT_END(pt);
}




int main(void) {
  /* context dels threads */
  struct pt sem_ctx, pot_ctx;

  /* init modules */
  ticker_setup();
  ticker_start();
  fixture_setup();
  adc_setup();
  sei();
  
  /* init contexts */
  PT_INIT(&sem_ctx);
  PT_INIT(&pot_ctx);

  /* read initial position of potentiometer */
  offset = 100;
  
  /* do schedule of threads */
  for(;;) {
    (void)PT_SCHEDULE(sem(&sem_ctx));
    (void)PT_SCHEDULE(pot(&pot_ctx));
  }
}
