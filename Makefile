SOURCES := $(shell find src -type f -name '*.c')
HEADERS := $(shell find src -type f -name '*.h')

# Piattaforma: rilevata automaticamente, oppure forzala con
#   make PLATFORM=linux   |   make PLATFORM=macos
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  PLATFORM ?= macos
else
  PLATFORM ?= linux
endif

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

ifeq ($(PLATFORM),macos)
  # Usa raylib compilata localmente se presente, altrimenti quella di Homebrew
  # (brew install raylib).
  ifneq ($(wildcard raylib/build/raylib/libraylib.a),)
    RAYLIB_INC = raylib/src
    RAYLIB_LIB = raylib/build/raylib/libraylib.a
  else
    BREW_RAYLIB := $(shell brew --prefix raylib 2>/dev/null)
    RAYLIB_INC = $(BREW_RAYLIB)/include
    RAYLIB_LIB = $(BREW_RAYLIB)/lib/libraylib.a
  endif
  CFLAGS += -I$(RAYLIB_INC)
  LDLIBS = $(RAYLIB_LIB) -framework OpenGL -framework Cocoa -framework IOKit \
           -framework CoreVideo -framework CoreAudio -framework AudioToolbox
else ifeq ($(PLATFORM),linux)
  CFLAGS += -Iraylib/src
  LDLIBS = raylib/build/raylib/libraylib.a -lm -ldl -lpthread -lGL -lrt -lX11
else
  $(error PLATFORM non valida: '$(PLATFORM)' (usa linux o macos))
endif

game: $(SOURCES) $(HEADERS) Makefile
	cc $(CFLAGS) $(SOURCES) -o game $(LDLIBS)

clean:
	rm -f game

.PHONY: clean
