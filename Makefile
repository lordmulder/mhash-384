SHELL = /bin/bash

#############################################################################
# BUILD OPTIONS
#############################################################################

CPLUSPLUS ?= 0
STATIC    ?= 0
CPU_TYPE  ?= native
NO_JAVA   ?= 0
NO_PYTHON ?= 0


#############################################################################
# CONFIGURATION
#############################################################################

ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
ISO_DATE := $(shell date "+%Y-%m-%d")
CM_FLAGS := -I$(ROOT_DIR)/include
RL_FLAGS := -DNDEBUG -O3 -march=$(CPU_TYPE)
DB_FLAGS := -g
PD_FLAGS := --from markdown --to html5 --toc -N --standalone

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
  JNIDIR := win32
  SUFFIX := .exe
  DLLEXT := .dll
  DLLOPT := -shared
else
  OSTYPE := linux
  JNIDIR := linux
  SUFFIX :=
  DLLEXT := .so
  DLLOPT += -fPIC -shared
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

CLI_BIN := $(ROOT_DIR)bin/mhash_384.$(ARCH)$(SUFFIX)
CLI_DBG := $(BIN).dbg
CLI_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).bin-$(OSTYPE)-$(ARCH).tar.gz

JNI_SRC := $(wildcard $(ROOT_DIR)bindings/Java/native/src/*.cpp)
JNI_INC := $(ROOT_DIR)bindings/Java/native/include
JNI_BIN := $(ROOT_DIR)bindings/Java/native/bin/MHashJava384.$(ARCH)$(DLLEXT)
JNI_JAR := $(ROOT_DIR)bindings/Java/wrapper/out/MHashJava384-Wrapper.jar
JNI_GUI := $(ROOT_DIR)bindings/Java/example/out/MHashJava384-Example.jar
JNI_OUT := $(ROOT_DIR)out/mhash_384.$(ISO_DATE).java-$(OSTYPE)-$(ARCH).tar.gz

#############################################################################
# TARGETS
#############################################################################

TARGETS = $(CLI_OUT)

ifneq ($(NO_JAVA),1)
  TARGETS += $(JNI_OUT)
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
	mkdir -p $(dir $@)
	rm -fv $@
	tar -czf $@ -C $(dir $(DOC)) $(notdir $(DOC)) -C $(dir $(TXT)) $(notdir $(TXT)) -C $(dir $(CLI_BIN)) $(notdir $(CLI_BIN))

$(JNI_OUT): $(JNI_BIN) $(JNI_JAR) $(JNI_GUI) $(DOC) $(TXT)
	mkdir -p $(dir $@)
	rm -fv $@
	tar -czf $@ -C $(dir $(DOC)) $(notdir $(DOC)) -C $(dir $(TXT)) $(notdir $(TXT)) -C $(dir $(JNI_BIN)) $(notdir $(JNI_BIN)) -C $(dir $(JNI_JAR)) $(notdir $(JNI_JAR)) -C $(dir $(JNI_GUI)) $(notdir $(JNI_GUI))

# -----------------------------------------------
# COMPILE
# -----------------------------------------------

$(CLI_BIN): $(CLI_SRC)
	mkdir -p $(dir $@)
	$(CLI_CXX) $(CM_FLAGS) $(RL_FLAGS) -o $@ $^
	strip -s $@

$(CLI_DBG): $(CLI_SRC)
	mkdir -p $(dir $@)
	$(CLI_CXX) $(CM_FLAGS) $(DB_FLAGS) -o $@ $^

$(JNI_BIN): $(JNI_SRC)
	mkdir -p $(dir $@)
	g++ $(CM_FLAGS) $(RL_FLAGS) $(DLLOPT) -I$(JNI_INC) -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(JNIDIR) -o $@ $^
	strip -s $@

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
	rm -fv $(DOC)
