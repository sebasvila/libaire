#ifndef _I2C_REQUEST_
#define _I2C_REQUEST_

#include <stdint.h>

/*
 * Defines an i2c request and its auxiliary data types
 */


/* The i2c request type */
typedef enum {
  I2Csend, 
  I2Creceive, 
  I2Csend_uint8       //!< Defines a single-byte send request type
} i2cr_type_t;


/* An i2c node address */
typedef uint8_t i2cr_addr_t;


/* An i2c request status */
typedef enum {
  Running=0,
  Success,
  ReceivedMessageLenError,
  SlaveRejected,
  SlaveDiscardedData,
  InternalError,
} i2cr_status_t;


/* 
 * An i2c request object. The form depends on the request type.
 */
typedef struct {
  i2cr_type_t rt;
  i2cr_addr_t node;
  volatile i2cr_status_t *status;
  union {
    struct {uint8_t *buffer; uint8_t length;} ue; // buffer in user space
    uint8_t local_byte;                           // locally stored byte
  } data;
} i2cr_request_t;


#endif
