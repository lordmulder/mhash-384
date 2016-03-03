PLUSPLUS ?= 0
CPU_TYPE ?= native

ISO_DATE := $(shell date "+%Y-%m-%d")

ifeq ($(PLUSPLUS),1)
  CXX := g++
  SRC := main++.cpp
else
  CXX := gcc
  SRC := main.c
endif

ifeq ($(OS),Windows_NT)
  OSTYPE := mingw
  SUFFIX := .exe
else
  OSTYPE := linux
endif

TRGT := $(shell $(CXX) -v 2>&1 | grep -oP "Target:\s+\K(\w+)")
ifeq ($(TRGT),x86_64)
  ARCH := x64
else
  ARCH := x86
endif

.PHONY: all mhash_release mhash_debug mhash_docs clean

all: mhash_release mhash_debug mhash_docs
	mkdir -p out
	rm -f out/mhash_384.$(ISO_DATE).$(OSTYPE)-$(ARCH).tar.gz
	tar -czf out/mhash_384.$(ISO_DATE).$(OSTYPE)-$(ARCH).tar.gz COPYING.txt README.html -C bin mhash_384.$(ARCH)$(SUFFIX)

mhash_release:
	mkdir -p bin
	$(CXX) -Iinclude -O3 -march=$(CPU_TYPE) -static -DNDEBUG -o bin/mhash_384.$(ARCH)$(SUFFIX) src/$(SRC)
	strip bin/mhash_384.$(ARCH)$(SUFFIX)

mhash_debug:
	mkdir -p bin
	$(CXX) -Iinclude -g -o bin/mhash_384_g.$(ARCH)$(SUFFIX) src/$(SRC)

mhash_docs:
	pandoc --from markdown --to html5 --toc -N --standalone --output README.html README.md

clean:
	rm -v bin/mhash_384*

