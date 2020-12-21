#ifndef _TICKER_H_
#define _TICKER_H_

/*
 * A ticker is an abstract machine that ticks at a precise
 * frequence. Every tick increases an integer counter that can be
 * publicly queried. Counter overflows freely and begins again with no
 * consequence.  Its designed to be used in medium and long latence
 * delays or chronometers.
 */

#include <stdint.h>

/* Setup but not start ticker */
void ticker_setup(void);

/* Start ticker counting */
void ticker_start(void);

/* Get current ticker value.
 * Only called if ticker started
 */
uint16_t ticker_get(void);

/* Get ticker beat in ticks per second */
inline uint16_t ticker_tps(void) {
  return 16000000/1024/156;
}

/* Stop ticker counting. */
void ticker_stop(void);


#endif
