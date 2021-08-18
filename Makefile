PREFIX?=/usr/lib
SERVICE:=watcher-daemon.service

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
	$(MAKE) -C ./src install
	install -d $(PREFIX)/systemd/system/
	install -m 644 $(SERVICE) $(PREFIX)/systemd/system/

uninstall:
	$(MAKE) -C ./lib uninstall
	$(MAKE) -C ./src uninstall
	$(RM) $(PREFIX)/systemd/system/$(SERVICE)

clean:
	$(MAKE) -C ./lib clean
	$(MAKE) -C ./src clean
