# Additional Makefile for building the C/C++ libratries of SuperNOVAS.
#
# To invoke simply sun make in the SuperNOVAS root directory
#
# Author: Attila Kovacs

distro: shared

all shared static analyze:
	$(MAKE) -C c99 $@
ifeq ($(ENABLE_CPP),1)
	$(MAKE) -C cpp $@
endif
ifeq ($(ENABLE_CUDA),1)
	$(MAKE) -C cuda $@
endif

clean distclean:
	$(MAKE) -s -C c99 $@
	$(MAKE) -s -C cpp $@
	$(MAKE) -s -C cuda $@
