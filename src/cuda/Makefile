# Part of SuperNOVAS
#
# Build supernovas-cuda shared of static C++ library
#
# Author: Attila Kovacs

NVCC = nvcc
CC := $(NVCC)
CFLAGS ?= -g -O2
OBJ := obj
LANGUAGE := c++
CHECK_DIR := .

LIB ?= ../../lib
NAME := supernovas-cuda

# Use the definitions project definitions
include ../../config.mk

# Flags for NVCC to create shared libs
SHARED_FLAGS := -shared -Xcompiler -fPIC
SONAME_FLAG := -Xlinker -soname,

CUDA_SOURCES := $(wildcard *.cu)
CUDA_OBJECTS := $(addprefix $(OBJ)/, $(subst .cu,.o,$(CUDA_SOURCES)))

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

$(LIB)/lib$(NAME).$(SOEXT).$(SO_VERSION): $(CUDA_SOURCES) | $(LIB)/libsupernovas.$(SOEXT)

$(LIB)/lib$(NAME).a: $(CUDA_OBJECTS) | $(LIB)/libsupernovas.a

$(OBJ)/%.o: %.cu $(OBJ) Makefile
	$(NVCC) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

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

