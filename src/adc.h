#ifndef _ADC_H_
#define _ADC_H_

/**
 * \addtogroup adc
 * \file adc.h
 *
 * \brief Abstracts the AVR analogic to digital peripheral.
 *
 * From this module standpoint, the AVR ADC services are based on:
 *  -# A set of physical analogic channels attached to physical pins 
 *     (some of them shared with digital ports); 
 *  -# A set of reference sources that are used to
 *     compare with the analogic signals beig converted.
 *  -# A hardware that reads a physical channel, compares
 *     its value to a reference source and returns the corresponding
 *     digital value.
 *
 * The center of this module is the #sdc_channel. An #sdc_channel is
 * a logical file-like object that abstracts a physical analogic
 * channel together with its reference source.
 * 
 * The functions of this module are operations on the #sdc_channel. A
 * typical use would follow this pattern:
 *
 *  -# Bind the logical channel to a physical channel and a reference
 *     source.
 *  -# Prepare the logical channel to be sampled.
 *  -# Request a sampling action to the channel.
 *  -# When sampled: get the value.
 *  -# Unbound the channel.
 *
 * Some functions are canned combinations of basic operations to allow
 * an easier use on simple cases.
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
 * \name Reference voltage sources
 *
 * Reference sources are the reference voltage that is used to
 * during the adc conversion. Every physical channel is associated to
 * a reference source when the logical channel is bound. 
 */
///@{
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
///@}



/**
 * \brief An adc channel proxy object.
 *
 * This is a proxy that represents a hardware analogic input which can
 * be asked for a conversion.
 */
typedef uint8_t adc_channel;



/**
 * \brief Binds the channel to a given port and reference voltage source. 
 *
 * This function has an 'open'-like semantics. It should be called
 * before any other operations on the channel object. It binds the
 * channel object to a physical channel and a reference source. If a
 * digital port is in use that overlaps the same pin that this analog
 * port, the digital port is disabled.
 *
 * \throws ALRT_INCOMPATIBLE_ADC_REF If any incompatibility arises
 * between reference sources of the currently bound adc channels.
 *
 * \param[in] ch: Analog port number.
 * \param[in] ref: Reference voltage source for this channel.
 *
 * \return An bound ::adc_channel
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

/*! start to read from last prepared channel */
void adc_start_conversion(void);

/*! prepare to convert from channel `ch` and start a conversion */
void adc_prepare_start(adc_channel ch);

/*! true iff last started conversion is running */
bool adc_converting(void);

/*! get the value of the last started conversion */
uint8_t adc_get(void);



/*! read from adc channel `ac` until read done.  Typically about 30us
 * of waiting time. Longuer waiting time if it needs to change
 * physical channel and/or reference voltage from last conversion.
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
