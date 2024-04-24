#==========================================
#    Makefile: makefile for sl 5.05
#	Copyright 1993, 1998, 2014, 2019
#                 Toyoda Masashi
#		  (mtoyoda@acm.org)
#	Last Modified: 2023/07/08
#==========================================

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/share/man

CC      ?= clang
CFLAGS  ?= -O3 -Wall
LDFLAGS ?= -lncurses

SRC := $(wildcard *.c)
BIN := $(SRC:%.c=%)

all: $(BIN)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

.PHONY: install

install: all
	install -d $(DESTDIR)$(BINDIR)
	install -d $(addprefix $(DESTDIR)$(MANDIR)/,man1 ja/man1)
	install -m755 $(BIN) $(DESTDIR)$(BINDIR)
	install -m644 sl.1 $(DESTDIR)$(MANDIR)/man1
	install -m644 sl.1.ja $(DESTDIR)$(MANDIR)/ja/man1

.PHONY: clean

clean:
	rm -rf $(BIN)

.PHONY: distclean

distclean: clean
