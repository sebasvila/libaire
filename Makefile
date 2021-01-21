.PHONY: lib doc install dist clean veryclean

lib:
	$(MAKE) -C build lib

doc:
	$(MAKE) -C doc html

install:
	$(MAKE) -C build install

dist:
	$(MAKE) -C buid dist

clean:
	$(MAKE) -C build clean
	$(MAKE) -C doc clean
	cd src;  \rm -f *~ 
	cd test; \rm -f *~

veryclean: clean
	$(MAKE) -C build veryclean
	cd src;  \rm -f \#*\#
	cd test; \rm -f \#*\#

