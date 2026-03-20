# Flags for measuring test coverage

CPPFLAGS += -I../../include
LDFLAGS += --coverage -lm 

include ../../config.mk

CPP_SRC := ../../src/cpp
CPP_SOURCES = $(wildcard $(CPP_SRC)/*.cpp)
CPP_OBJECTS = $(subst .cpp,.opp, $(subst $(CPP_SRC)/,obj/, $(CPP_SOURCES)))

TESTS = $(subst .cpp,, $(wildcard *.cpp))


.PHONY: build
build: libnovas.a $(TESTS)

.PHONY: run
run: build
	@for prog in $(TESTS) ; do ./$${prog} ; done

%Test: %Test.cpp $(CPP_OBJECTS)
	$(CXX) -o $@ $(CPPFLAGS) -g $(CXXFLAGS) $^ $(LDFLAGS) libnovas.a

libnovas.a: $(OBJECTS)
	ar -rc $@ $^
	ranlib $@

obj:
	mkdir $@

obj/%.o: %.c | obj
	@echo $(CPPFLAGS)
	$(CC) -c -o $@ $(CPPFLAGS) $< 

obj/%.opp: %.cpp | obj
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -pg -Wall -fno-elide-constructors -fno-default-inline --coverage $<

.PRECIOUS: %.c.gcov
%.c.gcov: %-%.gcno %-%.gcda
	gcov -b $*

.PRECIOUS: %.gcno %.gcda
%.gcno %.gcda: src/%.cpp
	$(MAKE) clean
	$(MAKE) run

.PHONY: clean-test
clean-test:
	rm -f *Test

.PHONY: clean-cov
clean-cov:
	rm -rf gmon.out *.gcov *.gcda *.gcno cov cov.info

.PHONY: clean
clean: clean-test clean-cov
	rm -rf *.o *.opp obj

.PHONY: distclean
distclean: clean
	rm -f libnovas.a

vpath %.c ../../src/c99
vpath %.cpp ../../src/cpp

