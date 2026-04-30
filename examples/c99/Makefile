# Part of SuperNOVAS
#
# Build exammple programs
#
# Author: Attila Kovacs

# Use the definitions project definitions
EXAMPLES = example-star example-high-z example-orbital example-time example-rise-set example-moon

LIB ?= ../../lib

CPPFLAGS += -I../../include
LDFLAGS += -L$(LIB) -lsupernovas

ifeq ($(CALCEPH_SUPPORT),1)
  EXAMPLES += example-calceph
  LDFLAGS += -lsolsys-calceph -lcalceph
endif

ifeq ($(CSPICE_SUPPORT),1)
  EXAMPLES += example-cspice
  LDFLAGS += -lsolsys-cspice -lcspice
endif

.PHONY: all
all: $(EXAMPLES)

# Static code analysis using 'cppcheck'
.PHONY: analyze
analyze:
	@echo "   [analyze]"
	@cppcheck $(CPPFLAGS) $(CHECKOPTS) .

.PHONY: clean
clean:

.PHONY: distclean
distclean:
	rm -f $(EXAMPLES)

example-%: example-%.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS)

.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  all           (default) Build all example programs."
	@echo "  analyze       Runs 'cppcheck' static analysis tool on sources."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo

