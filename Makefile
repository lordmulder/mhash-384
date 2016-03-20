#############################################################################
# BUILD OPTIONS
#############################################################################

PLUSPLUS ?= 0
STATIC   ?= 0
CPU_TYPE ?= native


#############################################################################
# CONFIGURATION
#############################################################################

ISO_DATE := $(shell date "+%Y-%m-%d")
CM_FLAGS := -Iinclude
RL_FLAGS := -DNDEBUG -O3 -march=$(CPU_TYPE)
DB_FLAGS := -g
PD_FLAGS := --from markdown --to html5 --toc -N --standalone

ifeq ($(PLUSPLUS),1)
  CXX := g++
  SRC := $(wildcard src/*.cpp)
else
  CXX := gcc
  SRC := $(wildcard src/*.c)
endif

ifdef ARCH
  CM_FLAGS += $(if $(filter x64,$(ARCH)),-m64,-m32)
else
  ARCH := $(if $(findstring x86_64,$(shell $(CXX) -v 2>&1 | grep "Target:")),x64,x86)
endif

ifeq ($(STATIC),1)
  CM_FLAGS += -static
endif

ifeq ($(OS),Windows_NT)
  OSTYPE := mingw
  SUFFIX := .exe
else
  OSTYPE := linux
  SUFFIX :=
endif

DOC := README.html COPYING.txt
BIN := bin/mhash_384.$(ARCH)$(SUFFIX)
DBG := $(BIN).dbg
OUT := out/mhash_384.$(ISO_DATE).$(OSTYPE)-$(ARCH).tar.gz


#############################################################################
# MAKE RULES
#############################################################################

.PHONY: all clean

all: $(OUT)

$(OUT): $(BIN) $(DBG) $(DOC)
	mkdir -p $(dir $@)
	rm -fv $@
	tar -czf $@ $(DOC) -C $(dir $(BIN)) $(notdir $(BIN))

$(BIN): $(SRC)
	mkdir -p $(dir $@)
	$(CXX) $(CM_FLAGS) $(RL_FLAGS) -o $@ $^
	strip -s $@

$(DBG): $(SRC)
	mkdir -p $(dir $@)
	$(CXX) $(CM_FLAGS) $(DB_FLAGS) -o $@ $^

%.html: %.md
	pandoc $(PD_FLAGS) --output $@ $^

clean:
	rm -fv $(BIN) $(DBG) $(word 1,$(DOC)) $(OUT)
