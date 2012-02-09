
BUILDDIR=build
ZMQVERSION=2.1.11
ZMQTAR=zeromq-$(ZMQVERSION).tar.gz
ZMQDIR=$(BUILDDIR)/zeromq-$(ZMQVERSION)
ZMQURL=http://download.zeromq.org/$(ZMQTAR)

ULVERSION=2.20.1
ULTAR=util-linux-$(ULVERSION).tar.gz
ULDIR=$(BUILDDIR)/util-linux-$(ULVERSION)
ULURL=http://www.kernel.org/pub/linux/utils/util-linux/v2.20/util-linux-$(ULVERSION).tar.gz

CFLAGS+=-Iinclude 

# ZMQ requires all of these
# TODO(sissel): -lrt is only required on Linux, not FreeBSD, etc, I think...
CFLAGS+=-pthread
LIBS+=-lstdc++ -lrt
LDFLAGS=-pthread

VPATH=$(BUILDDIR)/

zmqexec: zmqexec.o libzmq.a libuuid.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

zmqexec.c: include/zmq.h include/zmq_utils.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -r $(BUILDDIR)

$(BUILDDIR)/:
	mkdir $@

$(ZMQTAR): | $(BUILDDIR)
	wget $(ZMQURL) || curl -o $@ $(ZMQURL)

$(ZMQDIR)/: | $(ZMQTAR) $(BUILDDIR)/
	tar -zxf $(ZMQTAR)

$(ZMQDIR)/src/.libs/libzmq.a: | $(ZMQDIR)/
	(cd $(ZMQDIR); ./configure)
	$(MAKE) -C $(ZMQDIR)

$(BUILD)/include/:
	mkdir $@

$(BUILD)/include/zmq.h $(BUILD)/include/zmq_utils.h: | $(ZMQDIR)/ $(BUILD)/include/
	cp $(ZMQDIR)/$@ $@

$(BUILDDIR)/libzmq.a: | $(ZMQDIR)/src/.libs/libzmq.a
	cp $| $@

$(ULTAR): | $(BUILDDIR)
	wget -O $@ $(ULURL) || curl -o $@ $(ULURL)

$(ULDIR)/: | $(ULTAR) $(BUILDDIR)/
	tar -zxf $(ULTAR)

$(ULDIR)/libuuid/src/.libs/libuuid.a: | $(ULDIR)/
	cd $(ULDIR); ./configure
	make -C $(ULDIR)

$(BUILD)/libuuid.a: | $(ULDIR)/libuuid/src/.libs/libuuid.a
	cp $| $@
