#ifndef _MOTOR_H_
#define _MOTOR_H_


/* Define number of steps per revolution as configured by hardware */
#define MOTOR_STEPS_REV 200


/*
 * Defines de direction of the motor. If phisical motor
 * turns in the oposite sense, set 'motor_unwind=0' instead
 * of 'motor_wind'.
 */
typedef enum {motor_wind=0, motor_unwind} motor_dir_t;


/* 
 * Setup motor module.
 * Post: motor is prepared; motor disabled; direction wind  
 */
void motor_setup(void);

/* Order one step. Motor needs to be disabled */
void motor_step(void);

/* Set a given turning direction */
void motor_set_dir(motor_dir_t d);

/* Reverse the turning direction */
void motor_reverse(void);

/*
 * Enable and disable motor.  
 * Motor is power feeded only while enabled, thus braking torque 
 * only effective while enabled. Disabling avoids power consumption.
 */
void motor_enable(void);
void motor_disable(void);

#endif
