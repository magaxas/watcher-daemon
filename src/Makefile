CC:=gcc
LDFLAGS:=-L../lib -lcjson
CPPFLAGS:=-I../lib
CFLAGS:=-Wall

BIN:=main
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)
