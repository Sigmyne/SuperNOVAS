
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

# Default target for packaging with Linux distributions
.PHONY: distro
distro: shared $(DOC_TARGETS)

# Shared or static libraries (versioned and unversioned)
.PHONY: shared static analyze
shared static: summary
	$(MAKE) -C src $@

.PHONY: shared-c99
shared-c99: summary
	$(MAKE) -C src/c99 shared

.PHONY: shared-cpp
shared-cpp: summary
	$(MAKE) -C src/cpp shared

.PHONY: static-c99
static-c99: summary
	$(MAKE) -C src/c99 static

.PHONY: static-cpp
static-cpp: summary
	$(MAKE) -C src/cpp static

.PHONY: solsys
solsys:
	$(MAKE) -C src/c99 solsys

# Static analysis
.PHONY: analyze
analyze:
	$(MAKE) -C src $@

# All of the above
.PHONY: all
all: distro static test coverage analyze

# Run regression tests
.PHONY: test
test:
	$(MAKE) -C test run

.PHONY: benchmark
benchmark: shared-c99
	$(MAKE) -C benchmark

.PHONY: examples
examples: shared
	$(MAKE) -C examples

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
	@$(MAKE) -s -C src clean
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
	@$(MAKE) -s -C src distclean
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
# Install targets and recipes
# ----------------------------------------------------------------------------

LIB ?= lib

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
ifeq ($(ENABLE_CPP),1)
	$(INSTALL_DATA) include/supernovas.h $(DESTDIR)$(includedir)/
endif
	@$(MAKE) install-calceph-headers
	@$(MAKE) install-cspice-headers

.PHONY: install-docs
install-docs: install-markdown install-html install-examples install-legacy

.PHONY: install-markdown
install-markdown:
	@echo "installing Markdown documentation to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) CHANGELOG.md $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) CONTRIBUTING.md $(DESTDIR)$(docdir)/

.PHONY: install-html
install-html:
ifneq ($(wildcard doc/c99/html/search/*),)
	@echo "installing C99 API documentation to $(DESTDIR)$(htmldir)/c99"
	install -d $(DESTDIR)$(htmldir)/c99/search
	$(INSTALL_DATA) doc/c99/html/search/* $(DESTDIR)$(htmldir)/c99/search/
	$(INSTALL_DATA) doc/c99/html/*.* $(DESTDIR)$(htmldir)/c99/
	@echo "installing C99 Doxygen tag file to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) doc/supernovas.tag $(DESTDIR)$(docdir)/
else
	@echo "WARNING! Skipping C99 HTML docs install: needs doxygen and 'local-dox'"
endif
ifneq ($(wildcard doc/cpp/html/search/*),)
	@echo "installing C++ API documentation to $(DESTDIR)$(htmldir)/cpp"
	install -d $(DESTDIR)$(htmldir)/cpp/search
	$(INSTALL_DATA) doc/cpp/html/search/* $(DESTDIR)$(htmldir)/cpp/search/
	$(INSTALL_DATA) doc/cpp/html/*.* $(DESTDIR)$(htmldir)/cpp/
	@echo "installing C++ Doxygen tag file to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) doc/cpp/supernovas++.tag $(DESTDIR)$(docdir)/
else
	@echo "WARNING! Skipping C++ HTML docs install: needs doxygen and 'local-dox'"
endif

.PHONY: install-examples
install-examples:
	@echo "installing C99 examples to $(DESTDIR)$(docdir)/examples/c99"
	install -d $(DESTDIR)$(docdir)/examples/c99
	$(INSTALL_DATA) examples/c99/* $(DESTDIR)$(docdir)/examples/c99/
ifeq ($(ENABLE_CPP),1)
	@echo "installing C++ examples to $(DESTDIR)$(docdir)/examples/cpp"
	install -d $(DESTDIR)$(docdir)/examples/cpp
	$(INSTALL_DATA) examples/cpp/* $(DESTDIR)$(docdir)/examples/cpp/
endif

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


# ----------------------------------------------------------------------------
# Help and info targets
# ----------------------------------------------------------------------------

# Build configuration summary
.PHONY: summary
summary:
	@echo
	@echo "SuperNOVAS build configuration:"
	@echo
	@echo "    CALCEPH_SUPPORT      = $(CALCEPH_SUPPORT)"
	@echo "    CSPICE_SUPPORT       = $(CSPICE_SUPPORT)"
	@echo "    ENABLE_CPP           = $(ENABLE_CPP)"
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
	@echo "  static        Builds the static 'lib/libsupernovas[++].a' libraries."
	@echo "  shared        Builds the shared 'libsupernovas[++].so', 'libsolsys1.so', and" 
	@echo "                'libsolsys2.so' libraries (linked to versioned ones)."
	@echo "  local-dox     Compiles local HTML API documentation using 'doxygen'."
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
