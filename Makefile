#==========================================
#    Makefile: makefile for sl 5.04
#	Copyright 1993, 1998, 2014, 2019
#                 Toyoda Masashi
#		  (mtoyoda@acm.org)
#	Last Modified: 2019/03/19
#==========================================

CC=gcc
CFLAGS=-O3 -Wall

all: sl

sl: sl.c sl.h
	$(CC) $(CFLAGS) -o sl sl.c -lncurses

clean:
	rm -f sl

distclean: clean
