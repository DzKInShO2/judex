SOURCES := $(wildcard src/*.c)
INCLUDES = -Ilib/raylib-nuklear/include

CFLAGS = -Wall -Wpedantic -std=c11
LDFLAGS = -lraylib

all:
	cc -o judex ${SOURCES} ${INCLUDES} ${CFLAGS} ${LDFLAGS}

run:
	./judex
