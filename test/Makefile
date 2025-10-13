# Part of SuperNOVAS
#
# Builds and runs regression tests, measures test coverage
#
# Author: Attila Kovacs

include ../config.mk

# Flags for measuring test coverage
CFLAGS += -I../include -I../legacy -pg -Wall -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lm

# cppcheck options for 'check' target. You can add additional options by
# setting the CHECKEXTRA variable (e.g. in shell) prior to invoking 'make'.
CHECKOPTS = --enable=performance,warning,portability --language=c \
            --error-exitcode=1 --std=c99 --inline-suppr $(CHECKEXTRA)

OBJECTS := $(subst $(OBJ)/,,$(OBJECTS))
COVFILES := $(subst .o,.c.gcov,$(OBJECTS))

TESTS := test-compat test-super test-errors

ifeq ($(CALCEPH_SUPPORT), 1)
  TESTS += test-calceph
  LDFLAGS += -lcalceph
  OBJECTS += solsys-calceph.o
  COVFILES += solsys-calceph.c.gcov
endif

ifeq ($(CSPICE_SUPPORT), 1)
  TESTS += test-cspice
  LDFLAGS += -lcspice
  OBJECTS += solsys-cspice.o
  COVFILES += solsys-cspice.c.gcov
endif

.PHONY: check-compat
check-compat:
	@sed -i.tmp "s:-nan: nan:g" data/* && rm -f data/*.tmp
	diff data reference

.PHONY: run
run: clean-cov clean-data $(TESTS)
	@mkdir data
	./test-compat
	$(MAKE) check-compat
	./test-super
	./test-errors
ifeq ($(CALCEPH_SUPPORT), 1)
	./test-calceph ephem
endif
ifeq ($(CSPICE_SUPPORT), 1)
	./test-cspice ephem
endif

cov: cov.info
	genhtml $< -o $@

cov.info:
	geninfo . -b . -o $@

.PHONY: coverage
coverage: $(COVFILES)
	$(MAKE) clean-test
	$(MAKE) cov

.PRECIOUS: %.c.gcov
%.c.gcov: %.gcno %.gcda
	gcov -b $*

.PRECIOUS: %.gcno %.gcda
%.gcno %.gcda: ../src/%.c
	$(MAKE) clean
	$(MAKE) run

test-%: test-%.o $(OBJECTS) | Makefile
	$(CC) -o $@ $^ $(LDFLAGS)

test-%.o: src/test-%.c | Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

%.o: ../src/%.c | Makefile
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

test-calceph: LDFLAGS += -lcalceph

test-cspice: LDFLAGS += -lcspice

# Static code analysis using 'cppcheck'
.PHONY: analyze
analyze:
	@echo "   [analyze]"
	@cppcheck $(CPPFLAGS) $(CHECKOPTS) $(SRC)

.PHONY: clean-test
clean-test:
	rm -f test-*

.PHONY: clean-cov
clean-cov:
	rm -rf gmon.out *.gcov *.gcda *.gcno cov cov.info

.PHONY: clean-data
clean-data:
	rm -rf data

.PHONY: clean
clean: clean-test clean-cov
	rm -f *.o

.PHONY: distclean
distclean: clean clean-data

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


vpath %.c ../src
vpath %.c src

