# -----------------------------------------------
# OPTIONS
# -----------------------------------------------

DEBUG  ?= 0
STATIC ?= 0
MARCH  ?= native
MTUNE  ?= native

# -----------------------------------------------
# TOOLS
# -----------------------------------------------

STRIP ?= strip
WNDRS ?= windres

# -----------------------------------------------
# SYSTEM DETECTION
# -----------------------------------------------

OS_TYPE := $(shell uname -s | tr '[:upper:]' '[:lower:]')

# -----------------------------------------------
# FILES
# -----------------------------------------------

SRCDIR = src
RESDIR = res
OBJDIR = obj
BINDIR = bin
LIBDIR = ../libmhash384

SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
RESFILES = $(wildcard $(RESDIR)/*.rc)
OBJFILES = $(addprefix $(OBJDIR)/,$(patsubst %.cpp,%.o,$(notdir $(SRCFILES))))

ifeq ($(DEBUG),0)
  EXENAME = mhash384
else
  EXENAME = mhash384g
endif

ifeq ($(words $(filter mingw% cygwin%,$(OS_TYPE))),0)
  SUFFIX = run
else
  SUFFIX = exe
  OBJFILES += $(addprefix $(OBJDIR)/,$(patsubst %.rc, %.o,$(notdir $(RESFILES))))
endif

CORFILE = $(LIBDIR)/src/mhash384.cpp
RSCFILE = $(OBJDIR)/resource.h
EXEFILE = $(BINDIR)/$(EXENAME).$(SUFFIX)
TAGFILE = $(BINDIR)/BUILD_TAG.txt

# -----------------------------------------------
# FLAGS
# -----------------------------------------------

CXXFLAGS += -std=gnu++11 -I$(LIBDIR)/include
LDFLAGS += -L$(LIBDIR)/lib

ifeq ($(STATIC),1)
  LDFLAGS += -static
endif

ifneq ($(words $(filter mingw%,$(OS_TYPE))),0)
  LDFLAGS += -municode
endif

ifeq ($(DEBUG),0)
ifeq ($(SANITIZE),)
  CXXFLAGS += -O3 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -DNDEBUG
else
  CXXFLAGS += -O1 -U_FORTIFY_SOURCE -fsanitize=$(SANITIZE) -fno-omit-frame-pointer
  LDFLAGS  += -fsanitize=$(SANITIZE)
endif
  CXXFLAGS += -march=$(MARCH) -mtune=$(MTUNE)
  LDFLAGS  += -lmhash384-2
else
  CXXFLAGS += -g
  LDFLAGS  += -lmhash384g-2
endif

# -----------------------------------------------
# MAKE RULES
# -----------------------------------------------

.DELETE_ON_ERROR:

.PHONY: all clean

all: $(EXEFILE) $(TAGFILE)

$(EXEFILE): $(OBJFILES)
	@mkdir -p $(dir $@)
	$(CXX) $+ -o $@ $(LDFLAGS)
ifeq ($(DEBUG),0)
ifeq ($(SANITIZE),)
	$(STRIP) -s $@
endif
endif

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(RESDIR)/%.rc $(RSCFILE)
	@mkdir -p $(dir $@)
	$(WNDRS) -I$(OBJDIR) -o $@ $<

$(RSCFILE): $(CORFILE)
	@mkdir -p $(dir $@)
	grep -E '^#define[[:space:]]+MHASH384_VERSION' $< > $@
	date +'#define MHASH384_VERSION_BUILD %y%W%w' >> $@

$(TAGFILE): $(EXEFILE)
	@mkdir -p $(dir $@)
	touch $@
	-$< --version > $@

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(OBJDIR)/*.h
	rm -rf $(BINDIR)/*.$(SUFFIX)
