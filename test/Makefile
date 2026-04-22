# Part of SuperNOVAS
#
# Builds and runs regression tests, measures test coverage
#
# Author: Attila Kovacs

ifeq ($(ENABLE_CPP), 1)
	EXCLUDE_COV += '*/include/*' '*Test'
endif

.PHONY: run
run:
	$(MAKE) -C c99 run
ifeq ($(ENABLE_CPP), 1)
	$(MAKE) -C cpp run
endif

.PHONY: coverage
coverage:
	$(MAKE) cov

cov: lcov.info
	genhtml $< -o $@

lcov.info:
	geninfo . -o $@
	lcov --remove $@ $(EXCLUDE_COV) -o $@

# Static code analysis using 'cppcheck'
.PHONY: analyze
analyze:
	$(MAKE) -C c99 analyze

.PHONY: clean-test
clean-test:
	$(MAKE) -s -C c99 clean-test
	$(MAKE) -s -C cpp clean-test

.PHONY: clean-cov
clean-cov:
	$(MAKE) -s -C c99 clean-cov
	$(MAKE) -s -C cpp clean-cov

.PHONY: clean-data
clean-data:
	rm -rf data

.PHONY: clean
clean: clean-test clean-cov
	$(MAKE) -s -C c99 clean
	$(MAKE) -s -C cpp clean

.PHONY: distclean
distclean: clean clean-data
	$(MAKE) -s -C c99 distclean
	$(MAKE) -s -C cpp distclean
	rm -f lcov.info
	rm -rf cov

.PHONY: help
help:
	@echo
	@echo "Syntax: make [target]"
	@echo
	@echo "The following targets are available:"
	@echo
	@echo "  run           (default) Compiles and runs regression tests."
	@echo "  coverage      Extracts test coverage data."
	@echo "  analyze       Static analysis with cppcheck."
	@echo "  clean         Removes intermediate products."
	@echo "  distclean     Deletes all generated files."
	@echo


