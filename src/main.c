#include <avr/interrupt.h>
#include "pt.h"
#include "vaction.h"



int main(void) {
  /* context dels threads */
  struct pt vaction_ctxt;

  /* init modules */
  vaction_setup();
  sei();


  /* init thread contexts */
  PT_INIT(&vaction_ctxt);
  
  /* do schedule */
  for(;;) {
    (void)PT_SCHEDULE(vaction_thread(&vaction_ctxt));
  }
}
