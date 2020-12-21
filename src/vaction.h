#ifndef _VACTION_H_
#define _VACTION_H_

#include "pt.h"

/* Set breathes per minute. `rr` in tenths of breathes */
void vaction_set_rr(uint8_t rr);
/* Set inspiration ramp time. `rr` in tenths of second */
void vaction_set_ir(uint8_t ir);
/* Set I:E ratio. Ratio is 1:`e`, `e` in tenths */
void vaction_set_ie(uint8_t e);
/* Set travel. Travel in percent of max_travel */
void vaction_set_tr(uint8_t tr);

/* ventilation action thread */
PT_THREAD(vaction_thread(struct pt *pt));

/* module setup */
void vaction_setup(void);

#endif
