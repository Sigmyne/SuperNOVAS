# Part of SuperNOVAS
#
# Build supernovas++ shared of static C++ libraries
#
# Author: Attila Kovacs

CC := $(CXX)
SRC ?= ../src
LIB ?= ../lib
OBJ := obj

# Use the definitions project definitions
include ../config.mk

CPPFLAGS += -I../include
LDFLAGS += -lm

C_SOURCES := $(subst src,../src,$(SOURCES))
CPP_SOURCES := $(wildcard *.cpp)
CPP_OBJECTS := $(addprefix $(OBJ)/, $(subst .cpp,.opp,$(CPP_SOURCES)))

ifdef CXXFLAGS
  CFLAGS := $(CXXFLAGS)
else
  CXXFLAGS := $(CFLAGS)
endif

.PHONY: shared
shared: $(LIB)/libsupernovas++.$(SOEXT)

.PHONY: static
static: $(LIB)/libsupernovas++.a

.PHONY: all
all: shared static

$(LIB)/libsupernovas++.$(SOEXT): $(LIB)/libsupernovas++.$(SOEXT).$(SO_VERSION)

$(LIB)/libsupernovas++.$(SOEXT).$(SO_VERSION): $(C_SOURCES) $(CPP_SOURCES)

$(LIB)/libsupernovas++.a: $(OBJECTS) $(CPP_OBJECTS)

$(OBJ)/%.opp: %.cpp $(OBJ) Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

$(OBJ)/%.o: CPPFLAGS += -DNOVAS_NAMESPACE
$(OBJ)/%.o: ../src/%.c $(OBJ) Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

.PHONY: clean
clean:
	@rm -f *.o $(OBJ)/*.o*

.PHONY: distclean
distclean: clean
	@rm -rf $(LIB)/libsupernovas++.*

# Built-in help screen for `make help`
.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  shared        (default) builds libsupernovas++ shared library."
	@echo "  static        builds libsupernovas++.a."
	@echo "  all           all of the above."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo

# This Makefile depends on the config and build snipplets.
Makefile: ../config.mk ../build.mk

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include ../build.mk