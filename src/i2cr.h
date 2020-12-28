#ifndef _I2C_REQUEST_
#define _I2C_REQUEST_

#include <stdint.h>

/*
 * Defines an i2c request and its auxiliary data types
 */

/* An i2c request type */
typedef enum {I2Csend, I2Creceive} i2cr_type_t;

/* An i2c node address */
typedef uint8_t i2cr_addr_t;

/* An i2c request status */
typedef enum {
	      Running=0,
	      Success,
	      NodeCanceledSend,
	      ReceivedMessageOverflow,
} i2cr_status_t;

/* an i2c request object */
typedef struct {
  i2cr_type_t rt;
  i2cr_addr_t node;
  uint8_t *buffer;
  uint8_t length;
  volatile i2cr_status_t *status;
} i2cr_request_t;


#endif
