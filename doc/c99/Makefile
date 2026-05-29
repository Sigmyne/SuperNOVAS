# Part of SuperNOVAS
#
# Generates headless README.md and HTML documentation 
#
# Author: Attila Kovacs

# Check if there is a doxygen we can run
ifndef DOXYGEN
  DOXYGEN := $(shell which doxygen)
else
  $(shell test -f $(DOXYGEN))
endif

.PHONY: default
default: $(TARGET)

# Doxygen documentation (HTML and man pages) under docs/
.PHONY: all
all: Doxyfile ../../src/c99 ../../include
	@echo "   [doxygen]"
	@$(DOXYGEN) Doxyfile

.PHONY: clean
clean:

.PHONY: distclean
distclean: clean
	rm -rf html *.tag



