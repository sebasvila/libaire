#ifndef CONFIG_H_
#define CONFIG_H_


/*
 * This module contains the hardware related configuration.
 * It depends on the actual platform ans shield (understand shield as
 * an abstraction of the 'offboard devices'. Mainly defines:
 *
 * (1) Functional units of the AVR used by the modules
 * (2) External devices available and its mapping to AVR pins
 */


/***********************************************************
 * Arduino ONE 
 ***********************************************************/
#if defined(ArduinoONE)

#define FATAL_ALERT_PORT PORTB
#define FATAL_ALERT_PIN  5


#if defined(ShieldITIC)

#define POT_CHANNEL     1
#define POT_REFERENCE   Vcc

#endif






/***********************************************************
 * Arduino MEGA
 ***********************************************************/
#elif defined(ArduinoMEGA)



#endif



#endif
