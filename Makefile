BUILDDIR=build

default: zmqexec

.PHONY: clean
clean:
	-rm -fr $(BUILDDIR)

# Any other make target should invoke the build/ make
.DEFAULT:
	@-[ ! -d $(BUILDDIR)/ ] && mkdir $(BUILDDIR)/
	$(MAKE) -C $(BUILDDIR) -f ../Makefile.build SRC=$(PWD)/src $@ 

