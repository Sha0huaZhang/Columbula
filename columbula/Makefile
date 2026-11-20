NAME    := columbula
SRC     := src/main.c src/warble.c src/platform.c
HDR     := src/version.h src/warble.h src/platform.h src/warble_table.h
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra -Werror
DIST    := dist
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Darwin)
BIN   := $(DIST)/$(NAME)-darwin-universal
LINK  := cc $(CFLAGS) -mmacosx-version-min=12.0 -arch arm64 -arch x86_64
else ifeq ($(UNAME_S),Linux)
BIN   := $(DIST)/$(NAME)-linux-$(UNAME_M)
LINK  := gcc $(CFLAGS) -static
else
$(error macOS / Linux 用 make，Windows 用 build.bat)
endif

.PHONY: all check clean
.DELETE_ON_ERROR:

all: $(BIN)

$(BIN): $(SRC) $(HDR)
    @mkdir -p $(DIST)
    $(LINK) -o $@ $(SRC)
    @strip $@ 2>/dev/null || true

src/warble_table.h: lyrics.txt tools/gen_table.awk
    @mkdir -p src
    @LC_ALL=C awk -f tools/gen_table.awk lyrics.txt > $@

check: $(BIN)
    ./$(BIN) | diff -u tests/expected.txt -

clean:
    rm -rf $(DIST) src/warble_table.h
