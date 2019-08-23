all: wrapper

wrapper: main.o parser.o
	$(CC) -o wrapper main.o parser.o $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o wrapper

install:
	install -d $(DESTDIR)$(PREFIX)/games/wrapper/
	install -m755 wrapper $(DESTDIR)$(PREFIX)/games/wrapper/

