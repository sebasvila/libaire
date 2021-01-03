#include <avr/io.h>
#include <stdint.h>
#include <twi.h>
#include <avr/interrupt.h>
#include "i2cr.h"
#include "i2cq.h"
#include "i2c.h"


#define TW_GO_OPERATIVE 0xff

/* Automata current state */
enum {Idle, Starting, SeekingSlaveTx, TxData,
	      SeekingSlaveRx, RxData
} ida_state;

static i2cq_t requests;

void i2c_setup(void) {
  i2cq_empty(&requests);
  ida_state = Idle;

  //Internal pullups SDA & SCL
  PORTC |=  (1<<4) |
            (1<<5);

  //Initialize I2C prescaler and bit rate
  TWSR &= ~(1<<TWPS0) &
          ~(1<<TWPS1);
  
  /* I2C bit rate formula from atmega128 manual pg 204
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  note: TWBR should be 10 or higher for master mode
  It is 72 for a 16mhz Wiring board with 100kHz TWI */
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

  //I2C control register
  TWCR = _BV(TWEN) |  //Enable I2C module
         _BV(TWIE) ;  //Enable I2C interrupts
}


void i2c_open(void) {
}


void i2c_close(void) {
}



/* void i2c_sandr(i2cr_addr_t node,
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
} */



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


/* request being processed by driver */
static i2cr_request_t *current_req; // could it be a pointer?



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
 * @brief Enable I2C interrupts. Global interrupts must be previously set.
 */
static void enable_i2c_interrupts(void) {
  TWCR |= (1<<TWIE);    //Sets the TWI interrupt enable bit
}

/**
 * @brief Send a start condition to the I2C bus as a Master.
 */
static void throw_start(void) {
  TWCR = (1<<TWINT)  |  //Clears the flag
         (1<<TWEN)   |  //Enable TWI. Hardware takes control over the I/O pins connected to the SCL and SDA pins
         (1<<TWSTA)  |  //Generates the START condition. Application desires to be Master on the bus
         (1<<TWIE);     //Enable I2C interrupts
}

/**
 * @brief Send a stop condition to the I2C bus.
 */
static void throw_stop(void) {
  TWCR =  (1<<TWINT) |  //Clears the flag
          (1<<TWEN)  |  //Enable TWI. Hardware takes control over the I/O pins connected to the SCL and SDA pins
          (1<<TWSTO);   //Generates the STOP condition.
}

/**
 * @brief Send a byte to the I2C bus. It can be either an address or data.
 * 
 * @param b The byte to be sent.
 */
static void throw_byte(uint8_t b) {
  TWDR = b;             //Load byte to TWDR data register
  TWCR = (1<<TWINT) |   //Clears the flag
         (1<<TWEN)  |   //Enable TWI
         (1<<TWIE);     //Enable I2C interrupts
}

/**
 * @brief Send a request byte to the I2C bus.
 * Used to ask for more data to a slave.
 */
static void throw_request_byte(bool is_last){
  if (is_last){
    TWCR &= ~(1<<TWEA); //Data byte will be received and NACK will be returned from Master
  } else {
    TWCR |= (1<<TWEA);  //Data byte will be received and ACK will be returned from Master
  }
  TWCR =  (1<<TWINT) |  //Clears the flag
          (1<<TWEN)  |  //Enable TWI. Hardware takes control over the I/O pins connected to the SCL and SDA pins
          (1<<TWIE);    //Enable I2C interrupts
}

/**
 * @brief Get the recieved byte from I2C.
 * The TWDR port is both used to send and recieve data.
 */
#define GET_RECIEVED_BYTE() TWDR


/*
 * Set `s` status to current request and
 *  - sends ReSTART and fetch new request from queue, or
 *  - sends STOP and goes to Idle state
 */
static void fetch_or_idle(i2cr_status_t s) {
  *(current_req->status) = s;
  if (i2cq_is_empty(&requests)) {
    throw_stop();
    disable_i2c_interrupts();
    ida_state = Idle;
  } else {
    // fetch next request and begin cycle again
    throw_start();
    current_req = i2cq_front(&requests);
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

  switch (ida_state) {
  case Idle:
    /* In idle state. Interrupts disabled */
    if (e == TW_GO_OPERATIVE) {
      // get next request, must exist for sure
      current_req = i2cq_front(&requests);
      i2cq_dequeue(&requests);
      // Throw a START to get the bus control
      throw_start();
      // next state
      ida_state = Starting;
    } else {
      // unexpected event
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
        // I2Csend
        throw_byte((current_req->node << 1) | TW_WRITE);
        ida_state = SeekingSlaveTx;
      }
    } else {
      // unexpected event
      // error();
    }
    break;

  case SeekingSlaveTx:
    if (e == TW_MT_SLA_ACK) {
      // Slave contacted, let's talk with him
      throw_byte(current_req->buffer[i++]);
      ida_state = TxData;
    } else if (e == TW_MT_SLA_NACK) {
      // Slave contacted and not available
      fetch_or_idle(SlaveRejected);
    } else {
      // Unexpected event
    }
    break;

  case TxData:
    if (e == TW_MT_DATA_ACK) {
      // Data sent ok
      if (current_req->length != i) {
        // Send another byte and remain in the same state
        throw_byte(current_req->buffer[i++]);
      } else {
        // No more data to send
        fetch_or_idle(Success);
      }
    } else if (e == TW_MT_DATA_NACK) {
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
      throw_request_byte(current_req->length == 1);
      ida_state = RxData;
    } else if (e == TW_MR_SLA_NACK) {
      // Slave contacted and not available
      fetch_or_idle(SlaveRejected);
    } else {
      // Unexpected event
    }
    break;

  case RxData:
    if (e == TW_MR_DATA_ACK) {
      // Get another byte and remain in the same state
      current_req->buffer[i++] = GET_RECIEVED_BYTE();
      //If just one byte remains, next status will be TW_MR_DATA_NACK
      throw_request_byte(i >= (current_req->length - 1));
    } else if (e == TW_MR_DATA_NACK) {
      //Get last byte
      current_req->buffer[i++] = GET_RECIEVED_BYTE();
      if (current_req->length == i) {
        // No more data to recieve
        fetch_or_idle(Success);
      } else {
        //Last byte has been recieved but number of recieved bytes is wrong
        fetch_or_idle(ReceivedMessageLenError);
      }
    } else {
      // Unexpected event
    }
    break;
  }
}



ISR(TWI_vect) {
  ida_next(TW_STATUS);
}


void i2c_send(i2cr_addr_t node,
	      uint8_t *buffer,
	      uint8_t length,
	      volatile i2cr_status_t *status) {
  i2cr_request_t r =
    {
     .rt = I2Csend,
     .node = node,
     .buffer = buffer,
     .length = length,
     .status = status,
    };

  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, r);
  
  if (ida_state == Idle){
    //Start the automata
    ida_next(TW_GO_OPERATIVE);
  }
}



void i2c_receive(i2cr_addr_t node,
		 uint8_t *const  buffer,
		 uint8_t length,
		 volatile i2cr_status_t *const status) {
  i2cr_request_t r =
    {
     .rt = I2Creceive,
     .node = node,
     .buffer = buffer,
     .length = length,
     .status = status,
    };

  while (i2cq_is_full(&requests));
  i2cq_enqueue(&requests, r);

  if (ida_state == Idle){
    //Start the automata
    ida_next(TW_GO_OPERATIVE);
  }
}

