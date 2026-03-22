# C99 Usage

 - [Building your application with SuperNOVAS (C99)](#integration-c99)
 - [Example C99 usage](#examples-c99)
 - [Tips &amp; Tricks (C99)](#tips-c99)

This guide is specifically for using __SuperNOVAS__ for your application via its C99 API. There is a separate guide 
for [using the C++11 API](USAGE-CPP.md).

__SuperNOVAS__ is a C99 library at its core. If you are looking for maximum speed, or want to use SuperNOVAS on older 
platforms, the C99 API is there for you. 

The following links provide further useful documentation resources for you:
 
 - [C99 API documentation](https://sigmyne.github.io/SuperNOVAS/doc/html/topics.html)
 - [SuperNOVAS pages](https://sigmyne.github.io/SuperNOVAS) on github.io 
 
-----------------------------------------------------------------------------

<a name="integration-c99"></a>
## Building your application with SuperNOVAS (C99)

There are a number of ways you can build your application with __SuperNOVAS__. See which of the options suits your
needs best:

 - [Using a GNU `Makefile`](#makefile-application-c99)
 - [Using CMake](#cmake-application-c99)
 - [Deprecated API](#deprecations-c99)
 - [Legacy linking `solarsystem()` and `readeph()` modules](#legacy-application-c99)
 - [Legacy modules: a better way](#preferred-legacy-application-c99)


<a name="makefile-application-c99"></a>
### Using a GNU `Makefile`

<details>

Provided you have installed the __SuperNOVAS__ headers and (static or shared) libraries into a standard location, you 
can build your application against it easily. For example, to build `myastroapp.c` against __SuperNOVAS__, you might 
have a `Makefile` with contents like:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lsupernovas
```

If you have a legacy NOVAS C 3.1 application, it is possible that the compilation will give you errors due to missing 
includes for `stdio.h`, `stdlib.h`, `ctype.h` or `string.h`, because these headers were implicitly included with 
`novas.h` in NOVAS C 3.1, but not in __SuperNOVAS__ (at least not by default), as a matter of best practice. If this 
is a problem for you can 'fix' it in one of two ways: (1) Add the missing `#include` directives to your application 
source explicitly, or if that's not an option for you, then (2) set the `-DCOMPAT` compiler flag when compiling your 
application:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) -DCOMPAT $^ -lm -lsupernovas
```

If your application uses optional planet or ephemeris calculator modules, you may need to specify the additional 
shared libraries also:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lsupernovas -lsolsys-calceph -lcalceph
```

</details>

<a name="cmake-application-c99"></a>
### Using CMake

<details>

Add the appropriate bits from below to the `CMakeLists.txt` file of your application (`my-application`):

```cmake
  find_package(supernovas REQUIRED)
  target_include_directories(my-application PRIVATE ${supernovas_INCLUDE_DIRS})
  target_link_libraries(my-application PRIVATE ${supernovas_LIBRARIES})
```

</details>

<a name="deprecations-c99"></a>
### Deprecated API

__SuperNOVAS__ began deprecating some NOVAS C functions, either because they are no longer needed; or are not easy to 
use and have better alternatives around; or are internals that should never have been exposed to end-users. The 
deprecations are marked in the inline and HTML API documentations, suggesting also alternatives. 

That said, the deprecated parts of the API are NOT removed, nor we plan on removing them for the foreseeable future. 
Instead, they serve as a gentle reminder to users that perhaps they should stay away from these features for their own 
good.

However, you have the option to force yourself to avoid using the deprecated API if you choose to do so, by compiling
your application with `-D_EXCLUDE_DEPRECATED`, or equivalently, by defining `_EXCLUDE_DEPRECATED` in your source code 
_before_ including `novas.h`. E.g.:

```c
// Include novas.h without the deprecated definitions
#define _EXCLUDE_DEPRECATED
#include <novas.h>
```

After that, your compiler will complain if your source code references any of the deprecated entities, so you may 
change that part of your code to use the recommended alternatives instead.

<a name="legacy-application-c99"></a>
### Legacy linking `solarsystem()` / `solarsystem_hp()` and `readeph()` modules

The NOVAS C way to handle planet or other ephemeris functions was to link particular modules to provide the
`solarsystem()` / `solarsystem_hp()` and `readeph()` functions. This approach is discouraged in __SuperNOVAS__, with
preference for selecting implementations at runtime. The old, deprecated way, of incorporating Solar-system data is 
supported, nevertheless, for legacy applications with some caveats.

<details>

To use your own existing default `solarsystem()` implementation in the NOVAS C way, you will have to build 
__SuperNOVAS__ with `SOLSYS_SOURCE` set to the source file(s) of the implementation (`config.mk` or the environment).

The same principle applies to using your specific legacy `readeph()` implementation, except that you must set 
`READEPH_SOURCE` to the source file(s) of the chosen implementation when building __SuperNOVAS__). 

(You might have to also add additional include directories to `CPPFLAGS`, e.g. `-I/my-path/include` for you custom 
sources for their associated headers).

</details>

<a name="preferred-legacy-application-c99"></a>
### Legacy modules: a better way

A better way to recycle your old planet and ephemeris calculator modules may be to rename `solarsystem()` / 
`solarsystem_hp()` functions therein to e.g. `my_planet_calculator()` / `my_planet_calculator_hp()` and then in your 
application can specify these functions as the provider at runtime.

<details>

E.g.:

```c
  set_planet_calculator(my_planet_calculator);
  set_planet_calculator(my_planet_calculator_hp);
```

(You might also change the `short` type parameters to the SuperNOVAS enum types while at it, to conform to the 
SuperNOVAS `novas_planet_provider` / `novas_planet_provider_hp` types.)

For `readeph()` implementations, it is recommended that you change both the name and the footprint to e.g.:

```c
  int my_ephem_provider(const char *name, long id, double jd_tdb_high, double jd_tdb_low, 
                        enum novas_origin *origin, double *pos, double *vel);
```

and then then apply it in your application as:

```c
  set_ephem_provider(my_ephem_provider);
```

While all of that requires some minimal changes to your old code, the advantage of this preferred approach is that
you do not need to re-build the library with `USER_SOLSYS` and/or `USER_READEPH` defined.

</details>

-----------------------------------------------------------------------------

<a name="examples-c99"></a>
## Example C99 usage

 - [Calculating positions for a sidereal source](#sidereal-example-c99)
 - [Calculating positions for a Solar-system source](#solsys-example-c99)
 - [Coordinate and velocity transforms (change of coordinate system)](#transforms-c99)

__SuperNOVAS v1.1__ has introduced a new, more intuitive, more elegant, and more efficient approach for calculating
astrometric positions of celestial objects. The guide below is geared towards this new method. However, the original
NOVAS C approach remains viable also (albeit often less efficient).

> [!NOTE]
> You may find an equivalent legacy example showcasing the original NOVAS method in 
> [`NOVAS-legacy.md`](https://github.com/Sigmyne/SuperNOVAS/blob/main/doc/NOVAS-legacy.md).

<a name="sidereal-example-c99"></a>
### Calculating positions for a sidereal source

A sidereal source may be anything beyond the Solar system with 'fixed' catalog coordinates. It may be a star, or a 
galactic molecular cloud, or a distant quasar. 

 - [Specify the object of interest](#specify-object-c99)
 - [Specify the observer location](#specify-observer-c99)
 - [Specify the time of observation](#specify-time-c99)
 - [Set up the observing frame](#observing-frame-c99)
 - [Calculate an apparent place on sky](#apparent-place-c99)
 - [Calculate azimuth and elevation angles at the observing location](#horizontal-place-c99)
 - [Going in reverse...](#reverse-place-c99)
 - [Calculate rise, set, and transit times](#rise-set-transit-c99)

<a name="specify-object-c99"></a>
#### Specify the object of interest

First, you must provide the astrometric parameters (coordinates, and optionally radial velocity or redshift, proper 
motion, and/or parallax or distance also). Let's assume we pick a star for which we have B1950 (i.e. FK4) coordinates. 
We begin with the assigned name and the R.A. / Dec coordinates.

```c
 cat_entry star; // Structure to contain information on sidereal source 

 // Let's assume we have B1950 (FK4) coordinates of
 // 16h26m20.1918s, -26d19m23.138s
 novas_init_cat_entry(&star, "Antares", 16.43894213, -26.323094);
```

If you have coordinates as strings in decimal or HMS / DMS format, you might use `novas_str_hours()` and/or 
`novas_str_degrees()` to convert them to hours/degrees for `novas_init_cat_entry()`, with a fair bit of flexibility on 
the particulars of the representation, e.g.:

```c
 novas_init_entry(&star, "Antares",
   novas_str_hours("16h 26m 20.1918s"),   // e.g. using h,m,s and spaces as separators 
   novas_str_degrees("-26:19:23.138"));   // e.g. using colons to separate components
```

Next, if it's a star or some other source within our own Galaxy, you'll want to specify its proper motion (in the same
reference system as the above coordinates), so we can calculate its position for the epoch of observation.

```c
 // Now, let's add proper motion of 12.11, -23.30 mas/year.
 novas_set_proper_motion(&star, -12.11, -23.30);
``` 

For Galactic sources you will also want to set the parallax using `novas_set_parallax()` or equivalently the distance 
(in parsecs) using `novas_set_distance()`, e.g.:
 
```c
 // Add parallax of 5.89 mas
 novas_set_parallax(&star, 5.89);
```

Finally, for spectroscopic applications you will also want to set the radial velocity. You can use 
`novas_set_ssb_vel()` if you have standard radial velocities defined with respect to the Solar System Barycenter; or
`novas_set_lsr_vel()` if the velocity is relative to the Local Standard of Rest (LSR); or else `novas_set_redshift()`
if you have a redshift measure (as is typical for distant galaxies and quasars). E.g.:

```c
 // Add a radial velocity of -3.4 km/s (relative to SSB)
 novas_set_ssb_vel(&star, -3.4);
```

Alternatively, if you prefer, you may use the original NOVAS C `make_cat_entry()` to set the astrometric parameters 
above all at once.

<details>

E.g.:

```c
  make_cat_entry("Antares", "FK4", 1, 
   novas_str_hours("16h 26m 20.1918s"), 
   novas_str_degrees("-26:19:23.138"),
   -12.11, -23.30, 5.89, -3.4, &star);
```

</details>

Next, we wrap that catalog source into a generic celestial `object` structure. (An `object` handles various 
Solar-system sources also, as you'll see further below). Whereas the catalog source may have been defined in any epoch 
/ catalog system, the `object` structure shall define ICRS coordinates always (no exceptions):

```c
 object source;   // Encapsulates a sidereal or a Solar-system source
  
 // Use the B1950 astrometric parameters to set up the observables in ICRS...
 make_cat_object_sys(&star, "B1950", &source);
```

Alternatively, for high-_z_ sources you might simply use the 1-step `make_redshifted_object_sys()` e.g.:

```c
 object quasar;
  
 // 12h29m6.6997s +2d3m8.598s (ICRS) z=0.158339
 make_redshifted_object_sys("3c273", 12.4851944, 2.0523883, "ICRS", 0.158339, &quasar);
```


<a name="specify-observer-c99"></a>
#### Specify the observer location

Next, we define the location where we observe from. Let's assume we have a GPS location:

```c
 observer obs;    // Structure to contain observer location 

 // Specify the location we are observing from, e.g. a GPS / WGS84 location
 // 50.7374 deg N, 7.0982 deg E, 60m elevation
 make_gps_observer(50.7374, 7.0982, 60.0, &obs);
```

> [!TIP]
> You might use `make_itrf_observer()` instead if the location is defined on the GRS80 reference ellipsoid, used e.g. 
> for ITRF locations. Or, if you have geocentric Cartesian coordinates, you could use `make_xyz_site()` and then
> `make_observer_at_site()`.

Again you might use `novas_str_degrees()` for typical string representations of the longitude and latitude coordinates 
here, such as:

```c
 make_gps_observer(
   novas_str_degrees("50.7374N"),
   novas_str_degrees("7.0982 deg E"),
   60.0,
   &obs);
```

Alternatively, you can also specify airborne observers, or observers in Earth orbit, in heliocentric orbit, at the 
geocenter, or at the Solar-system barycenter. The above also sets default, mean annual weather parameters based on
the location and a global model based on Feulner et al. (2013). 

You can, of course, set actual weather values _after_, as appropriate, if you need them for the refraction models, 
e.g.:

```c
  obs.on_surf.temperature = 12.4;  // [C] Ambient temperature
  obs.on_surf.pressure = 973.47;   // [mbar] Atmospheric pressure
  obs.on_surf.humidity = 48.3;	   // [%] relative humidity to use for refraction.
```

<a name="specify-time-c99"></a>
#### Specify the time of observation

Next, we set the time of observation. For a ground-based observer, you will need to provide __SuperNOVAS__ with the
UT1 - UTC time difference (a.k.a. DUT1), and the current leap seconds. You can obtain suitable values for DUT1 from 
IERS, and for the highest precision, interpolate for the time of observations. For the example, let's assume 37 leap 
seconds, and DUT1 = 0.042,

```c
  int leap_seconds = 37;        // [s] UTC - TAI time difference
  double dut1 = 0.042;          // [s] UT1 - UTC time difference
```
 
Then we can set the time of observation, for example, using the current UNIX time:

```c
 novas_timespec obs_time;       // Structure that will define astrometric time

 // Set the time of observation to the precise UTC-based UNIX time
 novas_set_current_time(leap_seconds, dut1, &obs_time);
```

Alternatively, you may set the time as a Julian date in the time measure of choice (UTC, UT1, TT, TDB, GPS, TAI, TCG, 
or TCB):

```c
 double jd_tai = ...     // TAI-based Julian Date 

 novas_set_time(NOVAS_TAI, jd_tai, leap_seconds, dut1, &obs_time);
```

or, for the best precision we may do the same with an integer / fractional split:

```c
 long ijd_tai = ...     // Integer part of the TAI-based Julian Date
 double fjd_tai = ...   // Fractional part of the TAI-based Julian Date 
  
 novas_set_split_time(NOVAS_TAI, ijd_tai, fjd_tai, leap_seconds, dut1, &obs_time);
```

Or, you might use string dates, such as an ISO timestamp:

```c
 novas_set_str_time(NOVAS_UTC, "2025-01-26T22:05:14.234+0200", leap_seconds, dut1, &obs_time);
```

Note, that the likes of `novas_set_time()` will automatically apply diurnal corrections to the supplied UT1-UTC time 
difference for libration and ocean tides. Thus, the supplied values should not include these. Rather you should pass
`dut1` directly (or interpolated) from the IERS Bulletin values for the time of observation.

<a name="observing-frame-c99"></a>
#### Set up the observing frame

Next, we set up an observing frame, which is defined for a unique combination of the observer location and the time of
observation:

```c
 novas_frame obs_frame;  // Structure that will define the observing frame
 double xp = ...         // [mas] Earth polar offset x, e.g. from IERS Bulletin A.
 double yp = ...         // [mas] Earth polar offset y, from same source as above.
  
 // Initialize the observing frame with the given observing parameters
 novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &obs_time, xp, yp, &obs_frame);
```

Here `xp` and `yp` are small (sub-arcsec level) corrections to Earth orientation. Values for these are are published 
in the [IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html). These values should be 
interpolated for the time of observation, but should NOT be corrected for libration and ocean tides 
(`novas_make_frame()` will apply such corrections as appropriate for full accuracy frames). The Earth orientation 
parameters (EOP) are needed only when converting positions from the celestial CIRS (or TOD) frame to the Earth-fixed 
ITRS (or PEF) frames. You may ignore these and set zeroes if not interested in Earth-fixed calculations or if 
sub-arcsecond precision is not required. 

The advantage of using the observing frame, is that it enables very fast position calculations for multiple objects
in that frame (see the [benchmarks](#benchmarks)), since all sources in a frame have well-defined, fixed, topological 
positions on the celestial sphere. It is only a matter of expressing these positions as coordinates (and velocities) 
in a particular coordinate system. So, if you need to calculate positions for thousands of sources for the same 
observer and time, it will be significantly faster than using the low-level NOVAS C routines instead. You can create 
derivative frames for different observer locations, if need be, via `novas_change_observer()`.

> [!IMPORTANT]
> Without a proper ephemeris provider for the major planets, you are invariably restricted to working with 
> `NOVAS_REDUCED_ACCURACY` frames, providing milliarcsecond precision at most. Attempting to construct high-accuracy 
> frames without an appropriate high-precision ephemeris provider will result in an error from the requisite 
> `ephemeris()` calls.

> [!TIP]
> `NOVAS_FULL_ACCURACY` frames require a high-precision ephemeris provider for the major planets, e.g. to account 
> for the gravitational deflections. Without it, &mu;as accuracy cannot be ensured, in general. See section on 
> [Incorporating Solar-system ephemeris data or services](#solarsystem) further below.


<a name="apparent-place-c99"></a>
#### Calculate an apparent place on sky

Now we can calculate the apparent R.A. and declination for our source, which includes proper motion (for sidereal
sources) or light-time correction (for Solar-system bodies), and also aberration corrections for the moving observer 
and gravitational deflection around the major Solar System bodies (in full accuracy mode). You can calculate an 
apparent location in the coordinate system of choice (ICRS/GCRS, CIRS, J2000, MOD, TOD, TIRS, or ITRS) using
`novas_sky_pos()`. E.g.:

```c
 sky_pos apparent;    // Structure containing the precise observed position
  
 novas_sky_pos(&source, &obs_frame, NOVAS_CIRS, &apparent);
```

Apart from providing precise apparent R.A. and declination coordinates, the `sky_pos` structure also provides the 
_x,y,z_ unit vector pointing in the observed direction of the source (in the designated coordinate system). We also 
get radial velocity (for spectroscopy), and apparent distance for Solar-system bodies (e.g. for apparent-to-physical 
size conversion).

> [!NOTE]
> If you want geometric positions (and/or velocities) instead, without aberration and gravitational deflection, you 
> might use `novas_geom_posvel()`. 

> [!TIP]
> Regardless, which reference system you have used in the calculations above, you can always easily and efficiently 
> change the coordinate reference system in which your results are expressed, by creating an appropriate transform 
> via `novas_make_transform()` and then using `novas_transform_vector()` or `novas_transform_skypos()`. More on 
> [coordinate transforms](#transforms-c99) further below.


<a name="horizontal-place-c99"></a>
#### Calculate azimuth and elevation angles at the observing location

If your ultimate goal is to calculate the azimuth and elevation angles of the source at the specified observing 
location, you can proceed from the `sky_pos` data you obtained above (in whichever coordinate system!):

```c
 double az, el;   // [deg] local azimuth and elevation angles to populate
  
 // Convert the apparent position in CIRS on sky to horizontal coordinates
 novas_app_to_hor(&obs_frame, NOVAS_CIRS, apparent.ra, apparent.dec, novas_standard_refraction, 
   &az, &el);
```

Above we converted the apparent coordinates, that were calculated in CIRS, to refracted azimuth and elevation 
coordinates at the observing location, using the `novas_standard_refraction()` function to provide a suitable 
refraction correction. We could have used `novas_optical_refraction()` instead to use the weather data embedded in the 
frame's `observer` structure, or some user-defined refraction model, or else `NULL` to calculate unrefracted elevation 
angles.

<a name="reverse-place-c99"></a>
#### Going in reverse...

Of course, __SuperNOVAS__ allows you to go in reverse, for example from an observed Az/El position all the way to
proper ICRS R.A./Dec coordinates.

E.g.:

```c
  double az = ..., el = ...; // [deg] measured azimuth and elevation angles
  double pos[3];             // [arb. u.] xyz position vector
  double ra, dec;            // [h, deg] R.A. and declination to populate
  
  // Calculate the observer's apparent coordinates from the observed Az/El values,
  // lets say in CIRS (but it could also be ICRS, for all that matters). 
  novas_hor_to_app(&obs_frame, az, el, novas_standard_refraction, NOVAS_CIRS, &ra, &dec);
  
  // Convert apparent to ICRS geometric positions (no parallax)
  novas_app_to_geom(&obs_frame, NOVAS_CIRS, ra, dec, 0.0, pos);
  
  // Convert ICRS rectangular equatorial to R.A. and Dec
  vector2radec(pos, &ra, &dec);
```

Voila! And, of course you might want the coordinates in some other reference systems, such as B1950. For that you can 
simply add a transformation before `vector2radec()` above, e.g. as:

```c
  ...
  // Transform position from ICRS to B1950
  gcrs_to_mod(NOVAS_JD_B1950, pos, pos);

  // Convert B1950 xyz position to R.A. and Dec
  vector2radec(pos, &ra, &dec);
```


<a name="rise-set-transit-c99"></a>
#### Calculate rise, set, and transit times

You may be interested to know when sources rise above or set below some specific elevation angle, or at what time they 
appear to transit at the observer location. __SuperNOVAS__ has routines to help you with that too.

Given that rise, set, or transit times are dependent on the day of observation, and observer location, they are 
effectively tied to an observer frame.

```c
 novas_frame frame = ...;  // Earth-based observer location and lower-bound time of interest.
 object source = ...;      // Source of interest

 // UTC-based Julian day *after* observer frame, when source rises above 30 degrees of elevation 
 // next, given a standard optical refraction model.
 double jd_rise = novas_rises_above(30.0, &source, &frame, novas_standard_refraction);

 // UTC-based Julian day *after* observer frame, of next source transit
 double jd_transit = novas_transit_time(&source, &frame);
 
 // UTC-based Julian day *after* observer frame, when source sets below 30 degrees of elevation 
 // next, not accounting for refraction.
 double jd_set = novas_sets_below(30.0, &source, &frame, NULL);
```

Note, that in the current implementation these calls are not well suited sources that are at or within the 
geostationary orbit, such as such as Low Earth Orbit satellites (LEOs), geostationary satellites (which never really 
rise, set, or transit), or some Near Earth Objects (NEOs), which will rise set multiple times per day. For the latter, 
the above calls may still return a valid time, only without the guarantee that it is the time of the first such event 
after the specified frame instant. A future implementation may address near-Earth orbits better, so stay tuned for 
updates.


<a name="solsys-example-c99"></a>
### Calculating positions for a Solar-system source

 - [Planets and/or ephemeris type objects](#ephemeris-sources-c99)
 - [Solar-system objects with Keplerian orbital parameters](#orbital-sources-c99)
 - [Approximate planet and Moon orbitals](#planet-orbitals-c99)

Solar-system sources work similarly to the above with a few important differences at the start.

<a name="ephemeris-sources-c99"></a>
#### Planets and/or ephemeris type objects

Historically, NOVAS divided Solar-system objects into two categories: (1) major planets (including also the Sun, the 
Moon, and the Solar-system Barycenter); and (2) 'ephemeris' type objects, which are all other Solar-system objects.
The main difference is the numbering convention. NOVAS major planets have definitive ID numbers (see 
`enum novas_planet`), whereas 'ephemeris' objects have user-defined IDs. They are also handled by two separate adapter 
functions (although __SuperNOVAS__ has the option of using the same ephemeris provider for both types of objects 
also).

Thus, instead of `make_cat_object()` you define your source as a planet or ephemeris type `object` with a name or ID 
number that is used by the ephemeris service you provided. For major planets you might want to use `make_planet()`, if 
they use a `novas_planet_provider` function to access ephemeris data with their NOVAS IDs, or else 
`make_ephem_object()` for more generic ephemeris handling via a user-provided `novas_ephem_provider`. E.g.:

```c
 object mars, ceres; // Hold data on solar-system bodies.
  
 // Mars will be handled by the planet provider function
 make_planet(NOVAS_MARS, &mars);
  
 // Ceres will be handled by the generic ephemeris provider function, which let's say 
 // uses the NAIF ID of 2000001 _or_ the name 'Ceres' (depending on the implementation)
 make_ephem_object("Ceres", 2000001, &ceres);
```

> [!IMPORTANT] 
> Before you can handle all major planets and other ephemeris objects this way, you will have to provide one or more 
> functions to obtain the barycentric ICRS positions for your Solar-system source(s) of interest for the specific 
> Barycentric Dynamical Time (TDB) of observation. See section on 
> [Incorporating Solar-system ephemeris data or services](#solarsystem). 

And then, it's the same spiel as before, e.g.:

```c
 int status = novas_sky_pos(&mars, &obs_frame, NOVAS_TOD, &apparent);
 if(status) {
   // Oops, something went wrong...
   ...
 }
```

<a name="orbital-sources-c99"></a>
#### Solar-system objects with Keplerian orbital parameters

As of version __1.2__ you can also define solar system sources with Keplerian orbital elements (such as the most 
up-to-date ones provided by the [Minor Planet Center](https://minorplanetcenter.net/data) for asteroids, comets, 
etc.):

```c
 object NEA;		// e.g. a Near-Earth Asteroid
  
 // Fill in the orbital parameters (pay attention to units!)
 novas_orbital orbit = NOVAS_ORBIT_INIT;
 orbit.a = ...;	        // Major axis in AU...
 ...                    // ... and the rest of the orbital elements
  
 // Create an object for that orbit
 make_orbital_object("NEAxxx", -1, &orbit, &NEA);
```

> [!NOTE]
> Even with orbital elements, you will, in general, still require an ephemeris provider also, to obtain precise 
> positions for the Sun, an Earth-based observer, or the planet, around which the orbit is defined.


<a name="planet-orbitals-c99"></a>
#### Approximate planet and Moon orbitals


Finally, as of version __1.4__, you might generate approximate (arcmin-level) orbitals for the major planets (but not 
Earth!), the Moon, and the Earth-Moon Barycenter (EMB) also. E.g.:

```c
 double jd_tdb = ... // Time (epoch) for which to calculate orbital parameters  
  
 // Planet orbitals, e.g. for Mars
 novas_orbital mars_orbit = NOVAS_ORBIT_INIT;
 novas_make_planet_orbit(NOVAS_MARS, jd_tdb, &mars_orbit);
  
 // Moon's orbital around Earth
 novas_orbital moon_orbit = NOVAS_ORBIT_INIT;
 novas_make_moon_orbit(jd_tdb, &moon_orbit);
```

While the planet and Moon orbitals are not suitable for precision applications, they can be useful for determining 
approximate positions (e.g. via the `novas_approx_heliocentric()` and `novas_approx_sky_pos()` functions), and for 
rise/set time calculations.


<a name="transforms-c99"></a>
### Coordinate and velocity transforms (change of coordinate system)

__SuperNOVAS__ introduces matrix transforms (correctly since version __1.4__), which can take a position or velocity 
vector (geometric or apparent), obtained for an observer frame, from one coordinate system to another efficiently.
E.g.:

```c
  novas_frame frame = ...       // The observer frame (time and location)
  double j2000_vec[3] = ...;    // IN: original position vector, say in J2000.
  double tirs_vec[3] = {0.0};   // OUT: equivalent vector in TIRS we want to obtain
  
  novas_transform T;	        // Coordinate transformation data
  
  // Calculate the transformation matrix from J2000 to TIRS in the given observer frame.
  novas_make_transform(&frame, NOVAS_J2000, NOVAS_TIRS, &T);
  
  // Transform the J2000 position or velocity vector to TIRS...
  novas_transform_vector(j2000_vec, &T, tirs_vec);
```

Transformations support all __SupeNOVAS__ reference systems, that is ICRS/GCRS, J2000, TOD, MOD, CIRS, TIRS, and ITRS. 
The same transform can also be used to convert apparent positions in a `sky_pos` structure also, e.g.:

```c
  ...
  
  sky_pos j2000_pos = ...        // IN: in J2000, e.g. via novas_sky_pos()...
  sky_pos tirs_pos;              // OUT: equivalent TIRS position to calculate...
  
  // Transform the J2000 apparent positions to TIRS....
  novas_transform_sky_pos(&j2000_pos, &T, &tirs_pos);
```

-----------------------------------------------------------------------------

<a name="tips-c99"></a>
## Tips and tricks (C99)

 - [Multi-threaded calculations](#multi-threading-c99)
 - [Physical units](#physical-units-c99)
 - [String times and angles](#string-times-and-angles-c99)
 - [String dates](#string-dates-c99)

<a name="multi-threading-c99"></a>
### Multi-threaded calculations

Some of the calculations involved can be expensive from a computational perspective. For the most typical use case
however, NOVAS (and __SuperNOVAS__) has a trick up its sleeve: it caches the last result of intensive calculations so 
they may be re-used if the call is made with the same environmental parameters again (such as JD time and accuracy).
 
A direct consequence of the caching of results is that calculations are generally not thread-safe as implemented by 
the original NOVAS C 3.1 library. One thread may be in the process of returning cached values for one set of input 
parameters while, at the same time, another thread is saving cached values for a different set of parameters. Thus, 
when running calculations in more than one thread, the NOVAS C results returned may at times be incorrect, or more 
precisely they may not correspond to the requested input parameters.
 
While you should never call the original NOVAS C library from multiple threads simultaneously, __SuperNOVAS__ caches 
the results in thread local variables (provided your compiler supports it), and is therefore generally safe to use in 
multi-threaded applications. Just make sure that you:

 - use a compiler which supports the C11 language standard;
 - or, compile with GCC &gt;= 3.3;
 - or else, set the appropriate non-standard keyword to use for declaring thread-local variables for your compiler in 
   `config.mk` or in your equivalent build setup.
 

<a name="physical-units-c99"></a>
### Physical units

The NOVAS API has been using conventional units (e.g. AU, km, day, deg, h) typically for its parameters and return 
values alike. Hence, __SuperNOVAS__ follows the same conventions for its added functions and data structures also. 
However, when interfacing __SuperNOVAS__ with other programs, libraries, or data files, it is often necessary to use
quantities that are expressed in different units, such as SI or CGS. To facilitate such conversions, `novas.h` 
provides a set of unit constants, which can be used for converting to/from SI units (and radians). 

For example, `novas.h` contains the following definitions:

```c
 /// [s] The length of a synodic day, that is 24 hours exactly. @since 1.2
 #define NOVAS_DAY                 86400.0

 /// [rad] A degree expressed in radians. @since 1.2
 #define NOVAS_DEGREE              (M_PI / 180.0)

 /// [rad] An hour of angle expressed in radians. @since 1.2
 #define NOVAS_HOURANGLE           (M_PI / 12.0)
```

You can use these, for example, to convert quantities expressed in conventional units for NOVAS to standard (SI) 
values, by multiplying NOVAS quantities with the corresponding unit definition. E.g.:

```c
 // A difference in Julian Dates [day] in seconds.
 double delta_t = (tjd - tjd0) * NOVAS_DAY;
  
 // R.A. [h] / declination [deg] converted radians (e.g. for trigonometric functions).
 double ra_rad = ra_h * NOVAS_HOURANGLE;
 double dec_rad = dec_d * NOVAS_DEGREE; 
```

And vice-versa: to convert values expressed in standard (SI) units, you can divide by the appropriate constant to
'cast' an SI value into the particular physical unit, e.g.:

```c
 // Increment a Julian Date [day] with some time differential [s].
 double tjd = tjd0 + delta_t / NOVAS_DAY;
  
 // convert R.A. / declination in radians to hours and degrees
 double ra_h = ra_rad / NOVAS_HOURANGLE;
 double dec_d = dec_rad / NOVAS_DEGREE;
```

Finally, you can combine them to convert between two different conventional units, e.g.:

```c
 // Convert angle from [h] -> [rad] -> [deg]
 double lst_d = lst_h * NOVAS_HOURANGLE / NOVAS_DEGREE; 
  
 // Convert [AU/day] -> [m/s] (SI) -> [km/s]
 double v_kms = v_auday * (NOVAS_AU / NOVAS_DAY) / NOVAS_KMS
```


<a name="string-times-and-angles-c99"></a>
### String times and angles

__SuperNOVAS__ functions typically input and output times and angles as decimal values (hours and degrees, but also as 
days and hour-angles), but that is not how these are represented in many cases. Time and right-ascention are often 
given as string values indicating hours, minutes, and seconds (e.g. "11:32:31.595", or "11h 32m 31.595s"). Similarly 
angles, are commonly represented as degrees, arc-minutes, and arc-seconds (e.g. "+53 60 19.9"). For that reason, 
__SuperNOVAS__ provides a set of functions to convert string values expressed in decimal or broken-down format to floating
point representation. E.g.,

```c
 // Right ascention from string
 double ra_h = novas_str_hours("9 18 49.068");

 // Declination from string
 double dec_d = novas_str_degrees("-53 10 07.33");
```

The conversions have a lot of flexibility. Components can be separated by spaces (as above), by colons, commas, or
underscores, by the letters 'h'/'d', 'm', and 's', by single (minutes) and double quotes (seconds), or any combination 
thereof. Decimal values may be followed by 'h' or 'd' unit markers. Additionally, angles can end with a compass 
direction, such as 'N', 'E', 'S' or 'W'. So the above could also have been:

```c
 double ra_h = novas_str_hours("9h_18:49.068\"");
 double dec_d = novas_str_degrees("53d 10'_07.33S");
```

or as decimals:

```c
 double ra_h = novas_str_hours("9.31363");
 double dec_d = novas_str_degrees("53.16870278d South");
```

<a name="string-dates-c99"></a>
### String dates

Dates are typically represented broken down into year, month, and day (e.g. "2025-02-16", or "16.02.2025", or 
"2/16/2025"), with or without a time marker, which itself may or may not include a time zone specification. In 
astronomy, the most commonly used string representation of dates is with ISO 8601 timestamps. The following are
all valid ISO date specifications:

```
 2025-02-16			# Date only (0 UTC)
 2025-02-16T19:35:21Z		# UTC date/time
 2025-02-16T19:35:21.832Z	# UTC date/time with decimals
 2025-02-16T14:35:21+0500	# date in time zone (e.g. EST)
 2025-02-16T14:35:21.832+05:00  # alternative time zone specification
```

__SuperNOVAS__ provides functions to convert between ISO dates/times and their string representation for convenience. 
E.g.,

```c
 novas_timespec time;           // Astronomical time specification
 char timestamp[40];            // A string to contain an ISO representation

 // Parse an ISO timestamp into a Julian day (w/o returning the tail).
 double jd = novas_parse_iso_date("2025-02-16T19:35:21Z", NULL);
 if(isnan(jd)) {
   // Oops could not parse date.
   ...
 }
   
 // Use the parsed JD date (in UTC) with the appropriate leap seconds 
 // and UT1-UTC time difference
 novas_set_time(NOVAS_UTC, jd, leap_seconds, dut1, &time);
  
 // Print an ISO timestamp, with millisecond precision, into the 
 // designated string buffer.
 novas_iso_timestamp(&time, timestamp, sizeof(timestamp));
```

> [!NOTE]
> ISO 8601 timestamps are always UTC-based and expressed in the Gregorian calendar, as per specification, even for 
> dates that preceded the Gregorian calendar reform of 1582 (i.e. 'proleptic Gregorian' dates).

Other __SuperNOVAS__ string date functions will process dates in the astronomical calendar of date by default, that is 
in the Gregorian calendar after the Gregorian calendar reform of 1582, or the Julian/Roman calendar for dates prior, 
and support timescales other than UTC also. E.g.:

```c
 // Print a TDB timestamp in the astronomical calendar of date instead
 novas_timestamp(&time, NOVAS_TDB, timestamp, sizeof(timestamp));
```

Or, parse an astronomical date:

```c
 // Parse astronomical dates into a Julian day...
 double jd = novas_date("2025-02-16T19:35:21");
 if(isnan(jd)) {
   // Oops could not parse date.
   ...
 }
```

Or, parse an astronomical date, including the timescale specification:

```c
 // Parse a TAI-based timestamp into a Julian day and corresponding timescale
 double jd = novas_date_scale("2025-02-16T19:35:21+0200 TAI", &scale);
 if(isnan(jd)) {
   // Oops could not parse date.
   ...
 }
```

Sometimes your input dates are represented in various other formats. You can have additional flexibility for parsing 
dates using the `novas_parse_date_format()` and `novas_timescale_for_string()` functions.

<details>

E.g.,

```c
 char *pos = NULL;            // We'll keep track of the string parse position here
 enum novas_timescale scale;  // We'll parse the timescale here (if we can)

 // Parse the M/D/Y date up to the 'TAI' timescale specification...
 double jd = novas_parse_date_format(NOVAS_GREGORIAN_CALENDAR, NOVAS_MDY, 
   "2/16/2025 20:08:49.082 TAI", &pos);
  
 // Then parse the 'TAI' timescale marker, after the date/time specification
 scale = novas_timescale_for_string(pos);
 if(scale < 0) {
   // Oops, not a valid timescale marker. Perhaps assume UTC...
   scale = NOVAS_UTC;
 }

 // Now set the time for the given calendar, date format, and timescale of the 
 // string representation.
 novas_set_time(scale, jd, leap_seconds, dut1, &time);
``` 

</details>

-----------------------------------------------------------------------------
Copyright (C) 2026 Attila Kovács

