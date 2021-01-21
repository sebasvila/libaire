************
Introduction
************


libAire is a bare C library for Arduino that abstracts the main
hardware devices. There are many known modules and libraries for
Arduino and the AVR microcontrolers. Why to design another one?  These
are the most salient reasons:

1. Because many of them are written in C++ as a sequel of the
   preeminence of Arduino's IDE.

2. Because many of them offer a poor abstraction level of the
   hardware. Some of the AVR peripherals are rather complex. This is
   the case, for instance, of the analogic-digital
   converter. Designing a suitable abstraction for it is not an easy
   task and most of the existing modules simply translate the hardware
   view to software level.
    
3. Because we want a design that plays nicely with protothreads and
   thus no hidden polling should be used. Many of the known modules
   use polling when talking to peripherals.

4. Because we want to use interrupts when possible as a way to
   leverage the work that the library user should do.


.. note::

   This documentation is mainly written for the library user. It
   deliberately avoids to exhibit the implementation details unless
   necessary or particulary interesting.


Libaire modules structure
=========================

Libaire is built as a set of modules. Each module is devoted to a
specific responsibility. Following, there is the modules list and its
main objective.

adc

   Defines an abstraction of the AVR analogic/digital converter system.

alert

   Defines an alarm mechanism that allows to abort the system while
   giving to the technical user a primary hint on the cause of
   halt. It's a last resource emergency exception handler.

i2c

   An abstraction of the AVR i2c bus that can be user in master send
   and master receive modes.

pin

   An abstraction of the AVR digital i/o pins.

serial

   An abstraction of the AVR serial port.

switch

   An abstraction of a on/off or pushbutton switch device.

ticker

   A hardware clock abstraction that offers to the library user a counter
   that ticks continuously at a constant frequency.

timer

   A hardware clock abstraction that offers to the library user a timer that
   allows to asyncronously call a function in the future.



Microcontroller, platform and devices
=====================================

On this documentation we use the following terms with the defined
meaning:

microcontroller family

   A set of many microcontroler models that share the same or almost
   the same assembler level architecture. For instance megaAVR.

microcontroller model

   A specific member of a microcontroler family. For instance, the AVR
   ATMega 328.

platform

   An circuit that surrounds a microcontroller and renders it usable
   to some tasks. Usually in the form of a printed circuit. For
   instance Arduino UNO.

device

   An external device connected to a platform that complements its
   functions. For instance, a specific Arduino shield with relays.
   


Libaire hardware dependencies
-----------------------------

Libaire implementation depends on the model of AVR microcontroler
family because of the singularities of each family peripheral devices:
A/D conversor, GPIO ports, timers, etc.

However, as most of users are on Arduino comunity, we choosed to make
libaire to depend on an Arduino platform. The actual Arduino platform
also determines the model of AVR microcontroler.

The default platform is Arduino UNO.



Relationship between libaire and libdev
=======================================

libaire contains the software that is platform dependent.

libdev contains the software that is device dependent.




