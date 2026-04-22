# Part of SuperNOVAS
#
# Generates headless README.md and HTML documentation 
#
# Author: Attila Kovacs

CC ?= gcc

# Doxygen documentation (HTML and man pages)
.PHONY: all
all: README.md
	@$(MAKE) -C c99 all
	@$(MAKE) -C cpp all

.INTERMEDIATE: docedit
docedit: src/docedit.o
	$(CC) -o $@ $^

# Generate headless and undecorated README variants
README.md README-undecorated.md: ../README.md docedit
	@echo "   [README variants]"
	@./docedit

.PHONY: clean
clean:
	@rm -f docedit src/docedit.o
	@$(MAKE) -C c99 clean
	@$(MAKE) -C cpp clean

.PHONY: distclean
distclean: clean
	@rm -rf README*.md
	@$(MAKE) -C c99 distclean
	@$(MAKE) -C cpp distclean

.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  all           (default) Build HTML documentation."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo
