#ifndef _IMGR_H_
#define _IMGR_H_

#include <stdbool.h>

typedef struct {
  /* setup done or not */
  unsigned int setup_done: 1;
  /* how many times open called */
  unsigned int times_open: 7;
} imgr_state_t;


/**
 * @brief cosmetic macros
 *
 * Allow to define an enviroment that is excuted according
 * to the setup/open/close protocol
 */
#define INIT_MGR(s) static imgr_state_t s = (imgr_state_t){0,0}
#define WITH_SETUP_MGR(s) if (imgr_eff_setup(& s )) 
#define WITH_OPEN_MGR(s)  if (imgr_eff_open(& s )) 
#define WITH_CLOSE_MGR(s) if (imgr_eff_close(& s )) 

/**
 * @brief returns true iff this setup call must be effective
 */
bool imgr_eff_setup(imgr_state_t *const s);

/**
 * @brief returns true iff this open call must be effective
 */
bool imgr_eff_open(imgr_state_t *const s);

/**
 * @brief returns true iff this open call must be effective
 */
bool imgr_eff_close(imgr_state_t *const s);

#endif
