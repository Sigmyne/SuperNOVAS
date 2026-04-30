# Part of SuperNOVAS
#
# Build the example programs of the SuperNOVAS library 
#
# Author: Attila Kovacs

.PHONY: all analyze 
all analyze:
	@$(MAKE) -C c99 $@
ifeq ($(ENABLE_CPP),1)
	@$(MAKE) -C cpp $@
endif

.PHONY: clean distclean
clean distclean:
	@$(MAKE) -C c99 $@
	@$(MAKE) -C cpp $@


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
