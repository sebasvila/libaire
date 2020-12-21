.PHONY: all implanta clean veryclean

all:
	$(MAKE) -C build

implanta: all
	$(MAKE) -C build implanta_main

clean:
	$(MAKE) -C build clean
	cd src;  \rm -f *~ 
	cd test; \rm -f *~

veryclean: clean
	$(MAKE) -C build veryclean
	cd src;  \rm -f \#*\#
	cd test; \rm -f \#*\#

