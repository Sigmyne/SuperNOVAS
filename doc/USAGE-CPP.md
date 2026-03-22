# C++ Usage

 - [Building your application with SuperNOVAS (C++)](#integration-cpp)
 - [Example C++ usage](#examples-cpp)

This guide is specifically for using __SuperNOVAS__ for your application via its C++11 API. There is a separate guide 
for [using the C99 API](USAGE-C99.md).

__SuperNOVAS__ is a C99 library at its core. If you are looking for maximum speed, or want to use SuperNOVAS on older 
platforms, the C99 API is there for you. However, if you want a higher-level, more intuitive, and more elegant way of
using __SuperNOVAS__ then the C++ API gives you that comfort.

The following links provide further useful documentation resources for you:
 
 - [C++ API documentation](https://sigmyne.github.io/SuperNOVAS/doc/html/cpp/topics.html)
 - [SuperNOVAS pages](https://sigmyne.github.io/SuperNOVAS) on github.io 
 
-----------------------------------------------------------------------------

<a name="integration-cpp"></a>
## Building your application with SuperNOVAS (C++)

There are a number of ways you can build your application with __SuperNOVAS__. See which of the options suits your
needs best:

 - [Using a GNU `Makefile`](#makefile-application-cpp)
 - [Using CMake](#cmake-application-cpp)


<a name="makefile-application-cpp"></a>
### Using a GNU `Makefile`

<details>

Provided you have installed the __SuperNOVAS__ headers and (static or shared) libraries into a standard location, you 
can build your application against it easily. For example, to build `myastroapp.cpp` against __SuperNOVAS__, you might 
have a `Makefile` with contents like:

```make
  myastroapp: myastroapp.cpp 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lsupernovas -lsupernovas++
```

I.e., you will need to link your program against *both* the C99 `supernovas` library *and* the C++ `supernovas++`
library on top of that. If your application uses optional planet or ephemeris calculator modules, you may need to 
specify the additional shared libraries also:

```make
  myastroapp: myastroapp.cpp
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lsupernovas -lsupernovas++ -lsolsys-calceph -lcalceph
```

</details>

<a name="cmake-application-cpp"></a>
### Using CMake

<details>

Add the appropriate bits from below to the `CMakeLists.txt` file of your application (`my-application`):

```cmake
  find_package(supernovas REQUIRED)
  target_include_directories(my-application PRIVATE ${supernovas_INCLUDE_DIRS})
  target_link_libraries(my-application PRIVATE ${supernovas_LIBRARIES})
```

</details>


-----------------------------------------------------------------------------

<a name="examples-cpp"></a>
## Example C++ usage

 - [Fundametals](#fundamentals-cpp)
 - [Calculating positions for a sidereal source](#sidereal-example-cpp)
 - [Calculating positions for a Solar-system source](#solsys-example-cpp)

<a name="fundamentals-cpp"></a>
### Fundamentals

 - [Namespace](#namespace-cpp)
 - [Validation](#validation-cpp)
 - [Immutable instances](#immutable-cpp)
 - [Classes don't reference external objects internally](#internal-copies-cpp)

Before we dive into specific examples for using the __SuperNOVAS__ C++ API, you should know of the generic
features and design principles that underly the C++ implementation.
 
<a name="namespace-cpp"></a>
#### Namespace

To allow using simple class names, while being cognizant of potential namespace conflicts, the __SuperNOVAS__ classes
all live under the `supernovas` namespace. Thus, the class `Angle`, for example, has a full name `supernovas::Angle`
that can be used in any context. But, when convenient you can make one of more namespaces default in your source
code, e.g.:

```cpp
 #include <supernovas.h>
 
 using namespace supernovas;
 
 void my_func() {
   Angle a(...);
   ...
 }
```

is equivalent to:

```cpp
 #include <supernovas.h>
 
 void my_func() {
   supernovas::Angle a(...);
   ...
 }
```

<a name="validation-cpp"></a>
#### Validation

C++ does not have runtime exceptions the same way as Python or Java, which can be caught. While C++17 introduced
`std::optional` types that can be used to return with or without a valid data, __SuperNOVAS__ does not use these,
because _(a)_ the optionals are not supported on Apple and Windows (in 2026), and _(b)_ they don't do anything for 
constructors.

Instead all __SuperNOVAS__ classes are based on a `supernovas::Validating` base class, providing an `is_valid()` 
method. All subclasses (that is all __SuperNOVAS__ classes) sanity check their data as part of their constructor. And, 
mutable classes sanity check every time they are modified as well. The checks typically include flagging NaNs and 
infinite values (unless they are explicitly allowed), enum values outside of their normal range (yes the compiler 
checks for these, but those checks can be easily bypassed), and whatever else is necessary to ensure that the objects 
contain fully usable data.

It is generally a good idea to check for validity whenever getting sane numerical data (i.e., not NaNs) is critical, or
if you are not entirely sure. For example:

```cpp
  Observer obs = ...  // Define an observer location
  if(!obs.is_valid()) {
    // Oops, something isn't right...
    return;
  }
```

or, equivalently the objects themselves can be evaluated as boolean types, the same as calling `is_valid()`, i.e.:

```cpp
  Observer obs = ...  // Define an observer location
  if(!obs) {
    // Oops, something isn't right...
    return;
  }
```

And if in doubt as to why your object is invalid, you can always turn on debugging with `novas_debug(NOVAS_DEBUG_ON)`,
at least in the relevant section of your code. __SuperNOVAS__ will provide error descriptions and call traces every
time an invalid class instance is created, and when methods create invalid objects themselves. 


<a name="immutable-cpp"></a>
#### Immutable instances

Almost all classes of the __SuperNOVAS__ library use immutable instances. The immutability makes these __SuperNOVAS__ 
classes inherently thread safe. Given they are immutable, it is impossible for one thread to modify the class while 
another accesses it concurrently. No mutexes are needed for these.

The exceptions to immutabilty are `supernovas::CatalogEntry`, `supernovas::Orbital` and `supernovas::OrbitalSystem`. 
These classes have too many possible parameters, and also alternate parametrizations. It simply does not make sense to 
define everything in a constuctor. Rather, these classes take a minimalist approach to the constructor, which define 
the most essential parameters only. Then, you can define additional (optional) parameters using a builder pattern (see 
for example the sections on defining a [catalog source](#specify-object-cpp) or an 
[orbital source](#orbital-sources-cpp)). This approach also handles alternate parametrizations (e.g. radial velocity 
vs. LSR velocity vs. redshift, or distance vs. parallax). The best practice for these mutable types is to define them 
once in a single-thread, and never attempt to further modify them once they are passed into a multi-threaded 
environment.


<a name="internal-copies-cpp"></a>
#### Classes don't reference external objects internally

The __SuperNOVAS__ classes never store references to external objects internally. Instead, they always store copies
of the parameters that were supplied. While this may result in a small computational overhead, it adds to the thread 
safety. Thus, even if a `supernovas::CatalogEntry` object is mutable, once a `supernovas::CatalogSource` is created 
from it, that source will store a copy of the entry internally. So, even if the entry is modified afterwards, the 
source stays immutable with the parameters that were used when it was instantiated.


<a name="sidereal-example-cpp"></a>
### Calculating positions for a sidereal source

A sidereal source may be anything beyond the Solar system with 'fixed' catalog coordinates. It may be a star, or a 
galactic molecular cloud, or a distant quasar. 

 - [Specify the object of interest](#specify-object-cpp)
 - [Specify the observer location](#specify-observer-cpp)
 - [Specify the time of observation](#specify-time-cpp)
 - [Set up the observing frame](#observing-frame-cpp)
 - [Calculate an apparent place on sky](#apparent-place-cpp)
 - [Calculate azimuth and elevation angles at the observing location](#horizontal-place-cpp)
 - [Going in reverse...](#reverse-place-cpp)
 - [Calculate rise, set, and transit times](#rise-set-transit-cpp)

<a name="specify-object-cpp"></a>
#### Specify the object of interest

First, you must provide the astrometric parameters (coordinates, and optionally radial velocity or redshift, proper 
motion, and/or parallax or distance also). Let's assume we pick a star for which we have B1950 (i.e. FK4) coordinates. 
We begin with the assigned name and the R.A. / Dec coordinates, and then populate any other astrometric parameters we
may have:

```cpp
 // Let's assume we have B1950 (FK4) coordinates
 CatalogEntry entry = CatalogEntry("Antares", "16h26m20.1918s", "-26d19m23.138s", Equinox::b1950())
   .proper_motion(-12.11 * Unit::mas / Unit::yr, -23.30 * Unit::mas / Unit:yr)
   .parallax(5.89 * Unit::mas)
   .radial_velocity(5.89 * Unit::km / Unit::s);
```

Above, the coordinates were specified as strings. but they could have been floating-point values (e.g. `16.43894213 *
Unit::hour_angle`, `-26.323094 * Unit::deg`), or `supernovas::Angle` / `supernovas::TimeAngle` objects instead. We 
also specified an equinox (B1950), but you can skip that if you use ICRS coordinates. After that, we used a builder 
pattern to add additional detail, such as proper motion, parallax, and an (SSB-based) radial velocity. We could have 
also set `distance()` instead of `parallax()`, or `v_lsr()` or `redshift()` instead of `radial_velocity()`. Use what 
fits your needs best. Note the use of physical units along with the numerical data. 

Next, we we create a `Source` type object from this catalog entry:

```cpp
  auto source = entry.to_source();
```

The `supernovas::Source` class can represent different astronomical objects, and accordingly has different subclasses. 
Specifically, `entry.to_source()` above returns a `supernovas::CatalogSource`, but thanks to the `auto` keyword, we 
don't really need to know what subclass of `Source` is actually being created here.


<a name="specify-observer-cpp"></a>
#### Specify the observer location

##### A. Earth-based observer location

Next, we define the location where we observe from. Let's assume we have a GPS location, such as 50.7374 deg N, 7.0982 
deg E, 60m elevation:

```cpp
 // Specify the location we are observing from, e.g. a GPS / WGS84 location
 Site site = Site::from_GPS(50.7374 * Unit::deg, 7.0982 * Unit::deg, 60.0 * Unit::m);
```

> [!TIP]
> You might use one of the `supernovas::Site()` constructors directly if the location is defined on the GRS80 
> reference ellipsoid, or if you have geocentric Cartesian coordinates.

Again you could have specified the coordinates as DMS strings, or as `supernovas::Angle` objects also, e.g.:

```cpp
 Site site = Site::from_GPS("50.7374N", "7.0982 deg E", Coordinate(60.0 * Unit::m));
```

Next, you will want to create an `supernovas::Observer` instance for that location with the appropriate Earth 
Orientation Parameters (EOP), such as obtained from the 
[IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html) or data service. These are leap 
seconds, the UT1-UTC time difference capturing variations in Earth's rotation, and the _x<sub>p</sub>_, 
_y<sub>p</sub>_ polar offsets, which measure small wanders of Earth's rotational pole w.r.t. the crust:

```cpp
 // Let's assume 37 leap seconds, 0.6447s UT1-UTC difference, and some polar offsets:
 EOP eop(37, 0.6447 * Unit::s, 103.2 * Unit::mas, 211.3 * Unit::mas);
 
 // Now create an observer for that site
 auto obs = site::to_observer(eop);
```

Again, `supernovas::Observer` has many subclasses of specific flavors, so here we use the `auto` keyword again if we 
are lazy (otherwise we could have specified `supernovas::GeodeticObserver`, which is what `site.to_observer()` 
returns).

##### B. other observer locations

Alternatively, you can also specify airborne observers, or observers in Earth orbit, in heliocentric orbit, at the 
geocenter, or at the Solar-system barycenter. See the static methods of the `supernovas::Observer` class. E.g.:

```cpp
  auto obs = Observer::at_geocenter();
```


<a name="specify-time-cpp"></a>
#### Specify the time of observation
 
Then we can set the time of observation, for example, using the current UNIX time:

```cpp
 // Set the time of observation to the precise UTC-based UNIX time
 Time obs_time = Time::now(eop);
```

Once again, EOP is needed for the leap seconds and UT1-UTC time difference. 

Alternatively, you may set the time as a Julian date in the time measure of choice (UTC, UT1, TT, TDB, GPS, TAI, TCG, 
or TCB):

```cpp
 double jd_tai = ...     // TAI-based Julian Date 

 Time obs_time(jd_tai, eop, NOVAS_TAI)
```

or, for the best precision we may do the same with an integer / fractional split:

```cpp
 long ijd_tai = ...     // Integer part of the TAI-based Julian Date
 double fjd_tai = ...   // Fractional part of the TAI-based Julian Date 
  
 Time obs_time(ijd_tai, fjd_tai, eop, NOVAS_TAI);
```

Or, you might use string dates, such as an ISO timestamp:

```cpp
 Time obs_time = Time("2025-01-26T22:05:14.234+0200", eop);
```

<a name="observing-frame-cpp"></a>
#### Set up the observing frame

Next, we set up an observing frame, which is defined for a unique combination of the observer location and the time of
observation:

```cpp
 // Initialize the observing frame for the observer at the time of observation.
 Frame frame = obs.frame_at(obs_time);
 
 // It's a good idea to confirm that the frame is actually valid
 if(!frame) {
   // Oops, not a valid frame, perhaps because we don't have an ephemeris provider configured.
   ...
 }
```

Or, you can use `obs.reduced_accuracy_frame_at(time)` to construct a frame with mas-level accuracy only.

> [!IMPORTANT]
> Without a proper ephemeris provider for the major planets, you are invariably restricted to working with reduced 
> accuracy frames, providing milliarcsecond precision at most. Attempting to construct high-accuracy frames without an 
> appropriate high-precision ephemeris provider will result in an error from the requisite `ephemeris()` calls.

> [!TIP]
> Full accuracy (&mu;as-level) frames require a high-precision ephemeris provider for the major planets, e.g. to account 
> for the gravitational deflections. Without it, &mu;as accuracy cannot be ensured, in general. See section on 
> [Incorporating Solar-system ephemeris data or services](#solarsystem) further below.


<a name="apparent-place-cpp"></a>
#### Calculate an apparent place on sky

Now we can calculate the apparent R.A. and declination for our source, which includes proper motion (for sidereal
sources) or light-time correction (for Solar-system bodies), and also aberration corrections for the moving observer 
and gravitational deflection around the major Solar System bodies (in full accuracy mode). You can calculate an 
apparent location in the:

```cpp
 // Precise apparent positions and spectroscopic velocities (in TOD).
 Apparent app = source.apparent_in(frame);
```

You cat get true-of-date (TOD) equatorial coordinates (`.equatorial()`), or in CIRS (`.cirs()`). And you can convert
these to any other coordinate system of choice (ICRS/GCRS, J2000, or MOD), e.g.:

```cpp
 // true-of-date apparent coordinates
 Equatorial tod = app.equatorial();
 
 // the same converted to ICRS
 Equatorial icrs = tod.to_icrs();
```

You can also obtain ecliptic (TOD) and galactic coordinates the same way, and convert ecliptic coordinates to other 
equinoxes just like we did for the equatorial:

```cpp
 // apparent ecliptic coordinates in J2000
 Ecliptic ecl = app.ecliptic() >> NOVAS_J2000;
 
 // apparent Galactic coordinates
 Galactic gal = app.galactic();
```

Above the `>>` operator is used as a shorthand for the `.to_system()` method. It's the same as writing 
`to_system(NOVAS_ICRS)`. For spectroscopic applications, you can get a spectroscopic radial velocity or redshift 
(including gravitational effects) as:

```cpp
 ScalarVelocity rv = app.radial_velocity();
 
 double z = app.redshift();
```

And, you can also get a distance:

```cpp
 Distance d = app.distance();
```

> [!NOTE]
> If you want geometric positions and/or velocities instead, without aberration and gravitational deflection, you 
> might use `supernovas::Source::geometric_in(Frame&)` instead of `supernovas::Source::apparent_in(Frame&)`. 


<a name="horizontal-place-cpp"></a>
#### Calculate azimuth and elevation angles at the observing location

If your ultimate goal is to calculate the azimuth and elevation angles of the source at the specified observing 
location, you can proceed from the `Apparent` positions you obtained above, provided they are calculated for an
Earth based observer (otherwise, you'll get an invalid result):

```cpp
 // Convert the apparent position to unrefracted horizontal coordinates
 Horizontal hor = app.to_horizontal()
 
 // ... or apply atmospheric refraction with the model and weather of choice
 // Lets define the weather parameters explicitly as 12.0C, 895 mbar, and 47% humidity:
 Weather weather = Weather(Temperature::celsius(12.0), Pressure::mbar(895.0), 47.0);
 
 // Obrtain refraction cofrrected horizontal coordinates
 hor = app.to_horizontal().to_refracted(novas_optical_refraction, weather);
```

<a name="reverse-place-cpp"></a>
#### Going in reverse...

Of course, __SuperNOVAS__ allows you to go in reverse, for example from an observed Az/El position all the way to
proper ICRS R.A./Dec coordinates, or a geometric place.

E.g., let's assume you start with horizontal coorinates that measured at your observing location:

```cpp
 Horizontal hor = ...;      // observer azimuth and elevation angles
```

If needed correct for atmospheric refraction.

```cpp
 Weather weather(...); // define local weather parameters for the refraction (if needed)
 
 hor = hor.to_unrefracted(novas_optical_refraction, weather);
```
  
Noew you can calculate an apparent place on the celestial sphere given your observing frame (precise location and time
of observation).

```cpp
 Apparent app = hor.to_apparent(frame);
```

or,

```cpp
 Apparent app = hor.to_apparent(frame, ScalarVelocity(-14.2 * Unit::km / Unit::s), Distance(43.6 * Unit::pc);
```
  
Note, that when radial velocity and/or distance is not explicitly defined, they are assumed as 0 (km/s) and 1 Gpc, 
respectively. Next, you can convert the apparent place to a geometric position, referenced to the time when the 
observed light originated from the source (at the distance defined or assumed):

```cpp
 AstrometricPosition pos = app.astrometric_position();
```

Or, calculate the geometric position relative to the SSB instead...

```cpp
 AstrometricPosition pos_ssb = app.astrometric_position().referenced_to_ssb();
```

And finally, you can calculate nominal SSB-based ICRS coordinates as:

```cpp
 Equatorial icrs = pos_ssb.as_equatorial().to_icrs();
```


<a name="rise-set-transit-cpp"></a>
#### Calculate rise, set, and transit times

You may be interested to know when sources rise above or set below some specific elevation angle, or at what time they 
appear to transit at the observer location. __SuperNOVAS__ has routines to help you with that too.

Given that rise, set, or transit times are dependent on the day of observation, and observer location, they are 
effectively tied to an observer frame. Let's assume you have defined a source and an observing frame:

```cpp
 Frame frame = ...;        // Earth-based observer location and lower-bound time of interest.
 Source source = ...;      // Source of interest
```

Let's calculate the time when source rises above 30 degrees of elevation next *after* the observing time of the frame, 
given the NOVAS optical refraction model.

```cpp
 Weather weather = ...;    // Define local weather parameters... 
 
 Time t_rise = source.rises_above(30.0 * Unit::deg, frame, novas_optical_refraction, weather);
```

Or, calculate the time the source transits after the frame's time of observation:

```cpp
 Time t_transit = source.transits_in(frame);
```
 
Or, calculate the next time when source sets below 30 degrees of elevation, not accounting for refraction.

```cpp
 Time t_set = source.sets_below(30.0 * Unit::deg, frame);
```

Note, that in the current implementation these calls are not well suited sources that are at or within the 
geostationary orbit, such as such as Low Earth Orbit satellites (LEOs), geostationary satellites (which never really 
rise, set, or transit), or some Near Earth Objects (NEOs), which will rise set multiple times per day. For the latter, 
the above calls may still return a valid time, only without the guarantee that it is the time of the first such event 
after the specified frame instant. A future implementation may address near-Earth orbits better, so stay tuned for 
updates.


<a name="solsys-example-cpp"></a>
### Calculating positions for a Solar-system source

 - [Planets and/or ephemeris type objects](#ephemeris-sources-cpp)
 - [Solar-system objects with Keplerian orbital parameters](#orbital-sources-cpp)
 - [Approximate planet orbitals](#planet-orbitals-cpp)

Solar-system sources work similarly to the above with a few important differences at the start.

<a name="ephemeris-sources-cpp"></a>
#### Planets and/or ephemeris type objects

Historically, NOVAS divided Solar-system objects into two categories: (1) major planets (including also the Sun, the 
Moon, and the Solar-system Barycenter); and (2) 'ephemeris' type objects, which are all other Solar-system objects.
The main difference is the numbering convention. NOVAS major planets have definitive ID numbers (see 
`enum novas_planet`), whereas 'ephemeris' objects have user-defined IDs. They are also handled by two separate adapter 
functions (although __SuperNOVAS__ has the option of using the same ephemeris provider for both types of objects 
also).

Thus, you define your `Source` as a `supernovas::Planet` or `supernovas::EphemerisSource` type object with the name or 
ID number that is used by the ephemeris service you provided. For major planets you might want to use `Planet`, if 
they use a `novas_planet_provider` function to access ephemeris data with their NOVAS IDs, or else 
`supernovas::EphemerisSource` for more generic ephemeris handling via a user-provided `novas_ephem_provider`. E.g.:

```cpp
 // Planet types are handled by the planet provider function.
 auto mars = Planet::mars();
  
 // Ceres will be handled by the generic ephemeris provider function, which let's say 
 // uses the NAIF ID of 2000001 _or_ the name 'Ceres' (depending on the implementation)
 auto ceres = EphemerisSource("Ceres", 2000001);
```

> [!IMPORTANT] 
> Before you can handle all major planets and other ephemeris objects this way, you will have to provide one or more 
> functions to obtain the barycentric ICRS positions for your Solar-system source(s) of interest for the specific 
> Barycentric Dynamical Time (TDB) of observation. See section on 
> [Incorporating Solar-system ephemeris data or services](#solarsystem). 

And then, it's the same spiel as before, e.g.:

```cpp
 Apparent app = mars.apparent_in(frame);
```

or to get geometric (unaberrated, undeflected) positions and velocities of when the light left the Solar-system
body:

```cpp
 // Obtain geometric positions / velocities for when light
 Geometric geom = ceres.geometric_in(frame);
 
 // The 3D geometric position of Ceres at the time the observed light originated 
 Position pos = geom.position();
 
 // 3D geometric velocity of Ceres, antedated for when observed light originated
 Velocity vel = geom.velocity();
```

<a name="orbital-sources-cpp"></a>
#### Solar-system objects with Keplerian orbital parameters

You can also define solar system sources with Keplerian orbital elements (such as the most up-to-date ones provided by 
the [Minor Planet Center](https://minorplanetcenter.net/data) for asteroids, comets, etc.):

```cpp
 // e.g. a Near-Earth Asteroid in 
 Time ref_time = ... // reference time for which orbital parameters are defined
 
 // Define the orbital parameters in the given orbital system:
 Orbital orb = OrbitalSystem::equatorial(Planet::earth())
   .orbit(ref_time, 3452.0 * Unit::km, 192.3 * Unit::deg 3.55 * Unit::h)    // t0, M0, T
   .eccentricity(0.049, 44.1 * Unit::deg)                // e, omega
   .inclination(11.3 * Unit::deg, -112.1 * Unit::deg)    // i, Omega
   .apsis_period(14.5 * Unit::yr)
   .node_period(29.0 * Unit::yr);
   
 // Make an OrbitalSource with the designated name
 auto nea = orb.to_source("name-this-NEA");
```

> [!NOTE]
> Even with orbital elements, you will, in general, still require an ephemeris provider also, to obtain precise 
> positions for the Sun, an Earth-based observer, or the planet, around which the orbit is defined.

You don't have to fill all the parameters, and instead of setting inclination and the argument of the rising node,
you could also set the location of the orbital pole (via `pole()`). And, instead of orbital period, you might use
a mean motion parameter to instantiate the orbit with `supernovas::Orbital::from_mean_motion()`.

And then, it's once again the same spiel as before, e.g.:

```cpp
 Apparent app = nea.apparent_in(frame);
```

or

```cpp
 Geometric geom = nea.geometric_in(frame);
```


<a name="planet-orbitals-cpp"></a>
#### Approximate planet orbitals


Finally, you might generate approximate (arcmin-level) orbitals for the major planets (but not Earth!), the Moon, and 
the Earth-Moon Barycenter (EMB) also. E.g.:

```cpp
 // The current Keplerian orbital of Venus...
 Orbital orb = Planet::venus().orbit(Time::now());
```

Or, you can use such orbitals (implicitly) to calculate approximate positions and velocities for the planets, e.g.:

```cpp
 // Approximate, orbital model based, apparent positions for Mars...
 Apparent app = Planet::mars().approx_apparent_in(frame);
 
 //  Approximate, orbital model based, geometric positions and velocities for Neptune...
 Geometric geom = Planet::neptune().approx_geometric_in(frame);
```

Keep in mind that the planet and Moon orbitals are not suitable for precision applications.


-----------------------------------------------------------------------------
Copyright (C) 2026 Attila Kovács

