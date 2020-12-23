#################################################################

PLATFORM = ArduinoONE
SHIELD   = ShieldITIC

DEVICE=/dev/ttyACM0

CC=avr-gcc
CPPFLAGS=
CFLAGS=-Wall -std=gnu99 -Os -fshort-enums
LDFLAGS=-std=c99
AR=avr-ar

#################################################################

SRC_LIB = libaire.a

SRC_MODS = adc ticker pin switch timer alert queue serial \
	   #i2cq i2c

SRC_TESTS = test_timer test_timer2 \
            test_pin \
            test_ticker \
	    test_adc test_adc_2 \
            test_switch_1 test_switch_2 test_switch_3 \
            test_alert_1 \
	    test_serial_1 test_serial_2 test_serial_3 test_serial_4 \
            test_serial_5

# Library deps
$(SRC_LIB): $(SRC_LIB)($(addsuffix .o, $(SRC_MODS)))

# Link rules

test_adc: adc.o ticker.o shielditic.o alert.o pin.o
test_adc_2: adc.o ticker.o shielditic.o alert.o pin.o
test_ticker: ticker.o shielditic.o
test_timer: pin.o timer.o
test_timer2: pin.o timer.o
test_pin: pin.o
test_switch_1: switch.o shielditic.o
test_switch_2: switch.o shielditic.o
test_switch_3: switch.o shielditic.o ticker.o
test_switch_4: switch.o shielditic.o ticker.o adc.o
test_alert_1: alert.o pin.o
test_serial_1: serial.o queue.o
test_serial_2: serial.o queue.o
test_serial_3: serial.o queue.o adc.o ticker.o alert.o pin.o
test_serial_4: serial.o queue.o adc.o ticker.o alert.o pin.o
test_serial_5: serial.o queue.o adc.o ticker.o alert.o pin.o


#################################################################


# Platform and shield dependent parameters

ifeq ($(PLATFORM),ArduinoONE)
  MCU = atmega328p
  PROGRAMMER = arduino
  DUDEOPTS =
  FREQ = 16000000UL
else ifeq ($(PLATFORM),ArduinoMEGA)
  MCU = atmega2560
  PROGRAMMER = wiring
  DUDEOPTS = -D
  FREQ = 16000000UL
endif


ifeq ($(SHIELD),ShieldTIC)
endif


# Processing parameters

CFLAGS   +=  -mmcu=$(MCU)
LDFLAGS  +=  -mmcu=$(MCU)
CPPFLAGS +=  -D$(PLATFORM) -D$(SHIELD) -DF_CPU=$(FREQ) -I$(SRCDIR)


# Project structure

DEPSDIR = .deps
SRCDIR  = ../src
TESTDIR = ../test
BUILDDIR = .


# Sets of files

SOURCES = $(SRC_MODS) $(SRC_TESTS)
DEPFILES = $(addprefix $(DEPSDIR)/, $(addsuffix .d, $(SOURCES)))

# Search path

VPATH = $(SRCDIR):$(TESTDIR)


# Default targets

.PHONY: all tests
all:	$(SRC_LIB)
tests:  $(SRC_TESTS)


# Utility targets

.PHONY: clean veryclean
clean:
	@\rm -f *~ *.o *.s *.hex

veryclean: clean
	@\rm -f  \#*\# $(SRC_LIB) $(SRC_TESTS)
	@\rm -rf $(DEPSDIR)



# Generic rules

%.hex: %
	avr-objcopy -Oihex  $< $@

implanta_%: %.hex
	avrdude $(DUDEOPTS) -c $(PROGRAMMER) -p $(MCU) -P $(DEVICE) -U $<

%.s: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -S $<

$(DEPSDIR)/%.d: %.c | $(DEPSDIR)
	@set -e; rm -f $@; \
        $(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(DEPSDIR):
	 @mkdir -p $@


# Automagic dependencies

-include $(DEPFILES)
