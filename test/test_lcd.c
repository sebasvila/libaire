#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "charmaps.h"

/*
#define LBL_PEAK "PEAK:"
#define LBL_PLATEAU "PLATEAU:"
#define LBL_VOLUME "VOLUME |"
#define LBL_RATE "I:R |"
#define LBL_FREQUENCY "FREQ."
#define LBL_DIVIDERS "       |     |"
*/

int main() {
  lcd_t lcd = lcd_bind(&PORTD,4,5);
  lcd_on(lcd);
  lcd_disable_autoscroll(lcd);
  lcd_clear(lcd);

  lcd_create_char(lcd,0,U_char);
  lcd_create_char(lcd,1,P_char);
  lcd_create_char(lcd,2,C_char);
  lcd_move_cursor(lcd,2,1);
  lcd_send(lcd,LCD_DATA,0); //U
  lcd_send(lcd,LCD_DATA,1); //P
  lcd_send(lcd,LCD_DATA,2); //C
  lcd_move_cursor(lcd,2,2);
  lcd_print(lcd, "EPSEM RESPIRATOR");

  for(;;);

  return 0;
}


/*
void setLabels(void){
  lcd_clear();
  lcd_return_home();
  lcd_print(LBL_PEAK);
  lcd_set_cursor(0,1);
  lcd_print(LBL_PLATEAU);
  lcd_set_cursor(0,2);
  lcd_print(LBL_VOLUME);
  lcd_set_cursor(9,2);
  lcd_print(LBL_RATE);
  lcd_set_cursor(15,2);
  lcd_print(LBL_FREQUENCY);
  lcd_set_cursor(0,3);
  lcd_print(LBL_DIVIDERS);
}

void setVolume(uint16_t volume) {
  lcd_set_cursor(1,3);
  lcd_printf("%i",volume);
}

void setRatio(uint8_t ratio) {
  lcd_set_cursor(9,3);
  lcd_printf("1:%i",ratio);
}

void setFrequency(uint8_t freq) {
  lcd_set_cursor(16,3);
  lcd_printf("%i",freq);
}

void setPeak(uint8_t peak) {
  lcd_set_cursor(6,0);
  lcd_printf("%i",peak);
}

void setPlateau(uint8_t plateau) {
  lcd_set_cursor(9,1);
  lcd_printf("%i",plateau);
}

void blinkingVolume(void){
  for (uint8_t i = 0; i < 5; i++) {
    lcd_set_cursor(1,3);
    lcd_write(255);
    lcd_write(255);
    lcd_write(255);
    _delay_ms(500);
    setVolume(420);
    _delay_ms(500);
  }
}

void blinkingRatio(void){
  for (uint8_t i = 0; i < 5; i++) {
    lcd_set_cursor(11,3);
    lcd_write(255);
    _delay_ms(500);
    setRatio(2);
    _delay_ms(500);
  }
}

void blinkingFreq(void){
  for (uint8_t i = 0; i < 5; i++) {
    lcd_set_cursor(16,3);
    lcd_write(255);
    lcd_write(255);
    _delay_ms(500);
    setFrequency(22);
    _delay_ms(500);
  }
}


int main() {
  lcd_init();
  lcd_on();

  setLabels();
  setVolume(420);
  setRatio(2);
  setFrequency(22);
  setPeak(33);
  setPlateau(15);
  blinkingVolume();
  blinkingRatio();
  blinkingFreq();

  for (;;) {
  }


  lcd_off();
  return 0;
}
*/
