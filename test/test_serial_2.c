#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"



int main() {
  serial_setup();
  sei();

  serial_open();
  _delay_ms(300);
  serial_write('X');
  _delay_ms(300);

  for (;;) {
    char c;
    
    c = serial_read();
    if (c == '\r') {
      serial_write('\r');
      serial_write('\n');
    } else if (c == '\03') {
      break;
    } else
      serial_write(c);
  }

  _delay_ms(300);
  serial_write('\r');
  serial_write('\n');
  serial_write('X');
  _delay_ms(300);

  serial_close();

  return 0;
}
