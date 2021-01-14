#include <avr/io.h>
#include <stdint.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#include "i2cr.h"
#include "i2cq.h"
#include "i2c.h"


#define TW_GO_OPERATIVE 0xff  // special automata event
#define TWI_FREQ 100000UL     // i2c bus frequency




/************************
 * Hw utility functions *
 ************************/

/**
 * @brief Disable I2C interrupts.
 */
static void disable_i2c_interrupts(void) {
  TWCR &= ~(1<<TWIE);   //Clears the TWI interrupt enable bit
}


/**
 * @brief Send a start condition to the I2C bus as a Master.
 * Enable interrupts.
 */
static void throw_start(void) {
  TWCR = (1<<TWINT) |  // Clears previous interrupt flag
         (1<<TWSTA) |  // Generates START condition.
         (1<<TWEN)  |  // Enable TWI. 
         (1<<TWIE);    // Enable I2C interrupts
}


/**
 * @brief Send a stop condition to the I2C bus.
 */
static void throw_stop(void) {
  TWCR =  (1<<TWINT) |  // Clears previous interrupt flag
          (1<<TWSTO) |  // Generate STOP condition.
          (1<<TWEN);    // Enable TWI.
}


/**
 * @brief Send a byte to the I2C bus. It can be either an SLA or data.
 * 
 * @param b The byte to be sent.
 */
static void throw_byte(uint8_t b) {
  TWDR = b;             // Load byte to TWDR data register
  TWCR = (1<<TWINT) |   // Clears previous interrupt flag
         (1<<TWEN)  |   // Enable TWI
         (1<<TWIE);     // Enable I2C interrupts
}


/**
 * @brief Send a byte request to the I2C bus.
 * Used in master receive mode to ACK or NACK last byte arrived thus
 * indirectly asking slave for a new data byte or not.
 * 
 * @param is_last: true iff master do not wants to receive more bytes
 *
 */
static void throw_request_byte(bool is_last){
  if (is_last){
    // send NACK
    TWCR =
      (1<<TWINT) |  // Clears last interrupt flag
      (1<<TWEN)  |  // Enable TWI. Hardware takes control over the I/O pins connected to the SCL and SDA pins
      (1<<TWIE);    // Enable I2C interrupts
  } else {
    // send ACK
    TWCR =
      (1<<TWINT) |  // Clears last interrupt flag
      (1<<TWEA)  |  // Send ACK
      (1<<TWEN)  |  // Enable TWI
      (1<<TWIE);    // Enable I2C interrupts
  }
}


/**
 * @brief Get the recieved byte from I2C hw
 *
 * The TWDR port is both used to send and receive data.
 */
static uint8_t get_byte(void) {
  return TWDR;
}



/********************************************************
 * Interrupt driven master automata
 ********************************************************/


/* Automata current state */
static volatile enum {
  Idle, Starting, SeekingSlaveTx, TxData, SeekingSlaveRx, RxData
} ida_state;

/* Requests queue */
static i2cq_t requests;

/* request being processed by right now */
static const i2cr_request_t *current_req;



/**
 * @brief Set `s` status to current request (finished) and
 *  - sends ReSTART and fetch new request from queue, or
 *  - sends STOP and goes to Idle state
 * 
 * @param s Status to be written to the already-finished current request.
 */
static void fetch_or_idle(i2cr_status_t s) {
  // return status if it should be returned
  if (current_req->status) *(current_req->status) = s;
  i2cq_dequeue(&requests);
  if (i2cq_is_empty(&requests)) {
    throw_stop();
    disable_i2c_interrupts();
    ida_state = Idle;
  } else {
    // fetch next request and begin cycle again
    throw_start();
    current_req = i2cq_front(&requests);
    ida_state = Starting;
  }
}
  

/**
 * @brief Do an automata transition after event `e`
 * and current state `ida_state`.
 * 
 * @param e A byte containing the I2C current hardware status.
 */
static void ida_next(uint8_t e) {
  static uint8_t i;   //!< Index of next byte to be Rx/Tx

  switch (ida_state) {
  case Idle:
    /* In idle state. Interrupts disabled */
    if (e == TW_GO_OPERATIVE) {
      // get next request, must exist for sure
      current_req = i2cq_front(&requests);
      // Throw a START to get the bus control
      throw_start();
      // next state
      ida_state = Starting;
    } else {
      // unexpected event
      fetch_or_idle(InternalError);
    }
    break;

  case Starting:
    /* START sent, waiting it to finish */
    if (e == TW_START || e == TW_REP_START) {
      // The bus is available, begin messaging a node
      i = 0;
      if (current_req->rt == I2Creceive) {        
        throw_byte(current_req->node << 1 | TW_READ);
        ida_state = SeekingSlaveRx;
      } else {
        // I2Csend and I2Csend_uint8
        throw_byte((current_req->node << 1) | TW_WRITE);
        ida_state = SeekingSlaveTx;
      }
    } else {
      // unexpected event
      fetch_or_idle(InternalError);
    }
    break;

  case SeekingSlaveTx:
    if (e == TW_MT_SLA_ACK) {
      // Slave contacted, let's talk with him
      if (current_req->rt == I2Csend){
        throw_byte(current_req->data.ue.buffer[i++]);
      } else if (current_req->rt == I2Csend_uint8){
        throw_byte(current_req->data.local_byte);
        i++;
      }
      ida_state = TxData;
    } else if (e == TW_MT_SLA_NACK) {
      // Slave contacted and not available
      fetch_or_idle(SlaveRejected);
    } else {
      // Unexpected event
      fetch_or_idle(InternalError);
    }
    break;

  case TxData:
    if (e == TW_MT_DATA_ACK) {
      // Data sent ok
      if (current_req->rt != I2Csend_uint8 &&
	  i < current_req->data.ue.length) {
        // Send another byte and remain in the same state
        throw_byte(current_req->data.ue.buffer[i++]);
      } else {
        // No more data to send
        fetch_or_idle(Success);
      }
    } else if (e == TW_MT_DATA_NACK) {      
      // Data rejected by slave
      fetch_or_idle(SlaveDiscardedData);
    } else {
      // Unexpected event
      fetch_or_idle(InternalError);
    }
    break;
    
  case SeekingSlaveRx:
    if (e == TW_MR_SLA_ACK) {
      // Slave contacted, let's talk with him
      // First byte is received. Maybe the last one
      // current_req->data.ue.buffer[i++] = get_byte();   ??????????????????
      throw_request_byte(false);
      ida_state = RxData;
    } else if (e == TW_MR_SLA_NACK) {
      // Slave contacted and not available
      fetch_or_idle(SlaveRejected);
    } else {
      // Unexpected event
      fetch_or_idle(InternalError);
    }
    break;

  case RxData:
    if (e == TW_MR_DATA_ACK) {
      // Get another byte and remain in the same state
      current_req->data.ue.buffer[i++] = get_byte();
      //If this was the last byte, next event will be TW_MR_DATA_NACK
      throw_request_byte(current_req->data.ue.length == i);
    } else if (e == TW_MR_DATA_NACK) {
      // Check if all expected data was received
      if (current_req->data.ue.length == i) {
        // No more data to receive
        fetch_or_idle(Success);
      } else {
        // Received a block shorter than expected
        fetch_or_idle(ReceivedMessageLenError);
      }
    } else {
      // Unexpected event
      fetch_or_idle(InternalError);
    }
    break;
  }
}

/* interrupt service */
ISR(TWI_vect) {
  ida_next(TW_STATUS);
}



/*************************************************************
 * Generic management operations
 *************************************************************/

void i2c_setup(void) {
  // Initialize I2C prescaler and bit rate
  TWSR &= ~(1<<TWPS0) & ~(1<<TWPS1);
  
  /* 
   * I2C bit rate formula from atmega128 manual pg 204
   * SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
   * note: TWBR should be 10 or higher for master mode.
   * It is 72 for a 16MHz clocked board with 100kHz TWI 
   */
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
}


void i2c_open(void) {
  i2cq_empty(&requests);
  ida_state = Idle;
  TWCR = _BV(TWEN);  // Enable I2C module
}


void i2c_close(void) {
  while(ida_state != Idle);           // Wait till queue empty
  TWCR = 0;                           // Disable I2C module
}


bool i2c_swamped(void) {
  return i2cq_is_full(&requests);
}



/*************************************************************
 * Block transmision operations
 *************************************************************/

void i2c_send(i2cr_addr_t node,
	      uint8_t *const  buffer,
	      uint8_t length,
	      volatile i2cr_status_t *const  status) {
  i2cr_request_t r = {
    .rt = I2Csend,
    .node = node,
    .status = status,
    .data.ue = { .buffer = buffer, .length = length},
  };
  
  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, &r);
  
  // initialize the status to Running if needed
  if (status) *status = Running;

  if (ida_state == Idle){
    //Start the automata
    ida_next(TW_GO_OPERATIVE);
  }
}



void i2c_receive(i2cr_addr_t node,
		 uint8_t *const buffer,
		 uint8_t length,
		 volatile i2cr_status_t *const  status) {
  i2cr_request_t r = {
    .rt = I2Creceive,
    .node = node,
    .status = status,
    .data.ue = {.buffer = buffer, .length = length},
  };

  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, &r);

  // initialize the status to Running if needed
  if (status) *status = Running;

  if (ida_state == Idle){
    //Start the automata
    ida_next(TW_GO_OPERATIVE);
  }
}


/*************************************************************
 * Byte transmision operations
 *************************************************************/

void i2c_send_uint8(i2cr_addr_t node,
		    uint8_t b,
		    volatile i2cr_status_t *const status) {
  i2cr_request_t r = {
    .rt = I2Csend_uint8,
    .node = node,
    .status = status,
    .data.local_byte = b
  };
  
  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, &r);

  // initialize the status to Running if needed
  if (status) *status = Running;
  
  if (ida_state == Idle){
    //Start the automata
    ida_next(TW_GO_OPERATIVE);
  }
}




void i2c_receive_uint8(i2cr_addr_t node,
		       uint8_t *const b,
		       volatile i2cr_status_t *const status) {
  i2cr_request_t r = {
    .rt = I2Creceive,
    .node = node,
    .status = status,
    .data.ue = {.buffer = b, .length = 1},
  };

  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, &r);

  // initialize the status to Running if needed
  if (status) *status = Running;

  if (ida_state == Idle){
    //Start the automata
    ida_next(TW_GO_OPERATIVE);
  }

};


/*************************************************************
 * Combined transmision operations
 *************************************************************/

void i2c_sandr(i2cr_addr_t node,
	        uint8_t *const  s_buffer,
	        uint8_t s_length,
	        uint8_t *const  r_buffer,
	        uint8_t r_length,
	        volatile i2cr_status_t *const status) {
}



