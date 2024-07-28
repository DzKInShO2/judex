SOURCES := $(wildcard src/*.c)
INCLUDES = -Ilib/raygui/src -Ilib/raygui/examples/custom_file_dialog/

CFLAGS = -Wall -Wpedantic -std=c11
LDFLAGS = -lraylib

all:
	cc -o judex ${SOURCES} ${INCLUDES} ${CFLAGS} ${LDFLAGS}

run:
	./judex
