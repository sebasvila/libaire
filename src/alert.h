#ifndef _ALERT_H_
#define _ALERT_H_

#include <stdint.h>

/*
 * This module abstracts the alarm system. 
 * An alarm is a message sent to the end user signaling some
 * significative event that requires his consideration.
 *
 * Alarms are:
 * (1) Fatal alarms: something unrecoverable happened. Mostly 
 *     internal errors.
 * (2) (non fatal) alarms: something to be advertised to end user.
 */



/* fatal alert states */
#define ALRT_INCOMPATIBLE_ADC_REF 1



/* sets a fatal alarm state.
 * Whan called, the function activates a never ending pattern on the 
 * alarm led of the platform. Application becomes locked forever.
 * The pattern describes the value `e` and follows this 
 * sequence: two long dashes (sync) and `e` short dots.
 */
void alert_fatal(uint8_t e);





#endif
