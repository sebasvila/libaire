#include <avr/io.h>
#include <util/delay.h>
#include "motor.h"


/*
 * Configure port and pin corresponding to motor driver.
 * enable, step, and direction
 */
#define ENA_PRT  PORTB
#define ENA_PIN  0
#define STP_PRT  PORTD
#define STP_PIN  2
#define DIR_PRT  PORTD
#define DIR_PIN  5


/* Macros to get control registers of port */
#define PORT(x) (x)
#define DDR(x)  (*(&(x)-1))   // consider DDRy = PORTy - 1
#define PIN(x)  (*(&(x)-2))   // consider PINy = PORTy - 2


/* 
 * Setup motor module.
 * Post: motor is prepared; motor disabled; direction wind  
 */
void motor_setup(void) {
  /* config ports as output */
  DDR(ENA_PRT) |= _BV(ENA_PIN);
  DDR(STP_PRT) |= _BV(STP_PIN);
  DDR(DIR_PRT) |= _BV(DIR_PIN);
  /* set ports default value */
  PORT(ENA_PRT) &= ~_BV(ENA_PIN);    // motor disabled
  PORT(STP_PRT) &= ~_BV(STP_PIN);    // step pin low
  if (motor_wind == 0)               // provision for configuring dir
    PORT(DIR_PRT) &= ~_BV(DIR_PIN);
  else
    PORT(DIR_PRT) |=  _BV(DIR_PIN);
}

/* Order one step. Motor needs to be enabled */
void motor_step(void) {
  /* pulse on step pin */
  PORT(STP_PRT) |= _BV(STP_PIN);
  _delay_us(2);
  PORT(STP_PRT) &= ~_BV(STP_PIN);
}

  
/* Set a given turning direction */
void motor_set_dir(motor_dir_t d) {
  if (d == 0)               // provision for configuring dir
    PORT(DIR_PRT) &= ~_BV(DIR_PIN);
  else
    PORT(DIR_PRT) |=  _BV(DIR_PIN);
}


/* Reverse the turning direction */
void motor_reverse(void) {
  PIN(DIR_PRT) |= _BV(DIR_PIN);
}


/*
 * Enable and disable motor.  
 * Motor is power feeded only while enabled, thus braking torque 
 * only effective while enabled. Disabling avoids power consumption.
 */
void motor_enable(void) {
  PORT(ENA_PRT) |= _BV(ENA_PIN);
}


void motor_disable(void) {
  PORT(ENA_PRT) &= ~_BV(ENA_PIN);
}  



