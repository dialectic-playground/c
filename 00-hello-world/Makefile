# 00-hello-world: Standard Terminal Hello World, in C17.
#
# The universal build contract (shared/templates/BASE-MAKEFILE) defines the standard targets:
#
#   make all | install | uninstall | test | check | lint | format | clean | distclean
#   make test-hidden | grade | help
#
# The hooks below compile src/dp_hello/ with the platform's C compiler (GCC in the container
# and on Linux, Apple Clang on macOS, MinGW-w64 GCC on Windows), run the plain-C unit tests in
# src/tests/, and check formatting with clang-format 19 (style: ../.clang-format).
# Container-first: goals run in the toolchain container built from ../Containerfile
# (podman, then docker, then local; override with DP_RUNTIME). DP_RUNTIME=local builds natively.

PROJECT := 00-hello-world
TOOLS   := dp-hello

# Path to the sdlc-curriculum checkout (the specs, test suites, and runner live there).
CURRICULUM_ROOT ?= ../sdlc-curriculum

# Native compilers differ by OS (GCC, Clang, MinGW); default to the platform's C compiler.
ifeq ($(origin CC),default)
  CC := $(if $(filter Windows_NT,$(OS)),gcc,cc)
endif
CLANG_FORMAT ?= clang-format

BUILD_DIR := build
BIN_DIR   := $(BUILD_DIR)/bin

APP_SOURCES  := $(wildcard src/dp_hello/*.c)
LIB_SOURCES  := $(filter-out src/dp_hello/main.c,$(APP_SOURCES))
TEST_SOURCES := $(wildcard src/tests/*.c)
FORMAT_FILES := $(wildcard src/dp_hello/*.[ch] src/tests/*.[ch])

WARNINGS := -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wconversion
CFLAGS   ?= -O2
DP_CFLAGS = -std=c17 $(WARNINGS) $(CFLAGS)
# Windows: enter through wmain to receive the UTF-16 command line (see src/dp_hello/main.c).
APP_LDFLAGS := $(if $(filter Windows_NT,$(OS)),-municode)

UNIT_TESTS := $(BUILD_DIR)/tests/unit-tests$(EXE)

# $(call compile,FLAGS,OUTPUT,SOURCES,LDFLAGS)
compile = $(CC) $(DP_CFLAGS) $(1) -o $(2) $(3) $(LDFLAGS) $(4)

define DP_BUILD
@mkdir -p $(BIN_DIR)
$(call compile,,$(BIN_DIR)/dp-hello$(EXE),$(APP_SOURCES),$(APP_LDFLAGS))
endef

define DP_INSTALL
mkdir -p "$(BINDIR)"
cp $(BIN_DIR)/dp-hello$(EXE) "$(BINDIR)/dp-hello$(EXE)"
endef

define DP_UNINSTALL
rm -f "$(BINDIR)/dp-hello$(EXE)"
endef

define DP_UNIT_TEST
@mkdir -p $(BUILD_DIR)/tests
$(call compile,,$(UNIT_TESTS),$(LIB_SOURCES) $(TEST_SOURCES))
$(UNIT_TESTS)
endef

# Lint: every source must compile without a single warning, and match the clang-format style.
define DP_LINT
@mkdir -p $(BUILD_DIR)/lint
$(call compile,-Werror,$(BUILD_DIR)/lint/dp-hello$(EXE),$(APP_SOURCES),$(APP_LDFLAGS))
$(call compile,-Werror,$(BUILD_DIR)/lint/unit-tests$(EXE),$(LIB_SOURCES) $(TEST_SOURCES))
$(CLANG_FORMAT) --dry-run --Werror $(FORMAT_FILES)
endef

define DP_FORMAT
$(CLANG_FORMAT) -i $(FORMAT_FILES)
endef

define DP_CLEAN
rm -rf $(BUILD_DIR)
endef

# Nothing else is cached in the project directory: no dependencies, no generated config.
define DP_DISTCLEAN
rm -rf $(BUILD_DIR)
endef

include $(CURRICULUM_ROOT)/shared/templates/BASE-MAKEFILE
