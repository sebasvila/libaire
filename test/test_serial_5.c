#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "pt.h"
#include "pt-delay.h"
#include "ticker.h"
#include "adc.h"
#include "serial.h"
#include "config.h"

static uint8_t offset;

/*
 * Writes the ADC value given by the potentiometer
 * when changes
 */
PT_THREAD(write(struct pt *pt))
{
  static uint8_t last_offset=0;
    
  PT_BEGIN(pt);

  serial_open();

  for(;;) {
    PT_WAIT_WHILE(pt, last_offset == offset);
    PT_WAIT_UNTIL(pt, serial_can_write());
    serial_write_ui(offset);
    last_offset = offset;
    PT_WAIT_UNTIL(pt, serial_can_write());
    serial_eol();
  }

  serial_close();
  
  
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
    offset = adc_get_oversample() >> 1;
    
    /* polling time of potentiometer */
    PT_DELAY(pt, 3);
  }

  PT_END(pt);
}




int main(void) {
  /* context dels threads */
  struct pt write_ctx, pot_ctx;

  /* init modules */
  ticker_setup();
  serial_setup();
  adc_setup();
  sei(); 

  ticker_start();
 
  /* init contexts */
  PT_INIT(&write_ctx);
  PT_INIT(&pot_ctx);

  /* read initial position of potentiometer */
  offset = 100;
  
  /* do schedule of threads */
  for(;;) {
    (void)PT_SCHEDULE(write(&write_ctx));
    (void)PT_SCHEDULE(pot(&pot_ctx));
  }
}
