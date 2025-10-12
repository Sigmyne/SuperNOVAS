# ===========================================================================
# Generic configuration options for building the SuperNOVAS libraries (both 
# static and shared).
#
# You can include this snipplet in your Makefile also.
# ============================================================================

# Folders for compiled objects, libraries, and binaries, respectively 
OBJ ?= obj
LIB ?= lib
BIN ?= bin

# Default compiler to use (if not defined externally)
CC ?= gcc

# Default compiler options (if not defined externally)
CFLAGS ?= -g -Os -Wall


# Specific Doxygen to use if not the default one
#DOXYGEN ?= /opt/bin/doxygen


# To make SuperNOVAS thread-safe, we use thread-local storage modifier
# keywords. These were not standardized prior to C11. So while we automatically
# recognize C11 or GCC >= 3.3 to use the correct thread-local modifier keyword,
# for other compilers (e.g. Intel C, LLVM) you may need to specify it 
# explicitly here by passing the keyword via the THREAD_LOCAL definition
#
# E.g. 
#THREAD_LOCAL ?= __thread
#or
#THREAD_LOCAL ?= __declspec( thread )


# To compile library with an external or legacy `solarsystem()` / 
# `solarsystem_hp()` implementation as the default planet provider, specify 
# the source(s), which provide the implementation. (E.g. `legacy/solsys1.c 
# legacy/eph_manager.c` or 'src/solsys-calceph.c`). If not set, then
# `solarsystem()` / `solatsystem_hp()` will be provided by the currently
# defined `novas_planet_provider` call, such as `src/solsys3.c`.
#SOLSYS_SOURCE = legacy/solsys1.c legacy/eph_manager.c


# To compile to use some user-supplied legacy `readeph()` implementation as
# the default ephemeris data provider, specify the source that will provide
# the implementation. If not set, SuperNOVAS will not provide a legacy 
# `readeph()` implementation. 
#READEPH_SOURCE = legacy/readeph0.c


# Whether or not to build solsys-calceph libraries. You need the calceph 
# development libraries (libcalceph.so and/or libcaclceph.a) installed in
# LD_LIBRARY_PATH, and calceph.h in /usr/include or some other accessible
# location (you may also  set an appropriate -I<path> option to CPPFLAGS 
# prior to calling make).
#CALCEPH_SUPPORT = 1


# Whether or not to build solsys-cspice libraries. You need the CSPICE 
# development libraries (libcspice.so and/or libcspice.a) installed in
# LD_LIBRARY_PATH, and CSPICE header files in /usr/include/cspice or some 
# other accessible location (you may also  set an appropriate -I<path> 
# option to CPPFLAGS prior to calling make).
#CSPICE_SUPPORT = 1


# cppcheck options for 'check' target. You can add additional options by
# setting the CHECKEXTRA variable (e.g. in shell) prior to invoking 'make'.
LANGUAGE ?= c

CHECKOPTS ?= --enable=performance,warning,portability,style --language=$(LANGUAGE) \
            --error-exitcode=1 --std=c99


# Add-on ccpcheck options
CHECKOPTS += --inline-suppr $(CHECKEXTRA)


# Exhaustive checking for newer cppcheck...
#CHECKOPTS += --check-level=exhaustive


# ============================================================================
# END of user config section. 
#
# Below are some generated constants based on the one that were set above
# ============================================================================

SUPERNOVAS_BUILD := 1
export SUPERNOVAS_BUILD

# The version of the shared .so libraries
SO_VERSION := 1

# Folders in which sources and headers are located, respectively
SRC := src
INC := include

# Add include directory
CPPFLAGS += -I$(INC)

# If the THREAD_LOCAL variable was defined externally, use that definition to 
# specify the thread local keyword to use. 
ifdef THREAD_LOCAL
  CPPFLAGS += -DTHREAD_LOCAL=\"$(THREAD_LOCAL)\"
endif

# Whether to use user-provided legacy `solarsystem()` / `solarsystem_hp()` 
# functions as the  default planetary ephemeris provider.
ifdef SOLSYS_SOURCE
  SOURCES += $(SOLSYS_SOURCE)
  CPPFLAGS += -DUSER_SOLSYS=1
endif

# Whether to use a legacy `readeph()` function as the default non-planetary
# ephemeris provider.
ifdef READEPH_SOURCE
  SOURCES += $(READEPH_SOURCE)
  CPPFLAGS += -DUSER_READEPH=1
endif

# Compile for specific C standard
ifdef CSTANDARD
  CFLAGS += -std=$(CSTANDARD)
endif

# Extra warnings (not supported on all compilers)
ifeq ($(WEXTRA), 1) 
  CFLAGS += -Wextra
endif

# Add source code fortification checks
ifdef FORTIFY 
  CFLAGS += -D_FORTIFY_SOURCE=$(FORTIFY)
endif

# By default determine the build platform (OS type)
PLATFORM ?= $(shell uname -s)

# Platform-specific configurations
ifeq ($(PLATFORM),Darwin)
  # macOS specific
  SOEXT := dylib
  SHARED_FLAGS := -dynamiclib -fPIC
  SONAME_FLAG := -Wl,-install_name,@rpath/
  LIB_PATH_VAR := DYLD_LIBRARY_PATH
else
  # Linux/Unix specific
  SOEXT := so
  SHARED_FLAGS := -shared -fPIC
  SONAME_FLAG := -Wl,-soname,
  LIB_PATH_VAR := LD_LIBRARY_PATH
endif

# On Linux autodetect calceph / cspice libraties with ldconfig
ifeq ($(PLATFORM),Linux)
  AUTO_DETECT_LIBS := 1
  MISSING_SYMBOLS_OK := 1
else 
  AUTO_DETECT_LIBS := 0
  MISSING_SYMBOLS_OK := 0
endif 

ifeq ($(AUTO_DETECT_LIBS),1)
  # Use ldconfig (if available) to detect CALCEPH / CSPICE shared libs automatically
  ifndef CALCEPH_SUPPORT
    ifneq ($(shell ldconfig -p | grep libcalceph), )
      CALCEPH_SUPPORT = 1
    endif
  endif
  ifndef CSPICE_SUPPORT
    ifneq ($(shell ldconfig -p | grep libcspice), )
      CSPICE_SUPPORT = 1
    endif
  endif
endif

SOURCES = $(SRC)/target.c $(SRC)/observer.c $(SRC)/earth.c $(SRC)/equator.c $(SRC)/system.c \
          $(SRC)/transform.c $(SRC)/cio.c $(SRC)/orbital.c $(SRC)/spectral.c $(SRC)/grav.c \
          $(SRC)/nutation.c $(SRC)/timescale.c $(SRC)/frames.c $(SRC)/place.c $(SRC)/calendar.c  \
          $(SRC)/refract.c $(SRC)/naif.c $(SRC)/parse.c $(SRC)/util.c $(SRC)/planets.c \
          $(SRC)/itrf.c $(SRC)/ephemeris.c $(SRC)/solsys3.c $(SRC)/solsys-ephem.c

# Generate a list of object (obj/*.o) files from the input sources
OBJECTS := $(subst $(SRC),$(OBJ),$(SOURCES))
OBJECTS := $(subst .c,.o,$(OBJECTS))

# Default values for install locations
# See https://www.gnu.org/prep/standards/html_node/Directory-Variables.html 
prefix ?= /usr
exec_prefix ?= $(prefix)
libdir ?= $(exec_prefix)/lib
includedir ?= $(prefix)/include
datarootdir ?= $(prefix)/share
datadir ?= $(datarootdir)
mydatadir ?= $(datadir)/supernovas
docdir ?= $(datarootdir)/doc/supernovas
htmldir ?= $(docdir)/html

# Search for files in the designated locations
vpath %.h $(INCLUDE)
vpath %.c $(SRC)
vpath %.o $(OBJ)
vpath %.d dep

