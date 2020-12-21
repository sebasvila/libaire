#include <stdbool.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "pt.h"
#include "timer.h"
#include "motor.h"
#include "vaction.h"


/*****************************************************************
 * System constants
 *****************************************************************/

/* timer frequency (Hz) */
const float  freq  = 15625;
/* motor steps by revolution */
const int16_t stepsrev = 200;
/* turn angle for one step (rad) */ 
const float alpha = 2.0 * M_PI / 200;
/* ramp up(down) fraction of total travel */
const float ramp_frac = 0.2;
/* max motor turns */
const float max_turns = 4.0;
/* expiratory lowering time (s) */
const float exp_lowering_time = 0.7;


/*****************************************************************
 * Ventilation action dynamic parameters (change next time used)
 *****************************************************************/

static struct vaction_params {
  /* primary parameters */
  uint8_t rr;                      // breathes per minute (tenths)
  uint8_t ir;                      // inspiration rise (tenths second)
  uint8_t ie;                      // I:E ratio (1:`e` in tenths)
  uint8_t tr;                      // percent total travel (percent)
  /* computed parameters */
  int16_t travel;                  // in steps
  int16_t theta_rampup;            // in steps
  int16_t theta_rampdown;          // in steps
  int16_t ins_pause;               // in ticks
  int16_t exp_pause;               // in ticks
  int16_t exp_low_c;               // in ticks
  float accel;                     // in rad/s^2
} param = {
	   /* default primary parameters */
	   120, // 12 breathes per minute
	   9  , // inpiration rise 0.7 seconds
	   20 , // I:E ratio at 1:2
	   100  // 100% of total travel
};


/* Update computed parameters given system constants
 * and primary parameters. Must be called at init time 
 * and after any primary parameter change
 */
static void update_params(void) {
  /* total travel in steps */
  const int16_t travel = (param.tr/100.0*max_turns)*stepsrev;
  param.travel         = travel;
  /* step where speedup ends */
  param.theta_rampup   = travel*ramp_frac;
  /* step where speeddown begins */
  param.theta_rampdown = travel - param.theta_rampup;
  /* compute accel */
  const float S   = 2 * M_PI / stepsrev * travel;
  const float Sr  = 2 * M_PI / stepsrev * param.theta_rampup;
  const float T   = param.ir/10.0;
  param.accel = (S*S + 4*S*Sr + 4*Sr*Sr) / (2*Sr*T*T);
  /* expiration time between steps */
  param.exp_low_c = exp_lowering_time / travel * freq;
  /* rithm parameters (all in seconds) */
  const float period = 600.0 / param.rr;     // seconds
  const float k      = param.ie/10.0;
  const float k_one  = k + 1;
  const float ins_p = (period - k_one*param.ir/10.0)/k_one;     
  const float exp_p = (k*period - k_one*exp_lowering_time)/k_one;
  /* rithm parameters to steps */
  param.ins_pause = ins_p * freq;
  param.exp_pause = exp_p * freq;
}


/* Set breathes per minute. `rr` in tenths of breathes */
void vaction_set_rr(uint8_t rr) {
  param.rr = rr;
  update_params();
}


/* Set inspiration ramp time. `rr` in tenths of second */
void vaction_set_ir(uint8_t ir) {
  param.ir = ir;
  update_params();
}


/* Set I:E ratio. Ratio is 1:`e`, `e` in tenths */
void vaction_set_ie(uint8_t e) {
  param.ie = e;
  update_params();
}

/* Set travel. Travel in percent of max_travel */
void vaction_set_tr(uint8_t tr) {
  param.tr = tr;
  update_params();
}



/*****************************************************************
 * Ventilation action timer callbacks
 *****************************************************************/
static void do_nothing(void) {}


    

/*****************************************************************
 * Breathing action thread
 *****************************************************************/

PT_THREAD(vaction_thread(struct pt *pt))
{
  static uint16_t c, i;

  PT_BEGIN(pt);

  /* 
   * breathe forever!!
   */
  for(;;) {
    /* inspiration rising. assume motor wind direction */
    c = (param.ir/10.0) / (param.travel-1) * freq;
    timer_set_action(motor_step);
    motor_enable();
    i = param.travel;
    while (i>0) {
      timer_arm_once(c); 
      PT_WAIT_WHILE(pt, timer_armed());
      i--;
    }
    
    /* inspiration pause */
    timer_set_action(do_nothing);
    timer_arm_once(param.ins_pause); 
    PT_WAIT_WHILE(pt, timer_armed());
    
    /* expiration lowering */
    c = param.exp_low_c;
    motor_reverse(); // set unwind dir
    timer_set_action(motor_step);
    i = param.travel;
    while (i>0) {
      timer_arm_once(c); 
      PT_WAIT_WHILE(pt, timer_armed());
      i--;
    }

    /* Assure home position achieved */
    
    /* expiration pause */
    timer_set_action(do_nothing);
    timer_arm_once(param.exp_pause);
    motor_reverse(); // set wind dir again
    motor_disable(); // action in rest position: break torque not needed
    PT_WAIT_WHILE(pt, timer_armed());
  }
  
  PT_END(pt);
}




void vaction_setup(void) {
  /* init modules */
  timer_setup(t15625);
  motor_setup();
  
  /* setup computed parameters for first time */
  update_params();
}
