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
	@echo "  纯文本输出"
	@./$(BIN) | diff -u tests/expected.txt -
	@echo "  --version"
	@./$(BIN) -v | grep -Eqx '[0-9]+\.[0-9]+\.[0-9]+'
	@echo "  速度单位"
	@./$(BIN) -s 30ms -p | diff -q tests/expected.txt - >/dev/null
	@./$(BIN) -s=1.2s -p | diff -q tests/expected.txt - >/dev/null
	@./$(BIN) -s500 -p | diff -q tests/expected.txt - >/dev/null
	@echo "  非法速度被拒"
	@! ./$(BIN) -s 5x -p >/dev/null 2>&1
	@! ./$(BIN) -s >/dev/null 2>&1
	@! ./$(BIN) -s 0 -p >/dev/null 2>&1
	@echo "  全部通过"

clean:
	rm -rf $(DIST) src/warble_table.h
