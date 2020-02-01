# -----------------------------------------------
# OPTIONS
# -----------------------------------------------

DEBUG ?= 0

# -----------------------------------------------
# SYSTEM DETECTION
# -----------------------------------------------

ISODATE := $(shell date +%Y-%m-%d)
OS_ARCH := $(shell $(CXX) -v 2>&1 | grep -Po 'Target:\s*\K\w+')
OS_TYPE := $(shell uname -s | tr '[:upper:]' '[:lower:]')

# -----------------------------------------------
# DIRECTORIES
# -----------------------------------------------

SUBDIRS = libmhash384 frontend
CLEANUP = $(addsuffix .cleanup,$(SUBDIRS))

# -----------------------------------------------
# FILE NAMES
# -----------------------------------------------

OUTDIR = out
BINDIR = frontend/bin

ifeq ($(DEBUG),0)
  APPNAME = mhash384
else
  APPNAME = mhash384g
endif

ifeq ($(words $(filter mingw% cygwin%,$(OS_TYPE))),0)
  SUFFIX = run
else
  SUFFIX = exe
endif

EXEFILE = $(APPNAME).$(SUFFIX)
TARFILE = $(OUTDIR)/$(APPNAME).$(ISODATE).$(firstword $(subst _, ,$(OS_TYPE)))-$(OS_ARCH).tgz

# -----------------------------------------------
# MAKE RULES
# -----------------------------------------------

.PHONY: all clean $(SUBDIRS) $(CLEANUP)

all: $(TARFILE)

clean: $(CLEANUP)

$(TARFILE): $(SUBDIRS)
	@printf "\033[1;36m===[Make package]===\033[0m\n" $@
	@mkdir -p $(dir $@)
	rm -f $@
	tar -czvf $@ COPYING.txt -C $(BINDIR) $(EXEFILE) BUILD_TAG.txt
	@printf "\033[1;32mCompleted.\033[0m\n"

$(SUBDIRS):
	@printf "\033[1;36m===[Make %s]===\033[0m\n" $@
	$(MAKE) -B -C $@
	@printf "\033[1;32mCompleted.\033[0m\n"
	
$(CLEANUP):
	@printf "\033[1;31m===[Clean %s]===\033[0m\n" $(basename $@)
	$(MAKE) -B -C $(basename $@) clean
	@printf "\033[1;32mExtinguished.\033[0m\n"
