#include <avr/io.h>
#include "shielditic.h"

void shielditic_setup(void) {
  /* set semaph pins in output mode */
  DDRC |= 
    _BV(DDC2) |  /* sem2: red */
    _BV(DDC3) ;  /* sem2: yellow */

  DDRD |= 
    _BV(DDD4) |  /* sem2: green */
    _BV(DDD7) |  /* sem1: red */
    _BV(DDD6) |  /* sem1: yellow */
    _BV(DDD5) ;  /* sem1: green */

  /* init all leds to off */
  PORTC &=
    ~_BV(PORTC2) &  /* sem2: red */
    ~_BV(PORTC3) ;  /* sem2: yellow */
    
  PORTD &= 
    ~_BV(PORTD4) &  /* sem2: green */
    ~_BV(PORTD7) &  /* sem1: red */
    ~_BV(PORTD6) &  /* sem1: yellow */
    ~_BV(PORTD5) ;  /* sem1: green */
}


void led_on(led_semaph s, led_color c) {
  switch (s) {
  case semaph1:
    switch (c) {
    case red:
      PORTD |= _BV(PORTD7);
      break;
    case yellow:
      PORTD |= _BV(PORTD6);
      break;
    case green:
      PORTD |= _BV(PORTD5);
      break;
    }
    break;
  case semaph2:
    switch (c) {
    case red:
      PORTC |= _BV(PORTC2);
      break;
    case yellow:
      PORTC |= _BV(PORTC3);
      break;
    case green:
      PORTD |= _BV(PORTD4);
      break;
    }
    break;
  }
}


void led_off(led_semaph s, led_color c) {
  switch (s) {
  case semaph1:
    switch (c) {
    case red:
      PORTD &= ~_BV(PORTD7);
      break;
    case yellow:
      PORTD &= ~_BV(PORTD6);
      break;
    case green:
      PORTD &= ~_BV(PORTD5);
      break;
    }
    break;
  case semaph2:
    switch (c) {
    case red:
      PORTC &= ~_BV(PORTC2);
      break;
    case yellow:
      PORTC &= ~_BV(PORTC3);
      break;
    case green:
      PORTD &= ~_BV(PORTD4);
      break;
    }
    break;
  }
} 

void led_toggle(led_semaph s, led_color c) {
  switch (s) {
  case semaph1:
    switch (c) {
    case red:
      PIND |= _BV(PIND7);
      break;
    case yellow:
      PIND |= _BV(PIND6);
      break;
    case green:
      PIND |= _BV(PIND5);
      break;
    }
    break;
  case semaph2:
    switch (c) {
    case red:
      PINC |= _BV(PINC2);
      break;
    case yellow:
      PINC |= _BV(PINC3);
      break;
    case green:
      PIND |= _BV(PIND4);
      break;
    }
    break;
  }
}


bool led_is_on(led_semaph s, led_color c) {
  switch (s) {
  case semaph1:
    switch (c) {
    case red:
      return PIND & _BV(PIND7);
    case yellow:
      return PIND & _BV(PIND6);
    case green:
      return PIND & _BV(PIND5);
    }
  case semaph2:
    switch (c) {
    case red:
      return PINC & _BV(PINC2);
    case yellow:
      return PINC & _BV(PINC3);
    case green:
      return PINC & _BV(PINC4);
    }
  }
  return 0; /* never reached if all right */
}
