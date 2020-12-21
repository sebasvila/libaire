//Source: https://github.com/aostanin/avr-hd44780

#include <avr/io.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd.h"


static uint8_t lcd_displayparams;


/*
 * Send a nibble-sized message to the LCD.
 * @param l The LCD object where the message must be adressed
 * @param nibble The 4-bit message to be sent
 */
static void lcd_write_nibble(lcd_t l,uint8_t nibble) {
  PORT(l.port) = (PORT(l.port) & 0xf0) | (nibble & 0x0f);

  //Enable pulse
  PORT(l.port) &= ~l.en;
  PORT(l.port) |= l.en;
  PORT(l.port) &= ~l.en;

  _delay_us(50); //Es pot provar de baixar
}


/*
 * Sends a DATA or COMMAND byte instruction to the LCD.  Internally,
 * as the display is connected using 4-bit data bus, the function
 * splits the byte into two nibble-sized messages.
 *
 * @param l The LCD object where the message must be adressed
 * @param rs_mode Message is a LCD_COMMAND LCD_DATA
 * @param message The byte to be sent
 */
void lcd_send(lcd_t l, bool rs_mode, uint8_t message) {
  if (rs_mode) {
    PORT(l.port) |= l.rs;
  } else {
    PORT(l.port) &= ~l.rs;
  }
  lcd_write_nibble(l,message >> 4);
  lcd_write_nibble(l,message);
}


/*
 * Create and bind the lcd object.
 * D0-D3 data pins must be connected to the port's low nibble
 * The function takes about 25ms (blocking)
 *
 * @param port The physical AVR port where the pins are connected
 * @param rs_pin The bit number where RS pin is connected.
 * @param en_pin The bit number where Enable pin is connected.
 * @return The lcd object
 */
lcd_t lcd_bind(volatile uint8_t *port, uint8_t rs_pin, uint8_t en_pin){
    // Construct `lcd_t` object.
    lcd_t l;

    l.port = port;

    // `l`. `l.<>_pin` stores the mask of each pin
    l.rs = _BV(rs_pin);
    l.en = _BV(en_pin);

    /* Configure port+pin direction */
    DDR(port) |=  l.rs | l.en | 0x0f;

    // Wait for LCD to become ready (docs say 15ms+)
    _delay_ms(15);

    //Set ENABLE and RS pins to LOW state
    PORT(port) &= ~l.en & ~l.rs;

    lcd_write_nibble(l, 0x03); // 1st time
    _delay_ms(4.2);
    lcd_write_nibble(l, 0x03); // 2nd time
    _delay_ms(4.2)   ;         // Docs says more than 100us
    lcd_write_nibble(l, 0x03); // 3rd time
    _delay_us(100);

    // Set 4-bit mode
    lcd_write_nibble(l, 0x02);

    // Configure params
    lcd_send(l, LCD_COMMAND, LCD_FUNCTIONSET |
	        LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);

    // Configure more params
    lcd_displayparams = LCD_CURSOROFF | LCD_BLINKOFF;
    lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | lcd_displayparams);

    return l;
  }


/*
 * Turns the LCD ON
 * @param l The LCD object to turn ON
 */
void lcd_on(lcd_t l) {
  lcd_displayparams |= LCD_DISPLAYON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | lcd_displayparams);
}


/*
 * Clears the LCD display. The function takes 2ms (blocking)
 * @param l The LCD object that must be cleared
 */
void lcd_clear(lcd_t l) {
  lcd_send(l, LCD_COMMAND, LCD_CLEARDISPLAY);
  _delay_ms(2);   //Atention!
}


/*
 * Turns the LCD OFF
 * @param l The LCD object
 */
void lcd_off(lcd_t l) {
  lcd_displayparams &= ~LCD_DISPLAYON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | lcd_displayparams);
}


/*
 * Returns both display and cursor to the original 
 * position (address 0). The function takes 2ms (blocking)
 * @param l The LCD object
 */
void lcd_return_home(lcd_t l) {
  lcd_send(l, LCD_COMMAND, LCD_RETURNHOME);
  _delay_ms(2); //Atention
}


/*
 * Enables the cursor blinking
 * @param l The LCD object
 */
void lcd_enable_blinking(lcd_t l) {
  lcd_displayparams |= LCD_BLINKON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | lcd_displayparams);
}


/*
 * Disables the cursor blinking
 * @param l The LCD object
 */
void lcd_disable_blinking(lcd_t l) {
  lcd_displayparams &= ~LCD_BLINKON;
  lcd_send(l,LCD_COMMAND, LCD_DISPLAYCONTROL | lcd_displayparams);
}


/*
 * Enables the cursor visibility
 * @param l The LCD object
 */
void lcd_enable_cursor(lcd_t l) {
  lcd_displayparams |= LCD_CURSORON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | lcd_displayparams);
}


/*
 * Disables the cursor visibility
 * @param l The LCD object
 */
void lcd_disable_cursor(lcd_t l) {
  lcd_displayparams &= ~LCD_CURSORON;
  lcd_send(l, LCD_COMMAND, LCD_DISPLAYCONTROL | lcd_displayparams);
}


/*
 * Sets the scroll direction to LEFT
 * @param l The LCD object
 */
void lcd_scroll_left(lcd_t l) {
  lcd_send(l, LCD_COMMAND, LCD_CURSORSHIFT | LCD_DISPLAYMOVE |
	      LCD_MOVELEFT);
}


/*
 * Sets the scroll direction to RIGHT
 * @param l The LCD object
 */
void lcd_scroll_right(lcd_t l) {
  lcd_send(l, LCD_COMMAND, LCD_CURSORSHIFT |
	      LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}


/*
 * Sets the entry mode to be left to right. 
 * Means the cursor will move RIGHT at each written character.
 * @param l The LCD object
 */
void lcd_set_left_to_right(lcd_t l) {
  lcd_displayparams |= LCD_ENTRYLEFT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | lcd_displayparams);
}


/*
 * Sets the entry mode to be right to left. Means the cursor 
 * will move LEFT at each written character.
 * @param l The LCD object
 */
void lcd_set_right_to_left(lcd_t l) {
  lcd_displayparams &= ~LCD_ENTRYLEFT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | lcd_displayparams);
}


/*
 * Enables the autoscroll mode.
 * @param l The LCD object
 */
void lcd_enable_autoscroll(lcd_t l) {
  lcd_displayparams |= LCD_ENTRYSHIFTINCREMENT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | lcd_displayparams);
}


/*
 * Disables the autoscroll mode.
 * @param l The LCD object
 */
void lcd_disable_autoscroll(lcd_t l) {
  lcd_displayparams &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_send(l, LCD_COMMAND, LCD_ENTRYMODESET | lcd_displayparams);
}


/*
 * Creates a new character. The LCD can save up to 8 
 * user-defined characters (5x8px). 
 * The lcd_move_cursor() must be called after the char 
 * creation, otherwise the cursor is pointing somewhere else.
 *
 * @param l The LCD object
 * @param location The address to save the new character.
 * @param charmap The charmap containing the new char information
 */
void lcd_create_char(lcd_t l, uint8_t location, uint8_t *charmap) {
  lcd_send(l, LCD_COMMAND, (LCD_SETCGRAMADDR | ((location & 0x07) << 3)));
  for (int i = 0; i < 8; i++) {
    lcd_send(l, LCD_DATA, *charmap);
    charmap++;
  }
}


/*
 * Move the LCD cursor to the desired position
 * @param l The LCD object
 * @param col The column
 * @param row The row
 */
void lcd_move_cursor(lcd_t l, uint8_t col, uint8_t row) {
  static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  lcd_send(l, LCD_COMMAND, LCD_SETDDRAMADDR | (col + offsets[row]));
}


/*
 * Sends a string to the LCD
 * @param l The LCD object to send the string
 * @param string The string object to be sent
 */
void lcd_print(lcd_t l, char *string) {
  for (char *it = string; *it; it++) {
    lcd_send(l, LCD_DATA, *it);
  }
}


/*
 * Prints a formated string on the LCD
 * @param l The LCD object
 * @param format The
 */
void lcd_printf(lcd_t l, char *format, ...) {
  va_list args;
  char lcd_buffer[LCD_COL_COUNT + 1];

  va_start(args, format);
  vsnprintf(lcd_buffer, LCD_COL_COUNT + 1, format, args);
  va_end(args);

  lcd_print(l, lcd_buffer);
}
