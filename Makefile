CC=gcc
#-O2 -g -Wall coming from debian/rules...
HGW_CFLAGS="$(CFLAGS) -Werror"
PREFIX=/usr

all:
	CFLAGS=$(HGW_CFLAGS) make -C wrapper2
	CFLAGS=$(HGW_CFLAGS) make -C libhgw

clean:
	make -C wrapper2 clean
	make -C libhgw clean

install:
	PREFIX="$(PREFIX)" make -C wrapper2 install
	PREFIX="$(PREFIX)" make -C libhgw install

