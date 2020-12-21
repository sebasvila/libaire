#ifndef LCD_H
#define LCD_H

#pragma once   //********* Què fa?

#include <avr/io.h>
#include <stdbool.h>

/* A 'lcd_t' is an abstraction of an LCD display using the HD44780
 * controller.  It is currently implemented to use a 4 bit data
 * connection.
 */

#define PORT(x) (*(x))
#define DDR(x)  (*(x-1))   // consider DDRy = PORTy - 1
#define PIN(x)  (*(x-2))   // consider PINy = PORTy - 2

#define LCD_COL_COUNT 20
#define LCD_ROW_COUNT 4   //Not used

// The rest should be left alone
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT    0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE    0x08
#define LCD_1LINE    0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00

#define LCD_COMMAND false
#define LCD_DATA    true

typedef struct {
  volatile uint8_t *port;
  uint8_t rs; // sending data =1; sending intruction = 0
  uint8_t rw; // Read / Write pin
  uint8_t en; // Enable pin
  uint8_t D0; // Data bits
  uint8_t D1;
  uint8_t D2;
  uint8_t D3;
} lcd_t;


/* Create and bind the lcd */
lcd_t lcd_bind(
  volatile uint8_t *port,
  uint8_t rs_pin,
  uint8_t en_pin);

void lcd_send(lcd_t l, bool rs_mode, uint8_t value);


void lcd_on(lcd_t l);
void lcd_off(lcd_t l);

void lcd_clear(lcd_t l);
void lcd_return_home(lcd_t l);

void lcd_enable_blinking(lcd_t l);
void lcd_disable_blinking(lcd_t l);

void lcd_enable_cursor(lcd_t l);
void lcd_disable_cursor(lcd_t l);

void lcd_scroll_left(lcd_t l);
void lcd_scroll_right(lcd_t l);

void lcd_set_left_to_right(lcd_t l);
void lcd_set_right_to_left(lcd_t l);

void lcd_enable_autoscroll(lcd_t l);
void lcd_disable_autoscroll(lcd_t l);

void lcd_create_char(lcd_t l, uint8_t location, uint8_t charmap[8]);

void lcd_move_cursor(lcd_t l, uint8_t col, uint8_t row);

void lcd_print(lcd_t l, char *string);
void lcd_printf(lcd_t l, char *format, ...);

#endif
