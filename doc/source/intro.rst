************
Introduction
************


libAire is a bare C library for Arduino that abstracts the main
hardware devices. There are many known modules and libraries for
Arduino and the AVR microcontrolers. Why to design another one?
These are the most salient reasons:

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


Libaire modules structure
=========================

Libaire is built as a set of interelated related modules. These are the
modules and its main functionality.

adc
   Defines an abstraction of an analogic input channel.

alert
   Defines an alarm mechanism that allows to abort the system while
   giving to the technical user a primary hint on the cause of
   halt. It's a last resource emergency exception handler.

i2c
   An abstraction of an i2c bus.

pin
   An abstraction of a digital i/o pin.

queue
   A queue of bytes.

serial
   An abstraction of a serial port.

switch
   An abstraction of a switch.

ticker
   A counter that ticks at a constant frequency.

timer
   An abstraction of a timer that allows to asyncronously call a
   function in the future.



Microcontroller, platform and devices
=====================================

On this documentation we use the following terms with the defined
sense:

microcontroller family

   A set of many microcontroler models that share the same or almost
   the same assembler level architecture. For instance megaAVR.

microcontroller model

   A specific member of a microcontroler family. For instance, the AVR
   ATMega 328.

platform

   An circuit that surrounds a microcontroller and renders it usable
   to some tasks. Usually in the form of a printed circuit. For
   instance Arduino ONE.

device

   An external device connected to a platform that complements its
   functions. For instance, a specific Arduino shield with relays.
   


Libaire hardware dependencies
-----------------------------

Libaire depends mainly on the model of AVR microcontroler family
because of the dependencies on microcontroler peripheral devices: A/D
conversor, GPIO ports, timers, etc.

However, as most of users are on Arduino comunity, we choosed to make
libaire to depend on an Arduino platform. The actual Arduino platform
also determines the model of AVR microcontroler.

The default platform is Arduino ONE.



Relationship between libaire and libdev
=======================================

libaire contains the software that is platform dependent.

libdev contains the software that is device dependent.


