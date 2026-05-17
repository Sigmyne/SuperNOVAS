# Additional Makefile for building the C/C++ libratries of SuperNOVAS.
#
# To invoke simply sun make in the SuperNOVAS root directory
#
# Author: Attila Kovacs

.PHONY: distro
distro: shared

.PHONY: shared static analyze
all shared static analyze:
	$(MAKE) -C c99 $@
ifeq ($(ENABLE_CPP),1)
	$(MAKE) -C cpp $@
endif

.PHONY: clean distclean
clean distclean:
	$(MAKE) -s -C c99 $@
	$(MAKE) -s -C cpp $@
