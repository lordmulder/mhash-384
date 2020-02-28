# -----------------------------------------------
# OPTIONS
# -----------------------------------------------

DEBUG ?= 0
NODOC ?= 0

# -----------------------------------------------
# TOOLS
# -----------------------------------------------

TAR  ?= tar
ZIP  ?= zip
PNDC ?= pandoc

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

PKGPATH = $(OUTDIR)/$(APPNAME).$(ISODATE).$(OS_TYPE)
TARFILE = $(PKGPATH).tgz
ZIPFILE = $(PKGPATH).zip

ifeq ($(words $(filter %mingw32 %windows-gnu %cygwin %cygnus,$(OS_TYPE))),0)
  SUFFIX = run
  TARGET = $(TARFILE)
else
  SUFFIX = exe
  TARGET = $(ZIPFILE)
endif

EXEFILE = $(APPNAME).$(SUFFIX)

ifneq ($(NODOCS),1)
  DOCFILE = README.html
else
  DOCFILE = README.md
endif

# -----------------------------------------------
# MAKE RULES
# -----------------------------------------------

.PHONY: all clean $(SUBDIRS) $(CLEANUP)

all: $(TARGET)

$(TARFILE): $(SUBDIRS) $(DOCFILE)
	@printf "\033[1;36m===[Make package]===\033[0m\n"
	@mkdir -p $(dir $@)
	rm -f $@
	$(TAR) -czvf $@ COPYING.txt $(DOCFILE) img/mhash384/*.jpg -C $(BINDIR) $(EXEFILE) BUILD_TAG.txt
	@printf "\033[1;32mCompleted.\033[0m\n"

$(ZIPFILE): $(SUBDIRS) $(DOCFILE)
	@printf "\033[1;36m===[Make package]===\033[0m\n"
	@mkdir -p $(dir $@)
	rm -f $@
	$(ZIP) -j $@ COPYING.txt $(DOCFILE) $(BINDIR)/$(EXEFILE) $(BINDIR)/BUILD_TAG.txt && $(ZIP) $@ img/mhash384/*.jpg
	@printf "\033[1;32mCompleted.\033[0m\n"

$(SUBDIRS):
	@printf "\033[1;36m===[Make %s]===\033[0m\n" $@
	$(MAKE) -C $@
	@printf "\033[1;32mCompleted.\033[0m\n"

%.html: %.md
	@printf "\033[1;36m===[Make %s]===\033[0m\n" $(basename $@)
	$(PNDC) --from markdown_github+pandoc_title_block+header_attributes+implicit_figures+yaml_metadata_block --to html5 --toc -N --standalone -H etc/css/style.inc -o $@ $<
	@printf "\033[1;32mCompleted.\033[0m\n"

clean: $(CLEANUP)
	@printf "\033[1;31m===[Clean package]===\033[0m\n"
	rm -f *.html
	@printf "\033[1;32mExtinguished.\033[0m\n"

$(CLEANUP):
	@printf "\033[1;31m===[Clean %s]===\033[0m\n" $(basename $@)
	$(MAKE) -C $(basename $@) clean
	@printf "\033[1;32mExtinguished.\033[0m\n"
