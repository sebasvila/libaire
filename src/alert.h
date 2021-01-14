#ifndef _ALERT_H_
#define _ALERT_H_

#include <stdint.h>

/**
 * @brief This module abstracts an alarm system. 
 *
 * An alarm is an (obscure) message sent to the end user signaling
 * some significative fatal event that requires his consideration.
 *
 * Alarms are fatal alarms: something unrecoverable happened. Mostly
 *     internal errors.
 */


/** fatal alert states */
#define ALRT_INCOMPATIBLE_ADC_REF 1


/** 
 * @brief  Sets a fatal alarm state.
 *
 * When called, the function activates a never ending pattern on the 
 * alarm led of the platform. Application becomes locked forever.
 * The pattern describes the value `e` and follows this 
 * sequence: two long dashes (sync) and `e` short dots.
 *
 * @param e: Alarm number
 */
void alert_fatal(uint8_t e);



#endif
