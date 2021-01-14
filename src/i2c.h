#ifndef _I2C_H_
#define _I2C_H_

/*
 * Low level driver for i2c bus master access
 */

#include <stdint.h>
#include <stdbool.h>
#include "i2cr.h"



/******************************************************************
 * Module management operations
 ******************************************************************/

/**
 * @brief I2C driver setup.
 * Must be called before any other operation of the module.
 */
void i2c_setup(void);

/**
 * @brief Opens the i2c channel.
 * You cannot transmit through a closed channel. An open channel cannot be 
 * reopened without first closing it.
 */
void i2c_open(void);

/**
 * @brief Closes the i2c channel.
 * It's mandatory to call it before exiting the application. A closed i2c channel
 * can be reopened later if needed.
 */
void i2c_close(void);

/**
 * @brief Checks if right now i2c driver can receive more requests.
 * 
 * @returns true iff i2c driver cannot receive more requests.
 */
bool i2c_swamped(void);



/******************************************************************
 * Block send/receive
 ******************************************************************/

/**
 * @brief Request the driver to (asyncronously) send to `node`
 * the message of `length` bytes contained in `*buffer`.
 * `*status` has the current state of the request:
 *  - Running: while request not scheduled
 *  - Success: when request ended satifactorily
 *  - Other values: when request ended with an error condition.
 *
 * `*status` and `*buffer` cannot be disposed until send request
 * execution finished. `*status` can be queried to know the request
 * processing state. If the i2d driver cannot receive more requests,
 * the call blocks until this request can be accepted.
 * 
 * @param node: The I2C byte address of the receiver.
 * @param buffer: A pointer to the byte array where the message is saved.
 * @param length: The number of bytes to be sent from the buffer.    
 * @param status: A pointer to a `volatile i2cr_status_t` variable that 
 *               contains the current status of the request. If NULL, then
 *               no status will be reported (not recommeded).
 * @pre   length > 0
 * @post *status == Running if status != NULL
 */
void i2c_send(i2cr_addr_t node,
	      uint8_t *const  buffer,
	      uint8_t lenght,
	      volatile i2cr_status_t *const status);

/**
 * @brief Request the driver to (asyncronously) receive from `node`
 * a message of `length` bytes and to store it in `*buffer`.
 * `*status` has the current state of the request:
 *  - Running: while request not scheduled
 *  - Success: when request ended satifactorily
 *  - Other values: when request ended with an error condition.
 *
 * `*status` and `*buffer` cannot be disposed until send request
 * execution finished. `*status` can be queried to know the request
 * processing state. If the i2d driver cannot receive more requests,
 * the call blocks until this request can be accepted.
 * 
 * @param node:   The I2C byte address of the sender.
 * @param buffer: A pointer to a byte array where the message will be saved.
 * @param length: The number of bytes to be received from the buffer.
 * @param status: A pointer to a `volatile i2cr_status_t` variable 
 *                that contains the status of the request. If NULL, them
 *                no status will be reported (not recommended).
 * @pre length > 0
 * @post *status == Running if status != NULL
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
 *
 * A single byte `b` is sent (asyncronously) to `node`.
 * `*status` contains the current state of the request:
 *  - Running: while request not scheduled
 *  - Success: when request ended satifactorily
 *  - Other values: when request ended with an error condition.
 *
 * `status` can be queried to know the request processing state. If
 * the i2d driver cannot receive more requests, the call blocks until
 * this request can be accepted.
 * 
 * @param node:   The I2C byte address of the sender.
 * @param b:      An uint8_t value to be sent.
 * @param status: A pointer to a `volatile i2cr_status_t` variable 
 *                that contains the current state of the request. If
 *                NULL, no status will be reported (not recommended).
 * @pre length > 0
 * @post *status == Running if status != NULL
 */
void i2c_send_uint8(i2cr_addr_t node,
		    uint8_t b,
		    volatile i2cr_status_t *const status);

/**
 * @brief Request the driver to (asyncronously) receive from `node`
 * a uint8_t message and to store it in `*b`.
 * `*status` has the current state of the request:
 *  - Running: while request not scheduled
 *  - Success: when request ended satifactorily
 *  - Other values: when request ended with an error condition.
 *
 * `*status` and `*b` cannot be disposed until send request
 * execution finished. `*status` can be queried to know the request
 * processing state. If the i2d driver cannot receive more requests,
 * the call blocks until this request can be accepted.
 * 
 * @param node:   The I2C byte address of the sender.
 * @param b:      A pointer to an uint8_t where the message will be saved.
 * @param status: A pointer to a `volatile i2cr_status_t` variable 
 *                that contains the status of the request. If NULL, them
 *                no status will be reported (not recommended).
 * @post *status == Running if status != NULL
 */
void i2c_receive_uint8(i2cr_addr_t node,
		       uint8_t *const b,
		       volatile i2cr_status_t *const status);



/******************************************************************
 * Send and then receive blocks
 ******************************************************************/

/**
 * @brief Sends a message and then reads
 *
 * Equivalent to first sending the message and then receiving the result.
 * `*status` corresponds to receive operation.
 *
 * @param node:     Slave node address
 * @param s_buffer: Pointer to send message buffer
 * @param s_length: Length of message to be sent
 * @param r_buffer: Pointer to receive buffer
 * @param r_length: Expected length of received message
 * @param status:   A pointer to a `volatile i2cr_status_t` variable 
 *                  that contains the status of the request. If NULL, them
 *                  no status will be reported (not recommended).
 * @pre s_length > 0 and r_length > 0 and 
         len(s_buffer) >= s_length and len(r_buffer >= r_length)
 * @post *status == Running if status != NULL
 */
void i2c_sandr(i2cr_addr_t node,
	       uint8_t *const  s_buffer,
	       uint8_t s_lenght,
	       uint8_t *const  r_buffer,
	       uint8_t r_lenght,
	       volatile i2cr_status_t *const status);


#endif
