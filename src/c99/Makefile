
# ===============================================================================
# WARNING! You should leave this Makefile alone probably
#          To configure the build, you can edit config.mk, or else you export the 
#          equivalent shell variables prior to invoking 'make' to adjust the
#          build configuration. 
# ===============================================================================

include ../../config.mk

# ===============================================================================
# Specific build targets and recipes below...
# ===============================================================================

LIB ?= ../../lib

# Our own include directory
CPPFLAGS += -I../../include

# Allow deprecated functions to be used internally
CFLAGS += -Wno-deprecated-declarations

# We'll need math functions to link
LDFLAGS += -lm

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

ifeq ($(WITHOUT_CURL), 1)
  CPPFLAGS += -DWITHOUT_CURL
else
  LDFLAGS += -lcurl
endif

# Shared libraries (versioned and unversioned)
.PHONY: shared
shared: $(SHARED_TARGETS)

# Static libraries
.PHONY: static
static: $(LIB)/libnovas.a solsys

# solarsystem() call handler objects
.PHONY: solsys
solsys: $(SOLSYS_TARGETS)

# All of the above
.PHONY: all
all: distro static analyze

# Remove intermediates
.PHONY: clean
clean:
	@rm -f $(OBJECTS)

# Remove all generated files
.PHONY: distclean
distclean: clean
	@rm -f $(LIB)/libsupernovas.$(SOEXT)* $(LIB)/libsupernovas.a $(LIB)/libnovas.a \
      $(LIB)/libnovas.$(SOEXT)* $(LIB)/libsolsys*.$(SOEXT)*

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
$(LIB)/libsolsys-calceph.$(SOEXT).$(SO_VERSION): solsys-calceph.c

# Shared library: libsolsys-cspice.so.1 (standalone solsys2.c functionality)
$(LIB)/libsolsys-cspice.$(SOEXT).$(SO_VERSION): SHLIBS := -lcspice -lpthread
$(LIB)/libsolsys-cspice.$(SOEXT).$(SO_VERSION): solsys-cspice.c

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
ifeq ($(ENABLE_CPP),1)
	$(INSTALL_DATA) include/supernovas.h $(DESTDIR)$(includedir)/
endif
	@$(MAKE) install-calceph-headers
	@$(MAKE) install-cspice-headers

.PHONY: install-docs
install-docs: install-markdown install-html install-examples install-legacy

# C++ documentation directory
cppdocdir ?= $(docdir)++
cpphtmldir ?= $(cppdocdir)/html

.PHONY: install-markdown
install-markdown:
	@echo "installing C99 markdown documentation to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) doc/*.md $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) CHANGELOG.md $(DESTDIR)$(docdir)/
	$(INSTALL_DATA) CONTRIBUTING.md $(DESTDIR)$(docdir)/
ifeq ($(ENABLE_CPP),1)
	@echo "installing C++ markdown documentation to $(DESTDIR)$(cppdocdir)"
	install -d $(DESTDIR)$(cppdocdir)
	$(INSTALL_DATA) doc/cpp/*.md $(DESTDIR)$(cppdocdir)/
endif

install-html:
ifneq ($(wildcard doc/html/search/*),)
	@echo "installing C99 API documentation to $(DESTDIR)$(htmldir)"
	install -d $(DESTDIR)$(htmldir)/search
	$(INSTALL_DATA) doc/html/search/* $(DESTDIR)$(htmldir)/search/
	$(INSTALL_DATA) doc/html/*.* $(DESTDIR)$(htmldir)/
	@echo "installing Doxygen tag file to $(DESTDIR)$(docdir)"
	install -d $(DESTDIR)$(docdir)
	$(INSTALL_DATA) doc/*.tag $(DESTDIR)$(docdir)/
else
	@echo "WARNING! Skipping HTML docs install: needs doxygen and 'local-dox'"
endif
ifneq ($(wildcard doc/cpp/html/search/*),)
	@echo "installing C++ API documentation to $(DESTDIR)$(cpphtmldir)"
	install -d $(DESTDIR)$(cpphtmldir)/search
	$(INSTALL_DATA) doc/cpp/html/search/* $(DESTDIR)$(cpphtmldir)/search/
	$(INSTALL_DATA) doc/cpp/html/*.* $(DESTDIR)$(cpphtmldir)/
	@echo "installing C++ Doxygen tag file to $(DESTDIR)$(cppdocdir)"
	install -d $(DESTDIR)$(cppdocdir)
	$(INSTALL_DATA) doc/cpp/*.tag $(DESTDIR)$(cppdocdir)/
else
	@echo "WARNING! Skipping HTML docs install: needs doxygen and 'local-dox'"
endif


.PHONY: install-examples
install-examples:
	@echo "installing C99 examples to $(DESTDIR)$(docdir)/examples"
	install -d $(DESTDIR)$(docdir)/examples
	$(INSTALL_DATA) examples/* $(DESTDIR)$(docdir)/examples/
ifeq ($(ENABLE_CPP),1)
	@echo "installing C++ examples to $(DESTDIR)$(cppdocdir)/examples"
	install -d $(DESTDIR)$(cppdocdir)/examples
	$(INSTALL_DATA) doc/examples/cpp/* $(DESTDIR)$(cppdocdir)/
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


# Built-in help screen for `make help`
.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  distro        (default) 'shared' targets and also 'local-dox' provided 'doxygen'" 
	@echo "                is available, or was specified via the DOXYGEN variable (e.g. in"
	@echo "                'config.mk')"
	@echo "  static        Builds the static 'lib/libsupernovas.a' library."
	@echo "  shared        Builds the shared 'libsupernovas.so', 'libsolsys1.so', and" 
	@echo "                'libsolsys2.so' libraries (linked to versioned ones)."
	@echo "  solsys        Builds only the objects that may provide external 'solarsystem()'"
	@echo "                call implementations (e.g. 'solsys1.o', 'eph_manager.o'...)."
	@echo "  all           All of the above."
	@echo "  install       Install components (e.g. 'make prefix=<path> install')"
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo

# This Makefile depends on the config and build snipplets.
Makefile: ../../config.mk ../../build.mk

# ===============================================================================
# Generic targets and recipes below...
# ===============================================================================

include ../../build.mk
