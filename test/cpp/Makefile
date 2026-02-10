# Flags for measuring test coverage

CPPFLAGS += -I. -I../../include
LDFLAGS += --coverage -lm 

include ../../config.mk
SRC := ../../src

CPP_SRC := ../../cpp
CPP_SOURCES = $(wildcard $(CPP_SRC)/*.cpp)
CPP_OBJECTS = $(subst .cpp,.opp, $(subst $(CPP_SRC)/,obj/, $(CPP_SOURCES)))

TESTS = AngleTest TimeAngleTest IntervalTest DistanceTest SpeedTest TemperatureTest PressureTest \
	EOPTest SphericalTest CalendarTest PositionTest GalacticTest EclipticTest WeatherTest \
	VelocityTest EquinoxTest VectorTest CatalogEntryTest OrbitalTest EquatorialTest \
	TemperatureTest SiteTest ObserverTest TimeTest FrameTest SourceTest HorizontalTest \
	ApparentTest

.PHONY: build
build: libnovas++.a $(TESTS)

.PHONY: run
run: build
	@for prog in $(TESTS) ; do ./$${prog} ; done

.PHONY: coverage
coverage: run
	$(MAKE) cov

%Test: %Test.cpp $(CPP_OBJECTS)
	$(CXX) -o $@ $(CPPFLAGS) -g $(CXXFLAGS) $^ $(LDFLAGS) libnovas++.a

libnovas++.a: $(OBJECTS)
	ar -rc $@ $^
	ranlib $@

obj:
	mkdir $@

obj/%.o: $(SRC)/%.c | obj
	$(CXX) -c -o $@ -DNOVAS_NAMESPACE $(CPPFLAGS) $< 

obj/%.opp: $(CPP_SRC)/%.cpp | obj
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) -pg -Wall --coverage $<

cov: cov.info
	genhtml $< -o $@

cov.info:
	geninfo obj -o $@

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
	rm -rf *.o *.opp obj

.PHONY: distclean
distclean: clean
	rm -f libnovas++.a
