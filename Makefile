BUILDDIR=build

default: zmqexec

.PHONY: clean
clean:
	-rm -fr $(BUILDDIR)

$(BUILDDIR)/:
	mkdir $(BUILDDIR)

# Any other make target should invoke the build/ make
.DEFAULT:
	$(MAKE) $(MAKEARGS) $(BUILDDIR)/
	$(MAKE) $(MAKEARGS) -C $(BUILDDIR) -f ../Makefile.build $@

