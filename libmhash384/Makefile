# -----------------------------------------------
# OPTIONS
# -----------------------------------------------

DEBUG ?= 0
MARCH ?= native
MTUNE ?= native

# -----------------------------------------------
# FILES
# -----------------------------------------------

SRCDIR = src
OBJDIR = obj
LIBDIR = lib

SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
OBJFILES = $(addprefix $(OBJDIR)/,$(patsubst %.cpp,%.o,$(notdir $(SRCFILES))))

ifeq ($(DEBUG),0)
  LIBNAME = libmhash384-2
else
  LIBNAME = libmhash384g-2
endif

LIBFILE = $(LIBDIR)/$(LIBNAME).a

# -----------------------------------------------
# FLAGS
# -----------------------------------------------

CXXFLAGS += -std=gnu++11 -Iinclude

ifeq ($(DEBUG),0)
  CXXFLAGS += -O3 -DNDEBUG -march=$(MARCH) -mtune=$(MTUNE)
else
  CXXFLAGS += -g
endif

# -----------------------------------------------
# MAKE RULES
# -----------------------------------------------

.DELETE_ON_ERROR:

.PHONY: all clean

all: $(LIBFILE)

$(LIBFILE): $(OBJFILES)
	@mkdir -p $(dir $@)
	rm -f $@
	$(AR) rcs $@ $+

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(LIBDIR)/*.a
