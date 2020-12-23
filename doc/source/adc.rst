The adc module
==============

Introduction
------------

The module adc contains an abstract interface to the AVR
analogic-to-digital conversion hardware.

An inherently complex issue in the AD conversor is the reliability of
the values obtained. Such values are influenced by a number of
details: the waiting times after changing the channel or the reference
source. This module hides to some extent the procedures of channel
change or reference source change while trying to guarantee reliable
enough read values. Note that some of the factors that induce
unreliable reading come from the electrical characteristics of the
signal source being converted. These kind of factors cannot be
alleviated by the module mechanisms.



From this module standpoint, the AVR ADC services are based on:

1. A set of physical analogic channels attached to physical pins (some
   of them shared with digital ports);
2. A set of reference sources that are used to
   compare with the analogic signals beig converted.
3. A hardware that reads a physical channel, compares its value to a
   reference source and returns the corresponding digital value.

The center of this module is the adc_channel. An adc_channel is a
logical file-like object that abstracts a physical analogic channel
together with its reference source.

The functions of this module are operations on the adc_channel. A
basic use would follow this pattern:

1. Bind the logical channel to a physical channel and to a reference
   source.
2. Prepare the logical channel to be sampled.
3. Request the channel to begin a sampling.
4. When sampled: get the value obtained.
5. Unbound the channel.

Some functions are canned combinations of basic operations.


Module API
----------


The sampling range
++++++++++++++++++

The sampling vaules obtained are always defined between zero and this
constant:

.. doxygendefine:: ADC_MAX

Additionally, a utility macro is defined that allows to map any sample
value obtained to a given range:

.. doxygendefine:: ADC_VALUE


The reference sources
+++++++++++++++++++++

A logical channel is bound to a specific reference source. The signal
of this logical channel will be compared to the channel reference
source to get a digital value at sampling time. Several reference
sources are available.

The "external reference source" is a mode that allows for a external
voltage source to be used as a reference. The AVR A/D conversor poses
some contraints on the use of this reference: XX The module will
enforce this constraint.

.. doxygenenum:: adc_ref
		   

The basic operations
++++++++++++++++++++

.. doxygenfunction:: adc_bind


Examples
--------

A basic example working with a single channel. We sample four times
the channel.

.. code-block:: c

   #include <adc.h>

   int main() {
     adc_channel c;

     /* bind the logical channel channel */
     c = adc_bind(3, Int11);
     /* prepare channel `c` to be sampled */
     adc_prepare(c);
     /* start conversion on prepared channel */
     for(int i=0; i<4; ++) {
       adc_start_conversion();
       /* wait sampling and conversion ends */
       while (adc_converting());
       /* output the result */
       put(adc_get());
     }

     return 0;
   }

In the following example, we practice round robin sampling on two
channels. Note how the slow `put()` operation is executed while
waiting for the next conversion done. This allows for a faster
sampling rate.

.. code-block:: c

   #include <adc.h>

   int main() {
     adc_channel c1, c2;
     uint8_t s1, s2;

     /* bind the logical channel channel */
     c1 = adc_bind(3, Int11);
     c2 = adc_bind(4, Int11);
     /* do sampling */
     adc_prepare(c2);
     adc_start_conversion();
     while (adc_converting());
     s2 = adc_get();       
     for(;;) {
       adc_prepare(c1);
       adc_start_conversion();
       put(s2);
       while (adc_converting());
       s1 = adc_get();
       adc_prepare(c2);
       adc_start_conversion();
       put(s1);
       while (adc_converting());
       s2 = adc_get();       
     }

     return 0;
   }
   


