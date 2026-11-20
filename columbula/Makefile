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

# PowerPC（Mac OS X 10.5 / gcc 4.2）。现代 clang 已经没有 ppc 后端了，
# 必须在 ppc 工具链上编：make ppc
PPC_BIN  := $(DIST)/$(NAME)-darwin-ppc
LINK_PPC ?= cc -std=gnu99 -O2 -Wall -Wextra -mmacosx-version-min=10.5 -arch ppc

# 交叉编译 Linux（zig 自带的 musl 工具链，产物完全静态，不依赖目标机任何运行库）
#   make ppc-linux    -> dist/columbula-linux-ppc     32 位大端 PowerPC
#   make arm64-linux  -> dist/columbula-linux-arm64   AArch64
ZIG_CC := zig cc -std=c11 -O2 -Wall -Wextra -Werror -Wl,-s -o

.PHONY: all check clean ppc ppc-linux arm64-linux
.DELETE_ON_ERROR:

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	@mkdir -p $(DIST)
	$(LINK) -o $@ $(SRC)
	@strip $@ 2>/dev/null || true

src/warble_table.h: lyrics.txt tools/gen_table.awk
	@mkdir -p src
	@LC_ALL=C awk -f tools/gen_table.awk lyrics.txt > $@

# ppc 用 gcc 4.2 编，不带 -Werror：老编译器的 -Wextra 警告比新 clang 多，
# 一旦 -Werror 会因为无关警告失败
ppc: $(SRC) $(HDR)
	@mkdir -p $(DIST)
	$(LINK_PPC) -o $(PPC_BIN) $(SRC)

ppc-linux: $(SRC) $(HDR)
	@mkdir -p $(DIST)
	$(ZIG_CC) $(DIST)/$(NAME)-linux-ppc -target powerpc-linux-musleabihf $(SRC)

arm64-linux: $(SRC) $(HDR)
	@mkdir -p $(DIST)
	$(ZIG_CC) $(DIST)/$(NAME)-linux-arm64 -target aarch64-linux-musl $(SRC)

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
