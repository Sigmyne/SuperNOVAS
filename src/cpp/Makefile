# Part of SuperNOVAS
#
# Build supernovas++ shared of static C++ libraries
#
# Author: Attila Kovacs

CC := $(CXX)
OBJ := obj
LANGUAGE := c++
CHECK_DIR := .

LIB ?= ../../lib
NAME = supernovas++

ifeq ($(CXX),clang++)
  # Treat .c sources as C++, and use C++11
  CXXFLAGS += -x c++ -std=c++11
endif

# Use the definitions project definitions
include ../../config.mk

CPP_SOURCES := $(wildcard *.cpp)
CPP_OBJECTS := $(addprefix $(OBJ)/, $(subst .cpp,.o,$(CPP_SOURCES)))

CPPFLAGS += -I../../include

ifdef CXXFLAGS
  CFLAGS := $(CXXFLAGS)
else
  CXXFLAGS := $(CFLAGS)
endif

LDFLAGS += -lm -L$(LIB) -lsupernovas

.PHONY: shared
shared: $(LIB)/lib$(NAME).$(SOEXT)

.PHONY: static
static: $(LIB)/lib$(NAME).a

.PHONY: all
all: shared static

$(LIB)/lib$(NAME).$(SOEXT): $(LIB)/lib$(NAME).$(SOEXT).$(SO_VERSION)

$(LIB)/lib$(NAME).$(SOEXT).$(SO_VERSION): $(CPP_SOURCES) | $(LIB)/libsupernovas.$(SOEXT)

$(LIB)/lib$(NAME).a: $(CPP_OBJECTS) | $(LIB)/libsupernovas.a

$(OBJ)/%.o: %.cpp $(OBJ) Makefile
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

$(LIB)/libsupernovas.a:
	$(MAKE) -C ../c99 static

$(LIB)/libsupernovas.$(SOEXT):
	$(MAKE) -C ../c99 shared

.PHONY: clean
clean:
	@rm -f *.o $(OBJ)/*.o*

.PHONY: distclean
distclean: clean
	@rm -rf $(LIB)/lib$(NAME).*


# Built-in help screen for `make help`
.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  shared        (default) builds lib$(NAME) shared library."
	@echo "  static        builds lib$(NAME).a static libary."
	@echo "  all           all of the above."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo

# This Makefile depends on the config and build snipplets.
Makefile: ../../config.mk ../../build.mk

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include ../../build.mk
