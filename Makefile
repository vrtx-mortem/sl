PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/share/man

CC      ?= clang
CFLAGS  ?= -O3 -Wall
LDFLAGS ?= -lncurses
STRIP   ?= strip

SRC := $(wildcard *.c)
BIN := $(SRC:%.c=%)

all: sl.h $(BIN)

sl.h:
	sed -e 's/\\/\\\\/g' sl.def.h > $@

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	$(STRIP) $@

.PHONY: install

install: all
	install -d $(DESTDIR)$(BINDIR)
	install -d $(addprefix $(DESTDIR)$(MANDIR)/,man1 ja/man1)
	install -m755 $(BIN) $(DESTDIR)$(BINDIR)
	install -m644 sl.1 $(DESTDIR)$(MANDIR)/man1

.PHONY: clean

clean:
	rm -rf $(BIN) sl.h

.PHONY: distclean

distclean: clean
