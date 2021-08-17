.PHONY: rebuild lib bin clean all install

all: lib bin
rebuild: clean all

lib:
	$(MAKE) -C ./lib

bin:
	$(MAKE) -C ./src

install:
	$(MAKE) -C ./lib install

uninstall:
	$(MAKE) -C ./lib uninstall

clean:
	$(MAKE) -C ./lib clean
	$(MAKE) -C ./src clean
