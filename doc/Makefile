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

CC ?= gcc

# Doxygen documentation (HTML and man pages) under docs/
.PHONY: all
all: Doxyfile README.md $(SRC) $(INC)
	@echo "   [doxygen]"
	@$(DOXYGEN) Doxyfile

.INTERMEDIATE: docedit
docedit: src/docedit.o
	$(CC) -o $@ $^

# Generate headless and undecorated README variants
README.md: ../README.md docedit
	@echo "   [README variants]"
	@./docedit

.PHONY: clean
clean:
	rm -f docedit *.tag

.PHONY: distclean
distclean: clean
	rm -rf README*.md html



