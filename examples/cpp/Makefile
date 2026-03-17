# Part of SuperNOVAS
#
# Build C++ example programs
#
# Author: Attila Kovacs

# Use the definitions project definitions
include ../../config.mk

EXAMPLES = example-star example-high-z example-time

LIB ?= ../../lib

CPPFLAGS += -I../../include
CXXFLAGS ?= $(CPPFLAGS)
LDFLAGS += -L$(LIB) -lsupernovas -lsupernovas++

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

example-%: example-%.cpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) $< $(LDFLAGS)

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

