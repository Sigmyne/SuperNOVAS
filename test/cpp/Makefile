

# Flags for measuring test coverage
CPPFLAGS += -I. -I../../include
CXXFLAGS += -pg -Wall -fprofile-arcs --coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lm

include ../../config.mk

CPP_PATH := ../../cpp
CPP_SOURCES = $(wildcard $(CPP_PATH)/*.cpp)
CPP_OBJECTS = $(subst .cpp,.o, $(subst $(CPP_PATH)/,, $(CPP_SOURCES)))

TESTS = AngleTest TimeAngleTest IntervalTest DistanceTest SpeedTest TemperatureTest PressureTest \
	EOPTest SphericalTest CalendarTest PositionTest GalacticTest EclipticTest

.PHONY: build
build: $(TESTS)

.PHONY: run
run: build
	@for prog in $(TESTS) ; do ./$${prog} ; done

.PHONY: coverage
coverage: $(COVFILES)
	$(MAKE) clean-test
	$(MAKE) cov

%.o: %.cpp Makefile
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

%Test: %Test.o libnovas++.a $(CPP_OBJECTS)
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) -L. -lnovas++

.PRECIOUS: libnovas++.a
libnovas++.a: $(OBJECTS)
	ar -rc $@ $^
	ranlib $@

obj:
	mkdir $@

.PRECIOUS: obj/%.o
obj/%.o: ../../src/%.c obj Makefile
	$(CXX) -c -o $@ -DNOVAS_NAMESPACE $(CPPFLAGS) $< 

cov: cov.info
	genhtml $< -o $@

cov.info:
	geninfo . -b . -o $@

.PRECIOUS: %.c.gcov
%.c.gcov: %-%.gcno %-%.gcda
	gcov -b $*

.PRECIOUS: %.gcno %.gcda
%.gcno %.gcda: ../src/%.c
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
	rm -rf *.o obj

.PHONY: distclean
distclean: clean
	rm -f libnovas++.a

vpath %.cpp $(CPP_PATH)
vpath %.c ../../src