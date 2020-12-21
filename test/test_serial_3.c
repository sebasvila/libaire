#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "pt.h"
#include "pt-delay.h"
#include "ticker.h"
#include "adc.h"
#include "serial.h"
#include "config.h"

static uint16_t offset;

/*
 * Sends a sequence of chars by the serial port at a frequence given by
 * the potentiometer
 */
PT_THREAD(write(struct pt *pt))
{
  static uint8_t i;
    
  PT_BEGIN(pt);

  serial_open();
  i = 0;

  for(;;) {
    PT_DELAY(pt, offset);
    PT_WAIT_UNTIL(pt, serial_can_write());
    serial_write('A'+i);
    if (i==25) {
      PT_WAIT_UNTIL(pt, serial_can_write());
      serial_write('\r');
      PT_WAIT_UNTIL(pt, serial_can_write());
      serial_write('\n');;
      i = 0;
    } else
      i++;
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
    adc_start_conversion();
    PT_WAIT_WHILE(pt, adc_converting());
    offset = adc_get() / 2;
    
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
