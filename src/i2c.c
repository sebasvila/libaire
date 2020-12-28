#include "i2cr.h"
#include "i2cq.h"
#include "i2c.h"




static i2cq_t requests;


void i2c_setup(void) {
  i2cq_empty(&requests);
}


void i2c_open(void) {
}


void i2c_close(void) {
}


void i2c_send(i2cr_addr_t node,
	      const uint8_t *const  buffer,
	      uint8_t length,
	      volatile i2cr_status_t *const status) {
  const i2cr_request_t r =
    {
     .rt = I2Csend,
     .node = node,
     .send_buffer = buffer,
     .send_length = length,
     .status = status,
    };

  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, &r);
}



void i2c_receive(i2cr_addr_t node,
		 uint8_t *const  buffer,
		 uint8_t length,
		 volatile i2cr_status_t *const status) {
  i2cr_request_t r =
    {
     .rt = I2Creceive,
     .node = node,
     .receive_buffer = buffer,
     .receive_length = length,
     .status = status,
    };

  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, &r);
}


void i2c_sandr(i2cr_addr_t node,
	       const uint8_t *const  s_buffer,
	       uint8_t s_length,
	       uint8_t *const  r_buffer,
	       uint8_t r_length,
	       volatile i2cr_status_t *const status) {
  i2cr_request_t r =
    {
     .rt = I2Csendrec,
     .node = node,
     .send_buffer = s_buffer,
     .send_length = s_length,
     .receive_buffer = r_buffer,
     .receive_length = r_length,
     .status = status,
    };

  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, &r);
}



bool i2c_swamped(void) {
  return i2cq_is_full(&requests);
}



void i2c_send_b(i2cr_addr_t node,
		uint8_t b,
		volatile i2cr_status_t *const status) {};


void i2c_send_bb(i2cr_addr_t node,
		 uint16_t b,
		 volatile i2cr_status_t *const status) {};

void i2c_receive_b(i2cr_addr_t node,
		   uint8_t b,
		   volatile i2cr_status_t *const status) {};

void i2c_receive_bb(i2cr_addr_t node,
		    uint16_t b,
		    volatile i2cr_status_t *const status) {};




/********************************************************
 * Interrupt driven master automata
 ********************************************************/


#define TW_GO_OPERATIVE 0xff

/* Automata current state */
typedef enum {Idle, Starting, SeekingSlaveTx, TxData,
	      SeekingSlaveRx,
} ida_state;

/* request being processed by driver */
static i2cr_request_t current_req; // could it be a pointer?



/* Hw utility functions */
static void disable_i2c_interrupts(void) {}
static void enable_i2c_interrupts(void) {}
static void throw_start(void) {}
static void throw_stop(void) {}
static void throw_byte(uint8_t b) {}


/*
 * Set `s` status to current request and
 *  - sends ReSTART and fetch new request from queue, or
 *  - sends STOP and goes to Idle state
 */
static void fetch_or_iddle(i2cr_status_t s) {
  *current_req.status = s;
  if (i2cq_is_empty(&requests)) {
    throw_stop();
    disable_i2c_interrupts();
    ida_state = Idle;
  } else {
    // fetch next request and begin cycle again
    throw_start();
    i2cq_front(&requests, &current_req);
    i2cq_dequeue(&requests);
    ida_state = Starting;
  }
}
  

/*
 * Do an automata transition after event `e`
 * and current state `ida_current_state`
 */
static void ida_next(uint8_t e) {
  static uint8_t i;   // next byte to be Rx/Tx
  
  switch (ida_current_state) {
  case Idle:
    /* In idle state. Interrupts disabled */
    if (e == TW_GO_OPERATIVE) {
      // get next request, must exist for sure
      i2cq_front(&requests, &current_req);
      i2cq_dequeue(&requests);
      // Throw a START to get the bus control
      enable_i2c_interrupts();
      throw_start();
      // next state
      ida_state = Starting;
    } else {
      // unexpected event
    }
    break;

  case Starting;
    /* START sent, waiting it to finish */
    if (e == TW_START) {
      // The bus is available, begin messaging a node
      i = 0;
      if (current_req.rt == I2Creceive) {
	throw_byte(current_req.node | TW_READ);
	ida_state = SeekingSlaveRx;
      } else {
	// I2Csend
	throw_byte(current_req.node | TW_WRITE);
	ida_state = SeekingSlaveTx;
      }
    } else {
      // unexpected event
      error();
    }
    break;

  case SeekingSlaveTx:
    if (e == TW_MT_SLA_ACK) {
      // Slave contacted, let's talk with him
      throw_byte(current_req.buffer[i++]);
      ida_state = TxData;
    } else if (e == TW_MT_SLA_NACK) {
      // Slave contacted and not available
      fetch_or_idle(SlaveRejected);
    } else {
      // Unexpected event
    }
    break;

  case TxData:
    if (e == TX_MT_DATA_ACK) {
      // Data sent ok
      if (current_req.length != i) {
	// Send anothe byte and remain in the same state
	throw_byte(current_req.buffer[i++]);
      } else {
	// No more data to send
	fetch_or_idle(Success);
      }
    } else if (e == TX_MT_DATA_NACK) {
      // Data rejected by slave
      fetch_or_idle(SlaveDiscardedData);
    } else {
      // Unexpected event
    }
    break;
    
  case SeekingSlaveRx:
    if (e == TW_MR_SLA_ACK) {
      // Slave contacted, let's talk with him
      // Request a new byte. Maybe the last one
      throw_request_byte(current_req.lengtrh == 1);
      ida_state = RxData;
    } else if (e == TW_MR_SLA_NACK) {
      // Slave contacted and not available
      fetch_or_idle(SlaveRejected);
    } else {
      // Unexpected event
    }
    break;
  }

  case RxData:
    if (e == TW_MR_DATA_ACK) {
    } else if (e == TW_MR_DATA_NACK) {
    }
    break;
}


	     
