SHELL = /bin/bash

#############################################################################
# BUILD OPTIONS
#############################################################################

CPLUSPLUS ?= 0
CPU_TYPE  ?= native
NO_JAVA   ?= 0
NO_PYTHON ?= 0


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
  PYDEXT := .pyd
  DLLOPT := -shared
else
  OSTYPE := linux
  JNIDIR := linux
  BINEXT := .run
  DLLEXT := .so
  PYDEXT := .so
  DLLOPT := -fPIC -shared
endif

ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
WORK_DIR := /tmp/$(shell head /dev/urandom | tr -dc A-Za-z0-9 | head -c 12)
ISO_DATE := $(shell date "+%Y-%m-%d")

CM_FLAGS := -I$(ROOT_DIR)/include
RL_FLAGS := -DNDEBUG -O3 -march=$(CPU_TYPE)
DB_FLAGS := -g
EX_FLAGS := -static
SO_FLAGS := $(DLLOPT) -static-libgcc -static-libstdc++
PD_FLAGS := --from markdown --to html5 --toc -N --standalone


#############################################################################
# CHECKS
#############################################################################

ifneq ($(NO_JAVA),1)
  ifneq ($(notdir $(realpath $(JAVA_HOME)/include/jni.h)),jni.h)
    $(error $$(JAVA_HOME)/include/jni.h not found! Please make sure JAVA_HOME is set correctly)
  endif
endif

ifneq ($(NO_PYTHON),1)
  ifneq ($(notdir $(realpath $(PYTHON_INC)/Python.h)),Python.h)
    $(error $$(PYTHON_INC)/Python.h not found! Please make sure PYTHON_INC is set correctly)
  endif
endif


#############################################################################
# FILE NAMES
#############################################################################

TXT := $(ROOT_DIR)COPYING.txt
DOC := $(ROOT_DIR)README.html 

ifeq ($(CPLUSPLUS),1)
  CLI_CXX := g++
  CLI_SRC := $(wildcard $(ROOT_DIR)/src/*.cpp)
else
  CLI_CXX := gcc
  CLI_SRC := $(wildcard $(ROOT_DIR)/src/*.c)
endif

CLI_BIN := $(ROOT_DIR)bin/mhash_384.$(ARCH)$(BINEXT)
CLI_DBG := $(ROOT_DIR)bin/mhash_384_g.$(ARCH)$(BINEXT)
CLI_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).bin-$(OSTYPE)-$(ARCH).tar.gz

JNI_JAR := $(ROOT_DIR)bindings/Java/library/out/MHashJava384.jar
JNI_GUI := $(ROOT_DIR)bindings/Java/example/out/MHashJava384-Example.jar
JNI_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).bin-java.tar.gz

PYC_LIB := $(ROOT_DIR)bindings/Python/library/MHashPy384.py
PYC_GUI := $(ROOT_DIR)bindings/Python/example/MHashPy384_Example.py
PYC_BIN := $(ROOT_DIR)bindings/Python/native/bin/MHashPy384_Native.$(ARCH)$(PYDEXT)
PYC_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).python-$(OSTYPE)-$(ARCH).tar.gz


#############################################################################
# TARGETS
#############################################################################

TARGETS = $(CLI_OUT)

ifneq ($(NO_JAVA),1)
  TARGETS += $(JNI_OUT)
endif

ifneq ($(NO_PYTHON),1)
  TARGETS += $(PYC_OUT)
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
	mkdir -p $(dir $@) $(WORK_DIR)
	rm -fv $@ $(WORK_DIR)/*
	cp $(DOC) $(TXT) $(CLI_BIN) $(WORK_DIR)
	pushd $(WORK_DIR) && tar -czf $@ *

$(JNI_OUT): $(JNI_JAR) $(JNI_GUI) $(DOC) $(TXT)
	mkdir -p $(dir $@) $(WORK_DIR)
	rm -fv $@ $(WORK_DIR)/*
	cp $(DOC) $(TXT) $(JNI_JAR) $(JNI_GUI) $(WORK_DIR)
	pushd $(WORK_DIR) && tar -czf $@ *

$(PYC_OUT): $(PYC_LIB) $(PYC_GUI) $(DOC) $(TXT)
	mkdir -p $(dir $@) $(WORK_DIR)
	rm -fv $@ $(WORK_DIR)/*
	cp $(DOC) $(TXT) $(PYC_LIB) $(PYC_GUI) $(WORK_DIR)
	pushd $(WORK_DIR) && tar -czf $@ *

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

$(JNI_JAR): $(abspath $(dir $(JNI_JAR))/../build.xml)
	mkdir -p $(dir $@)
	pushd $(dir $^) && ant clean jar

$(JNI_GUI): $(abspath $(dir $(JNI_GUI))/../build.xml)
	mkdir -p $(dir $@)
	pushd $(dir $^) && ant clean jar

# -----------------------------------------------
# DOCUMENTS
# -----------------------------------------------

%.html: %.md
	pandoc $(PD_FLAGS) --output $@ $^

# -----------------------------------------------
# CLEAN UP
# -----------------------------------------------

clean:
	rm -fv $(CLI_BIN) $(CLI_DBG) $(CLI_OUT)
	rm -fv $(JNI_BIN) $(JNI_JAR) $(JNI_GUI) $(JNI_OUT)
	rm -fv $(PYC_BIN) $(PYC_OUT)
	rm -fv $(DOC)

