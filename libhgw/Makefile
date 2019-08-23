CFLAGS+=`pkg-config dbus-1 gconf-2.0 --cflags` -I. -fPIC
LDFLAGS=-shared `pkg-config dbus-1 --libs`

MAJOR = 1
MINOR = 9
RELEASE = 4

VERSION = $(MAJOR).$(MINOR).$(RELEASE)

OBJS = hgw_init.o hgw_conf.o hgw_callbacks.o hgw_msg.o \
       hgw_compat.o hgw_priv.o
HEADERS = hgw.h shadowapp.h hgw_priv.h


all: libhgw.so.$(VERSION)

libhgw.so.$(VERSION): $(OBJS)
	$(CC) -o libhgw.so.$(VERSION) -Wl,-soname=libhgw.so.$(MAJOR) \
	$(LDFLAGS) $(OBJS)
	ln libhgw.so.$(VERSION) -fs libshadowapp.so.0
	ln libhgw.so.$(VERSION) -fs libhgw.so.$(MAJOR)
	ln libhgw.so.$(VERSION) -fs libhgw.so

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o *.so*

install:
	install -d $(DESTDIR)$(PREFIX)/lib
	install -m755 libhgw.so.$(VERSION) $(DESTDIR)$(PREFIX)/lib
	ln $(PREFIX)/lib/libhgw.so.$(VERSION) -sf \
	$(DESTDIR)$(PREFIX)/lib/libshadowapp.so.0
	ln $(PREFIX)/lib/libhgw.so.$(VERSION) -sf \
	$(DESTDIR)$(PREFIX)/lib/libshadowapp.so
	ln $(PREFIX)/lib/libhgw.so.$(VERSION) -sf \
	$(DESTDIR)$(PREFIX)/lib/libhgw.so.$(MAJOR)
	ln $(PREFIX)/lib/libhgw.so.$(VERSION) -sf \
	$(DESTDIR)$(PREFIX)/lib/libhgw.so
	install -d $(DESTDIR)$(PREFIX)/include/hgw/
	install -m644 hgw.h $(DESTDIR)$(PREFIX)/include/hgw/
	install -m644 shadowapp.h $(DESTDIR)$(PREFIX)/include/

