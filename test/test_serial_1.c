#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"



int main() {
  serial_setup();
  sei();

  serial_open();
  serial_write('X');
  _delay_ms(300);
  for (;;) {
    serial_write('A');
    _delay_ms(300);
    serial_write('a');
    _delay_ms(300);
  }
  serial_close();

  return 0;
}
