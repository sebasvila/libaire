#ifndef _I2C_REQUEST_
#define _I2C_REQUEST_

#include <stdint.h>
#include "i2c.h"

/*
 * Defines an i2c request and its auxiliary data types
 */


/* The i2c request type */
typedef enum {
  I2Csend, 
  I2Creceive, 
  I2Csend_uint8,  // Single-byte send request type
  I2Csend_2uint8, // Double-byte send request type
} i2cr_type_t;

/* 
 * An i2c request object. The form depends on the request type.
 */
typedef struct {
  i2cr_type_t rt;
  i2c_addr_t node;
  volatile i2c_status_t *status;
  union {
    /* buffer in user space */
    struct {uint8_t *buffer; uint8_t length;} ue;
    /* locally stored double byte */
    uint8_t local_2byte[2];
    /* locally stored single byte */
    uint8_t local_byte;                           
  } data;
} i2cr_request_t;


#endif
