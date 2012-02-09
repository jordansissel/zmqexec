BUILDDIR=build

%: $(BUILDDIR)/Makefile
	make -C $(BUILDDIR) $@

.PHONY: clean
clean:
	-rm -fr $(BUILDDIR)

$(BUILDDIR)/:
	mkdir $(BUILDDIR)

$(BUILDDIR)/Makefile: Makefile.build | $(BUILDDIR)
	cp $< $@
