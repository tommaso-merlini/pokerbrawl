SOURCES := $(shell find src -type f -name '*.c')
HEADERS := $(shell find src -type f -name '*.h')

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iraylib/src
LDLIBS = raylib/build/raylib/libraylib.a -lm -ldl -lpthread -lGL -lrt -lX11

game: $(SOURCES) $(HEADERS)
	cc $(CFLAGS) $(SOURCES) -o game $(LDLIBS)
