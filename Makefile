.PHONY: rebuild lib bin clean all install debug

all: lib bin
rebuild: clean all
debug: clean lib debug

lib:
	$(MAKE) -C ./lib

debug: 
	$(MAKE) -C ./src debug

bin:
	$(MAKE) -C ./src

install:
	$(MAKE) -C ./lib install

uninstall:
	$(MAKE) -C ./lib uninstall

clean:
	$(MAKE) -C ./lib clean
	$(MAKE) -C ./src clean
