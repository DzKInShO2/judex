SOURCES := $(wildcard src/*.c)
SOURCES := ${SOURCES} lib/sfd/src/sfd.c

INCLUDES = -Ilib/raylib-nuklear/include -Ilib/sfd/src

CFLAGS = -Wall -Wpedantic -w -s -O2
LDFLAGS = -lraylib 

all:
	cc -o judex ${SOURCES} ${INCLUDES} ${CFLAGS} ${LDFLAGS}

run:
	./judex
