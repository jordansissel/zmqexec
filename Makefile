
ZMQVERSION=2.1.11
ZMQTAR=zeromq-$(ZMQVERSION).tar.gz
ZMQDIR=zeromq-$(ZMQVERSION)
ZMQURL=http://download.zeromq.org/$(ZMQTAR)

ULVERSION=2.20.1
ULTAR=util-linux-$(ULVERSION).tar.gz
ULDIR=util-linux-$(ULVERSION)
ULURL=http://www.kernel.org/pub/linux/utils/util-linux/v2.20/util-linux-$(ULVERSION).tar.gz

CFLAGS+=-Iinclude 

# ZMQ requires all of these
# TODO(sissel): -lrt is only required on Linux, not FreeBSD, etc, I think...
CFLAGS+=-pthread
LIBS+=-lstdc++ -lrt
LDFLAGS=-pthread

zmqexec: zmqexec.o libzmq.a libuuid.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

zmqexec.c: include/zmq.h include/zmq_utils.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -r $(ZMQTAR) $(ZMQDIR) include/ $(ULTAR) $(ULDIR) zmqexec *.o *.a *.so

$(ZMQTAR):
	wget $(ZMQURL) || curl -o $@ $(ZMQURL)

$(ZMQDIR)/: | $(ZMQTAR)
	tar -zxf $(ZMQTAR)

$(ZMQDIR)/src/.libs/libzmq.a: | $(ZMQDIR)/
	(cd $(ZMQDIR); ./configure)
	$(MAKE) -C $(ZMQDIR)

include/:
	mkdir $@

include/zmq.h include/zmq_utils.h: | $(ZMQDIR)/ include/
	cp $(ZMQDIR)/$@ $@

libzmq.a: | $(ZMQDIR)/src/.libs/libzmq.a
	cp $| $@

$(ULTAR):
	wget $(ULURL) || curl -o $@ $(ULURL)

$(ULDIR)/: | $(ULTAR)
	tar -zxf $(ULTAR)

$(ULDIR)/libuuid/src/.libs/libuuid.a: | $(ULDIR)/
	cd $(ULDIR); ./configure
	make -C $(ULDIR)

libuuid.a: | $(ULDIR)/libuuid/src/.libs/libuuid.a
	cp $| $@
