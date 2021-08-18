CC:=gcc
LDFLAGS:=-L../lib -lcjson
CPPFLAGS:=-I../lib
CFLAGS:=-Wall
PREFIX?=/usr

BIN:=watcher-daemon
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)

.PHONY: all clean debug install

debug: CFLAGS += -g3
debug: all

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)

install:
	install -d $(PREFIX)/bin/
	install -m 755 $(BIN) $(PREFIX)/bin/

uninstall:
	$(RM) $(PREFIX)/bin/$(BIN)
