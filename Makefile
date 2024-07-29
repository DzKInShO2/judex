SOURCES := $(wildcard src/*.c)
SOURCES := ${SOURCES} lib/sfd/src/sfd.c

INCLUDES = -Ilib/raylib-nuklear/include -Ilib/sfd/src

CFLAGS = -Wall -Wpedantic
LDFLAGS = -lraylib 

all:
	cc -o judex ${SOURCES} ${INCLUDES} ${CFLAGS} ${LDFLAGS} -ggdb

run:
	./judex
