INCLUDE=
LIBS=
CFLAGS=-g -Wall -Werror
LDFLAGS=
VERSION:=`date +%Y%m%d`
OBJS=\
	ebdel.o\
	ebget.o\
	ebput.o\
	ebseek.o\
	ebfind.o\
	ebscroll.o\
	ucs2.o\
	ebdump.o
DISTFILES=\
	Makefile\
	README\
	LICENSE\
	editbuffer.h\
	editbuffer.c
DEPS=
PROGRAM=editbuffer
LIB=${PROGRAM}.a

include Makefile.common
