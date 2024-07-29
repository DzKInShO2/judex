SOURCES := $(wildcard src/*.c)
SOURCES += lib/tinyfiledialogs/tinyfiledialogs.c

INCLUDES = -Ilib/raylib-nuklear/include -Ilib/tinyfiledialogs

CFLAGS = -Wall -Wpedantic -std=c11
LDFLAGS = -lraylib 

all:
	cc -o judex ${SOURCES} ${INCLUDES} ${CFLAGS} ${LDFLAGS} -ggdb

run:
	./judex
