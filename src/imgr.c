#include <stdbool.h>
#include "imgr.h"

/**
 * @brief returns true iff this setup call must be effective
 */
bool imgr_eff_setup(imgr_state_t *const s) {
  if (s->setup_done == 1)
    return false;
  else {
    s->setup_done = 1;
    return true;
  }
}

/**
 * @brief returns true iff this open call must be effective
 */
bool imgr_eff_open(imgr_state_t *const s) {
  return (s->times_open++ == 0);
}

/**
 * @brief returns true iff this open call must be effective
 */
bool imgr_eff_close(imgr_state_t *const s) {
  return (--s->times_open == 0);
}
