#ifndef _I2C_H_
#define _I2C_H_

/*
 * Low level driver for i2c bus master access
 */

#include <stdint.h>
#include <stdbool.h>
#include "i2cr.h"


/**
 * @brief I2C driver setup.
 */
void i2c_setup(void);

void i2c_open(void);
void i2c_close(void);



/**
 * @brief Checks if i2c driver can receive more requests.
 * 
 * @returns true if i2c driver cannot receive more requests.
 */
bool i2c_swamped(void);


/******************************************************************
 * Block send/receive
 ******************************************************************/


/**
 * @brief Request the driver to (asyncronously) send to `node`
 * the message of `length` bytes contained in `buffer`.
 * `status` has the state of the request:
 *  - Running: while request not scheduled
 *  - Success: when request ended satifactorily
 *  - Other values: when request ended with an error condition.
 * `status` will not be used again after request ended.
 * If the i2d driver cannot receive more requests, the call
 * blocks until this request can be accepted.
 * 
 * @param node The I2C byte address of the receiver.
 * @param buffer A pointer to a byte array where the message is saved.
 * @param length The number of bytes to be sent from the buffer.    
 * @param status A pointer to a `volatile i2cr_status_t` variable that 
 *               contains the status of the request.
 * @prec length > 0
 */
void i2c_send(i2cr_addr_t node,
	      uint8_t *const  buffer,
	      uint8_t lenght,
	      volatile i2cr_status_t *const  status);

/**
 * @brief Request the driver to (asyncronously) receive from `node`
 * a message of `length` bytes and to store it in `buffer`.
 * `status` has the state of the request:
 *  - Running: while request not scheduled
 *  - Success: when request ended satifactorily
 *  - Other values: when request ended with an error condition.
 * `status` will not be used again after request ended.
 * If the i2d driver cannot receive more requests, the call
 * blocks until this request can be accepted.
 * 
 * @param node   The I2C byte address of the sender.
 * @param buffer A pointer to a byte array where the message will be saved.
 * @param length The number of bytes to be received from the buffer.
 * @param status A pointer to a `volatile i2cr_status_t` variable 
 *               that contains the status of the request.
 * @prec length > 0
 */
void i2c_receive(i2cr_addr_t node,
		 uint8_t *const buffer,
		 uint8_t lenght,
		 volatile i2cr_status_t *const  status);



/******************************************************************
 * Single byte send/receive
 ******************************************************************/

/**
 * @brief Asyncronously sends a single byte to an i2c node.
 * A copy of the byte to be sent `b` is stored internally and thus 
 * no buffer is required.
 */
void i2c_send_uint8(i2cr_addr_t node,
		    uint8_t b,
		    volatile i2cr_status_t *const status);

/**
 * @brief Asyncronously receives a single byte from an i2c node.
 */
void i2c_receive_uint8(i2cr_addr_t node,
		       uint8_t *const b,
		       volatile i2cr_status_t *const status);



/******************************************************************
 * Send and then receive blocks
 ******************************************************************/

void i2c_sandr(i2cr_addr_t node,
	       uint8_t *const  s_buffer,
	       uint8_t s_lenght,
	       uint8_t *const  r_buffer,
	       uint8_t r_lenght,
	       volatile i2cr_status_t *const status);


#endif
