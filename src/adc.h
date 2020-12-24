#ifndef _ADC_H_
#define _ADC_H_

/**
 * \file adc.h
 * \brief Abstracts the AVR analogic to digital peripheral.
 */

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

/** 
 * Max value returned by adc module conversion 
 */
#define ADC_MAX 0xff

/** 
 * @brief Traslates adc value to a given range
 * 
 * The output of the adc is given as an integer in the range
 * [0..::ADC_MAX]. This macro translates it to the range [m,M].
 *
 * @param m: Min value of the target range
 * @param M: Max value of the target range
 */
#define ADC_VALUE(v,M,m) \
  ((int32_t)m + ((int32_t)M-(int32_t)m)*(int32_t)v/(int32_t)ADC_MAX)


/** 
 * \name Special analogic ports
 *
 * Special ports used when an application needs an analogic port
 * with known characteristics. 
 */
///@{
#define ADC_CHANNEL_0V  017  /**< A port that always gives 0V */
#define ADC_CHANNEL_11V 016  /**< A port that always gives 1.1V */
///@}


/** 
 * @brief A reference voltage source.
 * 
 * @remark If some channel is bound to Aref, other references should be avoided
 * @see    ¶23.5.2 pp. 211 of the Datasheet
 */
typedef enum {
  Aref=0,  /**< Use external Aref as a reference */
  Vcc=1,   /**< Use device Vcc as reference */
  Int11=3  /**< Use internal 1.1V source as reference */
} adc_ref;


/**
 * \brief An adc channel proxy object.
 *
 * This is a proxy that represents a hardware analogic input which can
 * be asked for a conversion.
 */
typedef uint8_t adc_channel;



/**
 * @brief Binds the channel to a given port and reference voltage source. 
 *
 * This function has an 'open'-like semantics. It should be called
 * before any other operations on the channel object. It binds the
 * channel object to a physical channel and a reference source. If a
 * digital port is in use that overlaps the same pin that this analog
 * port, the digital port is disabled.
 *
 * @throws ALRT_INCOMPATIBLE_ADC_REF If any incompatibility arises
 * between reference sources of the currently bound adc channels.
 *
 * @param ch: Analog port number.
 * @param ref: Reference voltage source for this channel.
 *
 * @return An bound ::adc_channel
 */
adc_channel adc_bind(uint8_t ch, adc_ref ref);

/**
 * @brief Unbind a previously bound channel.
 *
 * @param ch A bound ::adc_channel to be unbound.
 */
void adc_unbind(adc_channel *const ch);
		     

/**
 *  @brief Prepare to convert from channel ch
 *
 *  Prepares a bound logical channel `ch` to be queried.
 *  You can only query the last prepared channel. There is no 
 *  chance to read several channels concurrently.
 *
 *  @param ch The bound logical channel to be prepared.
 */
void adc_prepare(adc_channel ch);


/**
 * @brief Starts a sampling on the last prepared channel 
 */
void adc_start_conversion(void);

/**
 * @brief True iff the last started conversion is still running 
 *
 */
bool adc_converting(void);

/**
 * @brief Gets the value of the last started conversion
 * 
 * Can only be applyed after conversion ended. Refer to
 * adc_converting() to test this condition.
 *
 * @returns A value in range [0..::ADC_MAX]
 */
uint8_t adc_get(void);



/**
 * @brief Prepare to convert from a channel and starts a conversion 
 *
 * An utility function that first calls adc_prepare() and 
 * then adc_start_conversion().
 * 
 * @param ch : The adc_channel to be prepared
 */
void adc_prepare_start(adc_channel ch);

/**
 * @brief Prepares, starts and reads a channel
 *
 * An utility function that prepares a channel, starts a conversion, and, 
 * when ready, reads the sampled value.  Typically lasts around of 30us
 * of waiting time. It may spand a longuer waiting time if it needs to change
 * physical channel and/or reference voltage from last conversion.
 *
 * @param ch: The logical channel to be sampled
 * @returns A sampled value in range [0..::ADC_MAX]
 */
uint8_t adc_prep_start_get(adc_channel ch);





/*
 * Adjust channel range.
 * When Vcc external reference is used, this function can be used to
 * calibrate against the internal 1.1V reference. The function returns
 * the real voltage corresponding to Vcc. 
 */
// float adc_adjust(void); not implemented yet


/*
 * Oversampling conversion.
 * Four continuous measures of the same channel are made and the 
 * mean returned. Measuring span is of 13.5*4=54 cycles, approx.
 * 108 us.
 */
void adc_start_oversample(void);
bool adc_oversampling(void);
uint8_t adc_get_oversample(void);


/* initialize the module */
void adc_setup(void);

#endif
