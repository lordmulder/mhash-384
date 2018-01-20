SHELL = /bin/bash

#############################################################################
# BUILD OPTIONS
#############################################################################

CPLUSPLUS ?= 0
WITH_JAVA ?= 1

CPU_ARCH ?= native
CPU_TUNE ?= generic

HOME ?= /tmp

#############################################################################
# CONFIGURATION
#############################################################################

ifdef ARCH
  CM_FLAGS += $(if $(filter x64,$(ARCH)),-m64,-m32)
else
  ARCH := $(if $(findstring x86_64,$(shell $(CXX) -v 2>&1 | grep "Target:")),x64,x86)
endif

ifeq ($(OS),Windows_NT)
  OSTYPE := mingw
  JNIDIR := win32
  BINEXT := .exe
  DLLEXT := .dll
  DLLOPT := -shared
else
  OSTYPE := linux
  JNIDIR := linux
  BINEXT := .run
  DLLEXT := .so
  DLLOPT := -fPIC -shared
endif

ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
WORK_DIR := /tmp/$(shell head /dev/urandom | tr -dc A-Za-z0-9 | head -c 12)
JARS_DIR := $(HOME)/.jar
ISO_DATE := $(shell date "+%Y-%m-%d")

CM_FLAGS := -I$(ROOT_DIR)/include
RL_FLAGS := -DNDEBUG -O3 -march=$(CPU_ARCH) -mtune=$(CPU_TUNE)
DB_FLAGS := -g
EX_FLAGS := -static
SO_FLAGS := $(DLLOPT) -static-libgcc -static-libstdc++
PD_FLAGS := --from markdown_github+pandoc_title_block+header_attributes+implicit_figures --to html5 --toc -N --standalone -H "etc/css/style.inc"

#############################################################################
# FILE NAMES
#############################################################################

TXT := $(ROOT_DIR)COPYING.txt
DOC := $(ROOT_DIR)README.html 
JPG := $(wildcard $(ROOT_DIR)/img/mhash/*.jpg)

ifeq ($(CPLUSPLUS),1)
  CLI_CXX := g++
  CLI_SRC := $(wildcard $(ROOT_DIR)/src/*.cpp)
else
  CLI_CXX := gcc
  CLI_SRC := $(wildcard $(ROOT_DIR)/src/*.c)
endif

CLI_BIN := $(ROOT_DIR)bin/mhash_384.$(ARCH)$(BINEXT)
CLI_DBG := $(ROOT_DIR)bin/mhash_384_g.$(ARCH)$(BINEXT)
CLI_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).$(OSTYPE)-$(ARCH).tar.gz

JAV_JAR := $(ROOT_DIR)bindings/Java/library/out/MHashJava384.jar
JAV_GUI := $(ROOT_DIR)bindings/Java/example/out/MHashJava384-Example.jar
JAV_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).java.tar.gz

PYT_LIB := $(ROOT_DIR)bindings/Python/library/MHashPy384.py
PYT_GUI := $(ROOT_DIR)bindings/Python/example/MHashPy384_Example.py
PYT_BIN := $(ROOT_DIR)bindings/Python/native/bin/MHashPy384_Native.$(ARCH)$(PYDEXT)
PYT_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).python.tar.gz


#############################################################################
# TARGETS
#############################################################################

TARGETS = $(CLI_OUT) $(PYT_OUT)

ifeq ($(WITH_JAVA),1)
  TARGETS += $(JAV_OUT)
endif


#############################################################################
# MAKE RULES
#############################################################################

.PHONY: all clean

all: $(TARGETS)

# -----------------------------------------------
# PACKAGE
# -----------------------------------------------

$(CLI_OUT): $(CLI_BIN) $(CLI_DBG) $(DOC) $(TXT)
	rm -rfv $@ $(WORK_DIR)
	mkdir -p $(dir $@) $(WORK_DIR)/img/mhash
	cp $(JPG) $(WORK_DIR)/img/mhash
	cp $(DOC) $(TXT) $(CLI_BIN) $(WORK_DIR)
	pushd $(WORK_DIR) && tar -czf $@ *
	rm -rfv $(WORK_DIR)

$(JAV_OUT): $(JAV_JAR) $(JAV_GUI) $(DOC) $(TXT)
	rm -rfv $@ $(WORK_DIR)
	mkdir -p $(dir $@) $(WORK_DIR)/img/mhash
	cp $(JPG) $(WORK_DIR)/img/mhash
	cp $(DOC) $(TXT) $(JAV_JAR) $(JAV_GUI) $(WORK_DIR)
	pushd $(WORK_DIR) && tar -czf $@ *
	rm -rfv $(WORK_DIR)

$(PYT_OUT): $(PYT_LIB) $(PYT_GUI) $(DOC) $(TXT)
	rm -rfv $@ $(WORK_DIR)
	mkdir -p $(dir $@) $(WORK_DIR)/img/mhash
	cp $(JPG) $(WORK_DIR)/img/mhash
	cp $(DOC) $(TXT) $(PYT_LIB) $(PYT_GUI) $(WORK_DIR)
	pushd $(WORK_DIR) && tar -czf $@ *
	rm -rfv $(WORK_DIR)

# -----------------------------------------------
# COMPILE
# -----------------------------------------------

$(CLI_BIN): $(CLI_SRC)
	mkdir -p $(dir $@)
	$(CLI_CXX) $(CM_FLAGS) $(EX_FLAGS) $(RL_FLAGS) -o $@ $^
	strip -s $@

$(CLI_DBG): $(CLI_SRC)
	mkdir -p $(dir $@)
	$(CLI_CXX) $(CM_FLAGS) $(EX_FLAGS) $(DB_FLAGS) -o $@ $^

$(JAV_JAR): $(abspath $(dir $(JAV_JAR))/../build.xml)
	mkdir -p $(dir $@)
	pushd $(dir $^) && ant clean jar

$(JAV_GUI): $(abspath $(dir $(JAV_GUI))/../build.xml)
	mkdir -p $(dir $@)
	pushd $(dir $^) && ant clean jar

# -----------------------------------------------
# DOCUMENTS
# -----------------------------------------------

%.html: %.md
	mkdir -p $(JARS_DIR)
	wget -N -P $(JARS_DIR) https://repo1.maven.org/maven2/com/yahoo/platform/yui/yuicompressor/2.4.8/yuicompressor-2.4.8.jar
	wget -N -P $(JARS_DIR) https://repo1.maven.org/maven2/com/googlecode/htmlcompressor/htmlcompressor/1.5.2/htmlcompressor-1.5.2.jar
	pandoc $(PD_FLAGS) $^ | java -jar $(JARS_DIR)/htmlcompressor-1.5.2.jar --compress-css -o $@

# -----------------------------------------------
# CLEAN UP
# -----------------------------------------------

clean:
	rm -fv $(CLI_BIN) $(CLI_DBG) $(CLI_OUT)
	rm -fv $(JAV_BIN) $(JAV_JAR) $(JAV_GUI) $(JAV_OUT)
	rm -fv $(PYT_BIN) $(PYT_OUT)
	rm -fv $(DOC)
