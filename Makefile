# -----------------------------------------------
# OPTIONS
# -----------------------------------------------

DEBUG ?= 0
NODOC ?= 0

# -----------------------------------------------
# TOOLS
# -----------------------------------------------

TAR ?= tar
PNDOC ?= pandoc

# -----------------------------------------------
# SYSTEM DETECTION
# -----------------------------------------------

ISODATE := $(shell date +%Y-%m-%d)
OS_TYPE := $(shell $(CXX) -dumpmachine)

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

ifeq ($(words $(filter %mingw32 %windows-gnu %cygwin %cygnus,$(OS_TYPE))),0)
  SUFFIX = run
else
  SUFFIX = exe
endif

EXEFILE = $(APPNAME).$(SUFFIX)
TARFILE = $(OUTDIR)/$(APPNAME).$(ISODATE).$(OS_TYPE).tgz

ifneq ($(NODOCS),1)
DOCFILE = README.html
else
DOCFILE = README.md
endif

# -----------------------------------------------
# MAKE RULES
# -----------------------------------------------

.PHONY: all clean $(SUBDIRS) $(CLEANUP)

all: $(TARFILE)

$(TARFILE): $(SUBDIRS) $(DOCFILE)
	@printf "\033[1;36m===[Make package]===\033[0m\n"
	@mkdir -p $(dir $@)
	rm -f $@
	$(TAR) -czvf $@ COPYING.txt $(DOCFILE) img/mhash384/*.jpg -C $(BINDIR) $(EXEFILE) BUILD_TAG.txt
	@printf "\033[1;32mCompleted.\033[0m\n"

$(SUBDIRS):
	@printf "\033[1;36m===[Make %s]===\033[0m\n" $@
	$(MAKE) -C $@
	@printf "\033[1;32mCompleted.\033[0m\n"

%.html: %.md
	@printf "\033[1;36m===[Make %s]===\033[0m\n" $(basename $@)
	$(PNDOC) --from markdown_github+pandoc_title_block+header_attributes+implicit_figures+yaml_metadata_block --to html5 --toc -N --standalone -H etc/css/style.inc -o $@ $<

clean: $(CLEANUP)
	@printf "\033[1;31m===[Clean package]===\033[0m\n"
	rm -f *.html
	@printf "\033[1;32mExtinguished.\033[0m\n"

$(CLEANUP):
	@printf "\033[1;31m===[Clean %s]===\033[0m\n" $(basename $@)
	$(MAKE) -C $(basename $@) clean
	@printf "\033[1;32mExtinguished.\033[0m\n"
