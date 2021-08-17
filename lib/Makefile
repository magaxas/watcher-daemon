CC:=gcc
CFLAGS:=-Wall -fpic -shared
LIB:=libcjson.so
PREFIX?=/usr

SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)
HEADERS=$(wildcard *.h)

.PHONY: all clean install uninstall

all: $(LIB)

$(LIB): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

install:
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include
	install -m 444 $(LIB) $(PREFIX)/lib/$(LIB)
	install -m 444 $(HEADERS) $(PREFIX)/include/

uninstall:
	$(RM) $(foreach header,$(HEADERS),$(PREFIX)/include/$(header))
	$(RM) $(PREFIX)/lib/$(LIB)

clean:
	$(RM) $(LIB) $(OBJS)
