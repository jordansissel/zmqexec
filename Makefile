BUILDDIR=build

default: zmqexec

.PHONY: clean
clean:
	-rm -fr $(BUILDDIR)

$(BUILDDIR)/:
	mkdir $(BUILDDIR)

# Any other make target should invoke the build/ make
.DEFAULT:
	$(MAKE) $(BUILDDIR)/
	$(MAKE) -C $(BUILDDIR) -f ../Makefile.build SRC=$(PWD)/src $@ 

