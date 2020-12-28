#ifndef _TESTFIXTURE_H_
#define _TESTFIXTURE_H_

/**
 *  @bief Test environment. 
 *  
 *  Requires the shield ITIC and Arduino ONE.
 */
#include <stdbool.h>

typedef enum {red, yellow, green} led_color;
typedef enum {semaph1, semaph2} led_semaph;

void fixture_setup(void);
void led_on(led_semaph s, led_color c);
void led_off(led_semaph s, led_color c);
void led_toggle(led_semaph s, led_color c);
bool led_is_on(led_semaph s, led_color c);

#endif
