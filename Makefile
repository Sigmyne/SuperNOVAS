
# ===============================================================================
# WARNING! You should leave this Makefile alone probably
#          To configure the build, you can edit config.mk, or else you export the 
#          equivalent shell variables prior to invoking 'make' to adjust the
#          build configuration. 
# ===============================================================================

include config.mk

# ===============================================================================
# Specific build targets and recipes below...
# ===============================================================================

# We'll need math functions to link
LDFLAGS += -lm

# If there is doxygen, build the API documentation also by default
ifeq ($(.SHELLSTATUS),0)
  DOC_TARGETS += local-dox
else
  ifneq ($(DOXYGEN),none)
    $(info WARNING! Doxygen is not available. Will skip 'dox' target)
  endif
endif

SOLSYS_TARGETS :=
SHARED_TARGETS := $(LIB)/libsupernovas.$(SOEXT) $(LIB)/libnovas.$(SOEXT)

ifeq ($(CALCEPH_SUPPORT), 1)
  CPPFLAGS += -DUSE_CALCEPH=1
  SOLSYS_TARGETS += $(OBJ)/solsys-calceph.o
  SHARED_TARGETS += $(LIB)/libsolsys-calceph.$(SOEXT)
endif

ifeq ($(CSPICE_SUPPORT), 1)
  CPPFLAGS += -DUSE_CSPICE=1
  SOLSYS_TARGETS += $(OBJ)/solsys-cspice.o
  SHARED_TARGETS += $(LIB)/libsolsys-cspice.$(SOEXT)
endif

# Default target for packaging with Linux distributions
.PHONY: distro
distro: $(SHARED_TARGETS) $(DOC_TARGETS)

# Shared libraries (versioned and unversioned)
.PHONY: shared
shared: summary $(SHARED_TARGETS)
ifeq ($(ENABLE_CPP_LIBS), 1)
	make -C cpp shared
endif

# Static libraries
.PHONY: static
static: summary $(LIB)/libnovas.a solsys
ifeq ($(ENABLE_CPP_LIBS), 1)
	make -C cpp static
endif

# solarsystem() call handler objects
.PHONY: solsys
solsys: $(SOLSYS_TARGETS)

# All of the above
.PHONY: all
all: distro static test coverage analyze

# Run regression tests
.PHONY: test
test:
	$(MAKE) -C test run

.PHONY: benchmark
benchmark: shared
	$(MAKE) -C benchmark

.PHONY: examples
examples: shared
	$(MAKE) -C examples

# C++ static analysis
.PHONY: analyze-cpp
analyze-cpp: 
	$(MAKE) -C cpp analyze

# Perform checks (test + analyze)
.PHONY: check
check: test analyze

# Measure test coverage (on test set of files only)
.PHONY: coverage
coverage:
	$(MAKE) -C test coverage

# Remove intermediates
.PHONY: clean
clean:
	@rm -f $(OBJECTS) Doxyfile.local gmon.out
	@$(MAKE) -s -C cpp clean	
	@$(MAKE) -s -C test clean
	@$(MAKE) -s -C benchmark clean
	@$(MAKE) -s -C examples clean
	@$(MAKE) -s -C doc clean

# Remove all generated files
.PHONY: distclean
distclean: clean
	@rm -f $(LIB)/libsupernovas.$(SOEXT)* $(LIB)/libsupernovas.a $(LIB)/libnovas.a \
      $(LIB)/libnovas.$(SOEXT)* $(LIB)/libsolsys*.$(SOEXT)*
	@rm -f doc/Doxyfile.local doc/README.md
	@rm -rf build */build 
	@$(MAKE) -s -C cpp distclean
	@$(MAKE) -s -C test distclean
	@$(MAKE) -s -C benchmark distclean
	@$(MAKE) -s -C examples distclean
	@$(MAKE) -s -C doc distclean

.PHONY: dox
dox:
	DOXYGEN_HTML_HEADER="resources/header.html" $(MAKE) -C doc

.PHONY: local-dox
local-dox:
	$(MAKE) -C doc


# ----------------------------------------------------------------------------
# The nitty-gritty stuff below
# ----------------------------------------------------------------------------

$(LIB)/libsupernovas.$(SOEXT): $(LIB)/libsupernovas.$(SOEXT).$(SO_VERSION)

$(LIB)/libsolsys-calceph.$(SOEXT): $(LIB)/libsolsys-calceph.$(SOEXT).$(SO_VERSION)

$(LIB)/libsolsys-cspice.$(SOEXT): $(LIB)/libsolsys-cspice.$(SOEXT).$(SO_VERSION)

$(LIB)/libnovas.$(SOEXT): $(LIB)/libsupernovas.$(SOEXT)

# Shared library: libsupernovas.so.1 -- same as novas.so except the builtin SONAME
$(LIB)/libsupernovas.$(SOEXT).$(SO_VERSION): $(SOURCES)

# Shared library: libsolsys-calceph.so.1 (standalone solsys2.c functionality)
$(LIB)/libsolsys-calceph.$(SOEXT).$(SO_VERSION): SHLIBS := -lcalceph -lpthread
$(LIB)/libsolsys-calceph.$(SOEXT).$(SO_VERSION): $(SRC)/solsys-calceph.c

# Shared library: libsolsys-cspice.so.1 (standalone solsys2.c functionality)
$(LIB)/libsolsys-cspice.$(SOEXT).$(SO_VERSION): SHLIBS := -lcspice -lpthread
$(LIB)/libsolsys-cspice.$(SOEXT).$(SO_VERSION): $(SRC)/solsys-cspice.c

# Link submodules against the supernovas shared lib
$(LIB)/libsolsys%.$(SOEXT).$(SO_VERSION): | $(LIB) $(LIB)/libsupernovas.$(SOEXT).$(SO_VERSION)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ $(SHARED_FLAGS) $(SONAME_FLAG)$(notdir $@) -L$(LIB) -lsupernovas $(SHLIBS) $(LDFLAGS)

# Static library: libsupernovas.a
$(LIB)/libsupernovas.a: $(OBJECTS) | $(LIB) Makefile

$(LIB)/libnovas.a: $(LIB)/libsupernovas.a
	@rm -f $@
	( cd $(LIB); ln -s libsupernovas.a libnovas.a )

# Standard install commands
INSTALL_PROGRAM ?= install
INSTALL_DATA ?= install -m 644

.PHONY: install
install: install-libs install-headers install-docs install-legacy

.PHONY: install-libs
install-libs:
ifneq ($(wildcard $(LIB)/*),)
	@echo "installing libraries to $(DESTDIR)$(libdir)"
	install -d $(DESTDIR)$(libdir)
	cp -a $(LIB)/* $(DESTDIR)$(libdir)/
else
	@echo "WARNING! Skipping libs install: needs 'shared' and/or 'static'"
endif

.PHONY: install-calceph-headers
install-calceph-headers:
ifeq ($(CALCEPH_SUPPORT),1)
	$(INSTALL_DATA) include/novas-calceph.h $(DESTDIR)$(includedir)/
endif

.PHONY: install-cspice-headers
install-cspice-headers:
ifeq ($(CSPICE_SUPPORT),1) 
	$(INSTALL_DATA) include/novas-cspice.h $(DESTDIR)$(includedir)/
endif

.PHONY: install-headers
install-headers:
	@echo "installing headers to $(DESTDIR)$(includedir)"
	install -d $(DESTDIR)$(includedir)
	$(INSTALL_DATA) include/novas.h $(DESTDIR)$(includedir)/
	@$(MAKE) install-calceph-headers
	@$(MAKE) install-cspice-headers

.PHONY: install-docs
install-docs: install-markdown install-html install-examples install-legacy

.PHONY: install-markdown
install-markdown:
	@echo "installing documentation to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) doc/*.md $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) CHANGELOG.md $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) CONTRIBUTING.md $(DESTDIR)$(docdir)/

.PHONY: install-html
install-html:
ifneq ($(wildcard doc/html/search/*),)
	@echo "installing API documentation to $(DESTDIR)$(htmldir)"
	install -d $(DESTDIR)$(htmldir)/search
	$(INSTALL_DATA) doc/html/search/* $(DESTDIR)$(htmldir)/search/
	$(INSTALL_DATA) doc/html/*.* $(DESTDIR)$(htmldir)/
	@echo "installing Doxygen tag file to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) doc/supernovas.tag $(DESTDIR)$(docdir)/supernovas.tag
else
	@echo "WARNING! Skipping HTML docs install: needs doxygen and 'local-dox'"
endif

.PHONY: install-examples
install-examples:
	@echo "installing examples to $(DESTDIR)$(docdir)/examples"
	install -d $(DESTDIR)$(docdir)/examples
	$(INSTALL_DATA) examples/* $(DESTDIR)$(docdir)/examples/

.PHONY: install-legacy
install-legacy:
	@echo "installing legacy files to $(DESTDIR)$(docdir)/legacy"
	install -d $(DESTDIR)$(docdir)/legacy
	$(INSTALL_DATA) legacy/* $(DESTDIR)$(docdir)/legacy/

# Some standard GNU targets, that should always exist...
.PHONY: html
html: local-dox

.PHONY: dvi
dvi:

.PHONY: ps
ps:

.PHONY: pdf
pdf:

# Build configuration summary
.PHONY: summary
summary:
	@echo
	@echo "SuperNOVAS build configuration:"
	@echo
	@echo "    CALCEPH_SUPPORT      = $(CALCEPH_SUPPORT)"
	@echo "    CSPICE_SUPPORT       = $(CSPICE_SUPPORT)"
	@echo "    SOLSYS_SOURCES       = $(SOLSYS_SOURCES)"
	@echo "    READEPH_SOURCES      = $(READEPH_SOURCES)"
	@echo
	@echo "    CC      = $(CC)" 
	@echo "    CFLAGS  = $(CFLAGS)"
	@echo "    LDFLAGS = $(LDFLAGS)"
	@echo

# Built-in help screen for `make help`
.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following tairgets are available:"
	@echo
	@echo "  distro        (default) 'shared' targets and also 'local-dox' provided 'doxygen'" 
	@echo "                is available, or was specified via the DOXYGEN variable (e.g. in"
	@echo "                'config.mk')"
	@echo "  static        Builds the static 'lib/libsupernovas.a' library."
	@echo "  shared        Builds the shared 'libsupernovas.so', 'libsolsys1.so', and" 
	@echo "                'libsolsys2.so' libraries (linked to versioned ones)."
	@echo "  local-dox     Compiles local HTML API documentation using 'doxygen'."
	@echo "  solsys        Builds only the objects that may provide external 'solarsystem()'"
	@echo "                call implentations (e.g. 'solsys1.o', 'eph_manager.o'...)."
	@echo "  test          Runs regression tests."
	@echo "  benchmark     Runs benchmarks."
	@echo "  analyze       Performs static code analysis with 'cppcheck'."
	@echo "  check         Same as 'test' and then 'analyze'."
	@echo "  coverage      Runs 'gcov' to analyze regression test coverage."
	@echo "  summary       Provides a summary of the current build configuration."
	@echo "  all           All of the above."
	@echo "  install       Install components (e.g. 'make prefix=<path> install')"
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo

# This Makefile depends on the config and build snipplets.
Makefile: config.mk build.mk

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include build.mk
