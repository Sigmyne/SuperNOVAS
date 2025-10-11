/**
 * @file
 *
 *  SuperNOVAS types, definitions, and function prototypes.
 *
 *  SuperNOVAS astrometry software based on the Naval Observatory Vector Astrometry Software (NOVAS).
 *  It has been modified to fix outstanding issues, make it easier to use, and provide a ton of new
 *  features.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">
 *  http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 * @author G. Kaplan and Attila Kovacs
 * @version 1.5.0
 */

#ifndef _NOVAS_
#define _NOVAS_

#include <math.h>   // for sin, cos
#include <stdlib.h> // NULL
#include <stdint.h>
#include <time.h>

/// \cond PRIVATE
#if __STDC_VERSION__ < 199901L
#  define restrict                        ///< No 'restrict' keyword prior to C99
#endif
/// \endcond

/// Definition of &pi; in case it's not defined in math.h
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

// The upstream NOVAS library had a set of include statements that really were not necessary
// First, including standard libraries here meant that those libraries were included in the
// source code of any application that included 'novas.h', even if that source code did not
// contain any reference to those libraries. This is just unnecessary bloating, and simply a bad
// practice
//
// It also included "novascon.h", which defined various constants -- mainly for internal
// use by the library itself. This was problematic for two reasons. First, because the constants
// were declared as 'const' variables, rather than precompiler constants, which meant (a) that they
// needed to be defined in an actual source code "novascon.c"; a minor inconvenience, and (b)
// they could not be undefined in case the application needed something else locally with
// the same name. The rather non unique naming of constants such as 'C', 'F', 'T0', or 'AU'
// exacerbated the chance of namespace conflicts when compiled or linked against the user's
// application code, which may have defined their own constants or variables by the same name.
//
// For these reasons, we removed the unneeded '#include' directives by default. If you need them
// for compiling your code, which may have relied on these, you can add '-DCOMPAT' to the
// compiler options
//
#ifdef COMPAT
#  include <stdio.h>
#  include <ctype.h>
#  include <string.h>

#  include "novascon.h"
#endif


/// API major version
#define SUPERNOVAS_MAJOR_VERSION  1

/// API minor version
#define SUPERNOVAS_MINOR_VERSION  6

/// Integer sub version of the release
#define SUPERNOVAS_PATCHLEVEL     0

/// Additional release information in version, e.g. "-1", or "-rc1", or empty string "" for releases.
#define SUPERNOVAS_RELEASE_STRING "-devel"

/// \cond PRIVATE

#ifdef str_2
#  undef str_2
#endif

/// Stringify level 2 macro
#define str_2(s) str_1(s)

#ifdef str_1
#  undef str_1
#endif

/// Stringify level 1 macro
#define str_1(s) #s

/// \endcond

/// The version string for this library
/// \hideinitializer
#define SUPERNOVAS_VERSION_STRING str_2(SUPERNOVAS_MAJOR_VERSION) "." str_2(SUPERNOVAS_MINOR_VERSION) \
        "." str_2(SUPERNOVAS_PATCHLEVEL) SUPERNOVAS_RELEASE_STRING

#define NOVAS_MAJOR_VERSION       3       ///< Major version of NOVAS on which this library is based
#define NOVAS_MINOR_VERSION       1       ///< Minor version of NOVAS on which this library is based

/// The version string of the upstream NOVAS library on which this library is based.
#define NOVAS_VERSION_STRING      str_2(NOVAS_MAJOR_VERSION) "." str_2(NOVAS_MINOR_VERSION)

#undef str_1
#undef str_2

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif

// Doxygen topical sections (ordered)

/**
 * \defgroup source           Astronomical object of interest
 *
 * \defgroup observer         Observer location
 *
 * \defgroup time             Time of observation and astronomical timescales
 *
 * \defgroup frame            Observing frames
 *
 * \defgroup apparent         Apparent equatorial positions on sky
 *
 * \defgroup geometric        Geometric equatorial positions and velocities
 *
 * \defgroup equatorial       Transforming between equatorial systems
 *
 * \defgroup nonequatorial    Non-equatorial coordinates
 *
 * \defgroup refract          Atmospheric refraction
 *
 * \defgroup spectral         Spectroscopic applications
 *
 * \defgroup solar-system     Solar-system ephemeris providers
 *
 * \defgroup earth            Earth orientation
 *
 * \defgroup util             Helpers and utilities
 */



/// [day] Julian date at J2000
/// @ingroup time
#define NOVAS_JD_J2000            2451545.0

/// [day] Julian date at which the Modified Julian Date (MJD) is zero
/// @ingroup time
#define NOVAS_JD_MJD0             2400000.5

/// [day] Julian date at B1950 (NASA / NAIF SPICE definition)
/// precession(), transform_cat()
/// @ingroup time
#define NOVAS_JD_B1950            2433282.42345905

/// [day] Julian date at B1900 (NASA / NAIF SPICE definition)
/// precession(), transform_cat()
/// @ingroup time
#define NOVAS_JD_B1900            2415020.31352

/// [day] Julian date for J1991.25, which the Hipparcos catalog is referred to.
/// @sa precession(), transform_cat()
/// @ingroup time
#define NOVAS_JD_HIP              2448349.0625


/// [m/s] Speed of light in meters/second is a defining physical constant.
/// @c_util
#define NOVAS_C                   299792458.0

/// [s] The length of a synodic day, that is 24 hours exactly.
/// @since 1.2
/// @ingroup time
#define NOVAS_DAY                 86400.0

/// [day] The length of a tropical year (at J2000) in days.
/// @since 1.5
/// @ingroup time
#define NOVAS_TROPICAL_YEAR_DAYS  365.2421897

/// [day] The length of a Julian year (at J2000) in days.
/// @since 1.5
/// @ingroup time
#define NOVAS_JULIAN_YEAR_DAYS    365.25

/// [day] The length of a Besselian year in days.
/// @since 1.5
/// @c_time
#define NOVAS_BESSELIAN_YEAR_DAYS 365.242198781

/// [rad] A degree expressed in radians.
/// @since 1.2
/// @ingroup util
#define NOVAS_DEGREE              (M_PI / 180.0)

/// [rad] An arc minute expressed in radians.
/// @since 1.2
/// @ingroup util
#define NOVAS_ARCMIN              (NOVAS_DEGREE / 60.0)

/// [rad] An arc second expressed in radians.
/// @since 1.2
/// @ingroup util
#define NOVAS_ARCSEC              (NOVAS_ARCMIN / 60.0)

/// [rad] An hour of angle expressed in radians.
/// @since 1.2
/// @ingroup util
#define NOVAS_HOURANGLE           (M_PI / 12.0)

/// [m] A kilometer (km) in meters.
/// @since 1.2
/// @ingroup util
#define NOVAS_KM                  1000.0

/// [m] One km/s in m/s.
/// @since 1.3
/// @ingroup source
#define NOVAS_KMS                 (NOVAS_KM)

/// [m] Astronomical unit (AU). IAU definition.
/// See <a href="https://www.iau.org/static/resolutions/IAU2012_English.pdf">IAU 2012 Resolution B2</a>.
/// @sa DE405_AU
/// @ingroup source
#define NOVAS_AU                  1.495978707e+11

/// [m] Astronomical unit (AU). based on DE-405. (old definition)
/// @sa NOVAS_AU
/// @ingroup solar-system
#define DE405_AU                  1.4959787069098932e+11

/// [s] Light-time for one astronomical unit (AU) in seconds.
#define NOVAS_AU_SEC              ( NOVAS_AU / NOVAS_C )

/// [AU/day] Speed of light in units of AU/day.
#define NOVAS_C_AU_PER_DAY        ( NOVAS_DAY / NOVAS_AU_SEC )

/// [km] Astronomical Unit (AU) in kilometers.
#define NOVAS_AU_KM               ( 1e-3 * NOVAS_AU )

/// [m] A light-year in meters.
/// @since 1.5
/// @ingroup source
#define NOVAS_LIGHT_YEAR          ( NOVAS_C * NOVAS_TROPICAL_YEAR_DAYS * NOVAS_DAY )

/// [m] A parsec in meters.
/// @since 1.5
/// @ingroup source
#define NOVAS_PARSEC              ( NOVAS_AU / NOVAS_ARCSEC )

/// [m] Default distance at which sidereal sources are assumed when not specified otherwise
/// Historically NOVAS placed sources with no parallax at 1 Gpc distance, and so we follow.
/// @since 1.5
/// @ingroup source
#define NOVAS_DEFAULT_DISTANCE    (1e9 * NOVAS_PARSEC)

/// [m<sup>3</sup>/s<sup>2</sup>] Heliocentric gravitational constant (GM<sub>sun</sub>) from DE440,
/// see Park et al., AJ, 161, 105 (2021)
#define NOVAS_G_SUN               1.32712440041279419e20

/// [m<sup>3</sup>/s<sup>2</sup>] Geocentric gravitational constant (GM<sub>earth</sub>) from DE440,
/// see Park et al., AJ, 161, 105 (2021)
#define NOVAS_G_EARTH             3.98600435507e14

/// [m] Solar radius (photosphere)
/// @since 1.1
#define NOVAS_SOLAR_RADIUS        696340000.0

/// [W/m<sup>2</sup>] The Solar Constant i.e., typical incident Solar power on Earth.
/// The value of 1367 Wm<sup>−2</sup> was adopted by the World Radiation Center
/// (Gueymard, 2004).
/// @since 1.3
/// @sa novas_solar_power()
#define NOVAS_SOLAR_CONSTANT      1367.0

/// [m] Equatorial radius of Earth in meters from IERS Conventions (2003).
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_IERS_EARTH_RADIUS        6378136.6

/// [m] Earth ellipsoid flattening from IERS Conventions (2003). Value is 1 / 298.25642.
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_IERS_EARTH_FLATTENING    (1.0 / 298.25642)

/// [m] Equatorial radius of the WGS84 reference ellipsoid.
/// @since 1.5
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_WGS84_RADIUS        6378137.0

/// [m] WGS84 Earth flattening
/// @since 1.5
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_WGS84_FLATTENING    (1.0 / 298.257223563)

/// [m] Equatorial radius of the WGS84 reference ellipsoid.
/// @since 1.5
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_GRS80_RADIUS        6378137.0

/// [m] WGS84 Earth flattening
/// @since 1.5
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_GRS80_FLATTENING    (1.0 / 298.257222101)

/// [m] Equatorial radius of Earth (ITRF / GRS80 model)
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_EARTH_RADIUS        NOVAS_GRS80_RADIUS

/// [m] Earth ellipsoid flattening (ITRF / GRS80_MODEL)
/// @sa novas_geodetic_to_cartesian(), novas_cartesian_to_geodetic()
/// @ingroup observer
#define NOVAS_EARTH_FLATTENING    NOVAS_GRS80_FLATTENING

/// [rad/s] Rotational angular velocity of Earth from IERS Conventions (2003).
/// @ingroup earth
#define NOVAS_EARTH_ANGVEL        7.2921150e-5

/// [s] TAI - GPS time offset
/// @ingroup time
#define NOVAS_GPS_TO_TAI          19.0

/// [s] TT - TAI time offset
/// @ingroup time
#define NOVAS_TAI_TO_TT           32.184

/// [day] The Julian day the Gregorian calendar was introduced in 15 October 1582.
/// The day prior to that was 4 October 1582 in the Julian Calendar.
/// @ingroup time
#define NOVAS_JD_START_GREGORIAN  2299160.5


/// The ICRS system as a string
/// @since 1.3
/// @sa novas_set_catalog(), make_cat_object_sys(), make_redshifted_object_sys()
/// @c_source
#define NOVAS_SYSTEM_ICRS          "ICRS"

/// The B1950 coordiante system as a string
/// @since 1.3
/// @sa novas_set_catalog(), make_cat_object_sys(), make_redshifted_object_sys()
/// @c_source
#define NOVAS_SYSTEM_B1950         "B1950"

/// The J2000 coordinate syste, as a string
/// @since 1.3
/// @sa novas_set_catalog(), make_cat_object_sys(), make_redshifted_object_sys()
/// @c_source
#define NOVAS_SYSTEM_J2000         "J2000"

/// The 4th catalog of fundamental stars (FK4) coordinate system as a string
/// @since 1.3
/// @sa novas_set_catalog(), make_cat_object_sys()
/// @c_source
#define NOVAS_SYSTEM_FK4           "FK4"

/// The 5th catalog of fundamental stars (FK5) coordinate system as a string
/// @since 1.3
/// @sa novas_set_catalog(), make_cat_object_sys()
/// @c_source
#define NOVAS_SYSTEM_FK5           "FK5"

/// The 6th catalog of fundamental stars (FK6) coordinate system as a string
/// @since 1.5
/// @sa novas_set_catalog(), make_cat_object_sys()
/// @c_source
#define NOVAS_SYSTEM_FK6           "FK6"

/// The Hipparcos dataset coordinate system as a string
/// @since 1.3
/// @sa novas_set_catalog(), make_cat_object_sys()
/// @c_source
#define NOVAS_SYSTEM_HIP           "HIP"

/// [&mu;m] Default wavelength, e.g. for wavelength-dependent refraction models. It is set to the
/// median wavelength of visible light.
/// @since 1.4
/// @sa novas_refract_wavelength()
/// @ingroup refract
#define NOVAS_DEFAULT_WAVELENGTH      0.55

#ifndef COMPAT
// If we are not in the strict compatibility mode, where constants are defined
// as variables in novascon.h (with implementation in novascon.c), then define
// them here

#  ifndef TWOPI
#    define TWOPI                 (2.0 * M_PI)    ///< 2&pi;
#  endif

#  ifndef ASEC360
/// [arcsec] Number of arcseconds in 360 degrees.
#    define ASEC360               (360 * 60 * 60)
#  endif

#  ifndef DEG2RAD
/// [rad/deg] 1 degree in radians
#    define DEG2RAD               (M_PI / 180.0)
#  endif

#  ifndef RAD2DEG
/// [deg/rad] 1 radian in degrees
#    define RAD2DEG               (1.0 / DEG2RAD)
#  endif

#  ifndef ASEC2RAD
/// [rad/arcsec] 1 arcsecond in radians
#    define ASEC2RAD              (DEG2RAD / 3600.0)
#  endif

#endif

#ifndef _EXCLUDE_DEPRECATED

/**
 * Deprecated.
 * @deprecated Old definition of the Barycenter origin. @ref NOVAS_BARYCENTER is preferred. */
#  define BARYC                     NOVAS_BARYCENTER

/**
 * Deprecated.
 * @deprecated Old definition of the Center of Sun as the origin. @ref NOVAS_HELIOCENTER is preferred. */
#  define HELIOC                    NOVAS_HELIOCENTER

#endif // _EXCLUDE_DEPRECATED

/**
 * Settings for 'novas_debug()'
 *
 * @sa novas_debug()
 * @ingroup util
 */
enum novas_debug_mode {
  NOVAS_DEBUG_OFF = 0,     ///< Do not print errors and traces to the standard error (default).
  NOVAS_DEBUG_ON,          ///< Print errors and traces to the standard error.
  NOVAS_DEBUG_EXTRA        ///< Print all errors and traces to the standard error, even if they may be acceptable behavior.
};



/**
 * The type of astronomical objects distinguied by the NOVAS library.
 *
 * @sa object, NOVAS_OBJECT_TYPES
 * @c_source
 */
enum novas_object_type {
  /// A major planet, or else the Sun, the Moon, or the Solar-System Barycenter (SSB).
  /// @sa make_planet(), enum novas_planet, novas_planet_provider, novas_planet_provider_hp
  NOVAS_PLANET = 0,

  /// A Solar-system body that does not fit the major planet type, and requires specific
  /// user-provided novas_ephem_provider implementation.
  /// @sa make_ephem_object(), novas_ephem_provider
  NOVAS_EPHEM_OBJECT,

  /// Any non-solar system object that may be handled via 'catalog' coordinates, such as a star
  /// or a quasar.
  /// @sa make_cat_object_sys(), make_redshifted_object_sys(), cat_entry
  NOVAS_CATALOG_OBJECT,

  /// Any Solar-system body, whose position is determined by a set of orbital elements
  /// @since 1.2
  /// @sa make_orbital_object(), novas_orbital
  NOVAS_ORBITAL_OBJECT
};

/**
 * The number of object types distinguished by NOVAS.
 *
 * @hideinitializer
 * @sa enum novas_object_type
 */
#define NOVAS_OBJECT_TYPES      (NOVAS_ORBITAL_OBJECT + 1)

/**
 * Enumeration for the 'major planet' numbers in NOVAS to use as the solar-system body number
 * whenever the object type is NOVAS_PLANET.
 *
 * @sa make_planet(), NOVAS_PLANET, NOVAS_PLANETS, NOVAS_PLANET_NAMES_INIT,
 *     NOVAS_PLANET_RADII_INIT
 *
 * @c_source
 */
enum novas_planet {
  NOVAS_SSB = 0,          ///< Solar-system barycenter position ID
  NOVAS_MERCURY,          ///< Major planet number for Mercury in NOVAS.
  NOVAS_VENUS,            ///< Major planet number for Venus in NOVAS.
  NOVAS_EARTH,            ///< Major planet number for Earth in NOVAS.
  NOVAS_MARS,             ///< Major planet number for Mars in NOVAS.
  NOVAS_JUPITER,          ///< Major planet number for Jupiter in NOVAS.
  NOVAS_SATURN,           ///< Major planet number for Saturn in NOVAS.
  NOVAS_URANUS,           ///< Major planet number for Uranus in NOVAS.
  NOVAS_NEPTUNE,          ///< Major planet number for Neptune in NOVAS.
  NOVAS_PLUTO,            ///< Major planet number for Pluto in NOVAS.
  NOVAS_SUN,              ///< Numerical ID for the Sun in NOVAS.
  NOVAS_MOON,             ///< Numerical ID for the Moon in NOVAS.
  NOVAS_EMB,              ///< NOVAS ID for the Earth-Moon Barycenter (EMB). @since 1.2
  NOVAS_PLUTO_BARYCENTER  ///< NOVAS ID for the barycenter of the Pluto System. @since 1.2
};

/**
 * The number of major planets defined in NOVAS.
 *
 * @hideinitializer
 * @sa enum novas_planet
 */
#define NOVAS_PLANETS             (NOVAS_PLUTO_BARYCENTER + 1)

/**
 * String array initializer for Major planet names, matching the enum novas_planet. E.g.
 *
 * ```c
 * const char *planet_names[] = NOVAS_PLANET_NAMES_INIT;
 * ```
 *
 * @hideinitializer
 * @since 1.2
 * @author Attila Kovacs
 *
 * @sa enum novas_planet
 * @ingroup source
 */
#define NOVAS_PLANET_NAMES_INIT { \
        "SSB", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", \
        "Sun", "Moon", "EMB", "Pluto-Barycenter" }

/**
 * Array initializer for mean planet radii in meters, matching the enum novas_planet. E.g.
 *
 * ```c
 * const double *planet_radii[] = NOVAS_PLANET_RADII_INIT;
 * ```
 *
 * REFERENCES:
 * <ol>
 * <li>https://orbital-mechanics.space/reference/planetary-parameters.html, Table 3.</li>
 * <li>B. A. Archinal, et al.,
 * Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements:
 * 2015. Celestial Mechanics and Dynamical Astronomy, 130(3):22, March 2018.
 * doi:10.1007/s10569-017-9805-5.</li>
 * <li>B. A. Archinal, et al.,
 * Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements:
 * 2015. Celestial Mechanics and Dynamical Astronomy, 130(3):22, March 2018.
 * doi:10.1007/s10569-017-9805-5.
 * </ol>
 *
 * @hideinitializer
 * @since 1.5
 * @author Attila Kovacs
 *
 * @sa enum novas_planet, NOVAS_PLANET_NAMES_INIT, NOVAS_RMASS_INIT
 */
#define NOVAS_PLANET_RADII_INIT { \
        0.0, \
        2440530.0, 6051800.0, 6378136.6, 3396190.0, 71492000.0, 60268000.0, 25559000.0, 24764000.0, 1188300.0, \
        695700000.0, \
        1737400.0, \
        0.0, 0.0 \
}

/**
 * Reciprocal masses of solar system bodies, from DE-405 (Sun mass / body mass).
 * [0]: Earth/Moon barycenter (legacy from NOVAS C), MASS[1] = Mercury, ...,
 * [9]: Pluto (barycenter), [10]: Sun, [11]: Moon.
 * Barycentric reciprocal masses (index 12, 13) are not set.
 *
 * NOTES:
 * <ol>
 * <li>The values have been updated to used DE440 data (Park et al. 2021) in 1.4.
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Ryan S. Park et al 2021 AJ 161 105, DOI 10.3847/1538-3881/abd414</li>
 * <li>IERS Conventions, Chapter 3, Table 3.1</li>
 * </ol>
 *
 * @hideinitializer
 * @since 1.2
 * @author Attila Kovacs
 *
 * @sa enum novas_planet, NOVAS_PLANET_NAMES_INIT, NOVAS_PLANET_RAII_INIT, NOVAS_PLANETS_GRAV_Z_INIT
 */
#define NOVAS_RMASS_INIT  { \
        328900.559708565, \
        6023657.9450387, 408523.718656268, 332946.048773067, 3098703.54671961, \
        1047.348631244, 3497.9018007932, 22902.9507834766, 19412.2597758766, 136045556.16738, \
        1.0, 27068702.9548773, \
        0.0, 0.0 }

/**
 * Gravitational redshifts for major planets (and Moon and Sun) for light emitted at surface
 * and detected at a large distance away. Barycenters are not considered, and for Pluto the
 * redshift for the Pluto system is assumed for distant observers.
 *
 * @sa enum novas_planet
 * @sa NOVAS_PLANETS
 *
 * @hideinitializer
 * @since 1.1.1
 * @author Attila Kovacs
 */
#define NOVAS_PLANET_GRAV_Z_INIT { \
        0.0, 1.0047e-10, 5.9724e-10, 7.3050e-10, 1.4058e-10, 2.0166e-8, 7.2491e-9, 2.5420e-9, \
        3.0893e-9, 9.1338e-12, 2.120483e-6, 3.1397e-11, 0.0, 0.0 }

/**
 * Default set of gravitating bodies to use for deflection calculations in reduced accuracy mode.
 * (only apply gravitational deflection for the Sun.)
 *
 * @since 1.1
 * @author Attila Kovacs
 *
 * @sa grav_bodies_reduced_accuracy, grav_planets(), grav_undo_planets()
 * @ingroup apparent
 */
#define DEFAULT_GRAV_BODIES_REDUCED_ACCURACY   ( (1 << NOVAS_SUN) )

/**
 * Default set of gravitating bodies to use for deflection calculations in full accuracy mode.
 *
 * @since 1.1
 * @author Attila Kovacs
 *
 * @sa grav_bodies_full_accuracy, grav_planets(), grav_undo_planets()
 * @ingroup apparent
 */
#define DEFAULT_GRAV_BODIES_FULL_ACCURACY      ( DEFAULT_GRAV_BODIES_REDUCED_ACCURACY | (1 << NOVAS_JUPITER) | (1 << NOVAS_SATURN) )

/**
 * Current set of gravitating bodies to use for deflection calculations in reduced accuracy mode. Each
 * bit signifies whether a given body is to be accounted for as a gravitating body that bends light,
 * such as the bit `(1 << NOVAS_JUPITER)` indicates whether or not Jupiter is considered as a deflecting
 * body. You should also be sure that you provide ephemeris data for bodies that are designated for the
 * deflection calculation.
 *
 * @since 1.1
 *
 * @sa grav_def(), grav_planets(), DEFAULT_GRAV_BODIES_REDUCED_ACCURACY
 * @sa set_ephem_provider()
 * @ingroup apparent
 */
extern int grav_bodies_reduced_accuracy;

/**
 * Current set of gravitating bodies to use for deflection calculations in full accuracy mode. Each
 * bit signifies whether a given body is to be accounted for as a gravitating body that bends light,
 * such as the bit `(1 << NOVAS_JUPITER)` indicates whether or not Jupiter is considered as a deflecting
 * body. You should also be sure that you provide ephemeris data for bodies that are designated for the
 * deflection calculation.
 *
 * @since 1.1
 *
 * @sa grav_def(), grav_planets(), DEFAULT_GRAV_BODIES_FULL_ACCURACY
 * @sa set_ephem_provider_hp()
 * @ingroup apparent
 */
extern int grav_bodies_full_accuracy;


/**
 * Solar-system body IDs to use as object.number with ephemeris source types. JPL ephemerides
 * use <a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html">NAIF IDs</a>
 * to identify objects in the Solar-system, which is thus the most widely adopted convention for
 * numbering Solar-system bodies. But other numbering systems also exists, for example the
 * CALCEPH library uses its own convention for the numbering of asteroids.
 *
 * @cond C_API
 * @sa object, NOVAS_EPHEM_OBJECT, NOVAS_ID_TYPES
 * @endcond
 *
 * @author Attila Kovacs
 * @since 1.2
 * @ingroup solar-system
 */
enum novas_id_type {
  NOVAS_ID_NAIF = 0,  ///< If the ephemeris provider should use NAIF IDs
  NOVAS_ID_CALCEPH    ///< If the ephemeris provider should use CALCEPH IDs
};

 /**
  * Number of different Solar-system body ID types enumerated
  *
  * @hideinitializer
  * @author Attila Kovacs
  * @since 1.2
  *
  * @sa enum novas_id_type
  * @c_solar-system
  */
 #define NOVAS_ID_TYPES      (NOVAS_ID_CALCEPH + 1)

/**
 * Types of places on and around Earth that may serve a a reference position for the observation.
 *
 * @sa observer, NOVAS_OBSERVER_PLACES
 * @c_observer
 */
enum novas_observer_place {
  /// Calculate coordinates as if observing from the geocenter for location and Earth rotation
  /// independent coordinates.
  /// @sa make_observer_at_geocenter()
  NOVAS_OBSERVER_AT_GEOCENTER = 0,

  /// Stationary observer in the corotating frame of Earth.
  /// @sa make_gps_observer(), make_itrf_observer(), make_site_observer()
  NOVAS_OBSERVER_ON_EARTH,

  /// Observer is on Earth orbit, with a position and velocity vector relative to geocenter.
  /// This may also be appropriate for observatories at the L2 or other Earth-based Langrange
  /// points.
  /// @sa make_observer_in_space()
  NOVAS_OBSERVER_IN_EARTH_ORBIT,

  /// Observer airborne, moving relative to the surface of Earth.
  /// @since 1.1
  /// @sa make_airborne_observer()
  NOVAS_AIRBORNE_OBSERVER,

  /// Observer is orbiting the Sun.
  /// @since 1.1
  /// @sa make_solar_system_observer()
  NOVAS_SOLAR_SYSTEM_OBSERVER
};

/**
 * The number of observer place types supported
 *
 * @hideinitializer
 * @sa enum novas_observer_place
 */
#define NOVAS_OBSERVER_PLACES     (NOVAS_SOLAR_SYSTEM_OBSERVER + 1)

/**
 * The basic types of positional coordinate reference systems supported by NOVAS. These
 * determine only how the celestial pole is to be located, but not how velocities are to be
 * referenced. specific pos-vel coordinates are referenced to an 'astro_frame', which must
 * specify one of the values defined here.
 *
 * @sa novas_frame, NOVAS_REFERENCE_SYSTEMS
 * @sa novas_sky_pos(), novas_geom_posvel(), novas_geom_to_app(), novas_app_to_geom(), novas_app_to_hor(),
 *     novas_hor_to_app(), novas_make_transform()
 *
 * @c_equatorial
 */
enum novas_reference_system {
  /// Geocentric Celestial Reference system. Essentially the same as ICRS but includes
  /// aberration and gravitational deflection for an observer around Earth.
  NOVAS_GCRS = 0,

  /// True equinox Of Date: dynamical system of the 'true' equator, with its origin at the
  /// 'true' equinox (pre IAU 2006 system).
  NOVAS_TOD,

  /// Celestial Intermediate Reference System: dynamical system of the true equator, with its
  /// origin at the CIO (preferred since IAU 2006)
  NOVAS_CIRS,

  /// International Celestial Reference system. The equatorial system fixed to the frame of
  /// distant quasars.
  NOVAS_ICRS,

  /// The J2000 dynamical reference system
  /// @since 1.1
  NOVAS_J2000,

  /// Mean of date:  dynamical system of the 'mean' equator, with its origin at the
  /// 'mean' equinox (pre IAU 2006 system). It includes precession (Lieske et. al. 1977),
  /// but not nutation. For example, FK4 or B1950 are MOD coordinate systems.
  /// @since 1.1
  NOVAS_MOD,

  /// Terrestrial Intermediate Reference System. It is the IAU 2006 standard pseudo Earth-fixed
  /// (PEF) coordinate system, which co-rotates with Earth, but does not include Earth polar
  /// wobble corrections.
  /// @since 1.4
  NOVAS_TIRS,

  /// International Terrestrial Reference System. This is the IAU 2006 Earth-fixed reference
  /// system, and includes small measured corrections for the unmodelled polar motion, as
  /// published by the IERS Bulletins.
  /// @since 1.4
  NOVAS_ITRS
};

/**
 * The number of basic coordinate reference systems in NOVAS.
 *
 * @hideinitializer
 * @sa enum novas_reference_system
 */
#define NOVAS_REFERENCE_SYSTEMS   (NOVAS_ITRS + 1)


/**
 * Constants that determine the type of equator to be used for the coordinate system.
 *
 * @sa NOVAS_EQUATOR_TYPES
 * @sa equ2ecl(), ecl2equ()
 *
 * @c_nonequatorial
 */
enum novas_equator_type {
  NOVAS_MEAN_EQUATOR = 0, ///< Mean celestial equator of date without nutation (pre IAU 2006 system).
  NOVAS_TRUE_EQUATOR,     ///< True celestial equator of date (pre IAU 2006 system).
  NOVAS_GCRS_EQUATOR      ///< Geocentric Celestial Reference System (GCRS) equator.
};

/**
 * The number of equator types defined in `enum novas_equator_type`.
 *
 * @hideinitializer
 * @sa enum novas_equator_type
 * @since 1.2
 *
 * @c_nonequatorial
 */
#define NOVAS_EQUATOR_TYPES (NOVAS_GCRS_EQUATOR + 1)


/**
 * Constants that determine the type of dynamical system. I.e., the 'current' equatorial
 * coordinate system used for a given epoch of observation.
 *
 * @sa gcrs2equ()
 *
 * @c_equatorial
 */
enum novas_dynamical_type {
  /// Mean Of Date (TOD): dynamical system that include precession but not including nutation,
  /// such as commonly used for older catalogs like FK4, KF5, B1950, or HIP.
  NOVAS_DYNAMICAL_MOD = 0,///< NOVAS_DYNAMICAL_MOD

  /// True Of Date (TOD): dynamical system of the 'true' equator of date, with its origin at the
  /// true equinox of date. In the IAU 2000 methodology, it includes precession and nutation, but
  /// not the sub-arcsecond level Earth Orientation Parameters (EOP). The latter are used only
  /// when converting to the Earth-fixed TIRS system.
  NOVAS_DYNAMICAL_TOD,    ///< NOVAS_DYNAMICAL_TOD

  /// Celestial Intermediate Reference System (CIRS): dynamical system of the true equator,
  /// with its origin at the CIO (preferred since IAU 2006)
  NOVAS_DYNAMICAL_CIRS    ///< NOVAS_DYNAMICAL_CIRS
};

/**
 * Constants to control the precision of NOVAS nutation calculations.
 *
 * @sa novas_make_frame()
 * @c_frame
 */
enum novas_accuracy {
  /// Use full precision calculations to micro-arcsecond accuracy. It can be computationally
  /// intensive when using the dynamical equator.
  NOVAS_FULL_ACCURACY = 0,

  /// Calculate with truncated terms. It can be significantly faster if a few milliarcsecond
  /// accuracy is sufficient.
  NOVAS_REDUCED_ACCURACY
};


/**
 * Constants that determine whether what model (if any) to use for implicit refraction
 * calculations.
 *
 * @sa on_surface, novas_app_to_hor(), novas_hor_to_app(), refract(), refract_astro()
 * @c_refract
 */
enum novas_refraction_model {
  /// Do not apply atmospheric refraction correction
  NOVAS_NO_ATMOSPHERE = 0,

  /// Uses a standard atmospheric model, ignoring any weather values defined for the specific
  /// observing location.
  /// @sa novas_standard_refraction(), novas_set_default_weather()
  NOVAS_STANDARD_ATMOSPHERE,

  /// Uses the weather parameters that are specified together with the observing location.
  /// @sa novas_optical_refraction()
  NOVAS_WEATHER_AT_LOCATION,

  /// Uses the Berman &amp; Rockwell 1976 refraction model for Radio wavelengths with the
  /// weather parameters specified together with the observing location.
  /// @since 1.4
  /// @sa novas_radio_refraction()
  NOVAS_RADIO_REFRACTION,

  /// Uses the IAU / SOFA wavelength-depended refraction model with the weather parameters
  /// specified together with the observing location. The wavelength can be  specified
  /// via `novas_refract_wavelength()` or else it is assumed to be 550 nm (visible light).
  /// @since 1.4
  /// @sa novas_wave_refraction(), novas_refract_wavelength()
  NOVAS_WAVE_REFRACTION
};

/**
 * The number of built-in refraction models available in SuperNOVAS.
 *
 * @hideinitializer
 * @sa enum novas_refraction_model
 */
#define NOVAS_REFRACTION_MODELS   (NOVAS_WAVE_REFRACTION + 1)


#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated This enum is used only by functions which have been deprecated. Hence you
 *             probably won't need this either.
 *
 * Constants that determine the type of rotation measure to use.
 */
enum novas_earth_rotation_measure {
  /// Use Earth Rotation Angle (ERA) as the rotation measure, relative to the CIO (new IAU
  /// 2006 standard)
  EROT_ERA = 0,

  /// Use GST as the rotation measure, relative to the true equinox (pre IAU 20006 standard)
  EROT_GST
};
#endif

#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated This enum is used only by functions which have been deprecated. Hence you
 *             probably won't need this either.
 *
 * The class of celestial coordinates used as parameters for ter2cel() and cel2ter().
 */
enum novas_equatorial_class {
  NOVAS_REFERENCE_CLASS = 0,        ///< Celestial coordinates are in GCRS
  NOVAS_DYNAMICAL_CLASS             ///< Celestial coordinates are apparent values (CIRS or TOD)
};
#endif

#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated This enum is used only by functions which have been deprecated. Hence you
 *             probably won't need this either.
 *
 * The convention in which the celestial pole offsets are defined for polar wobble.
 */
enum novas_pole_offset_type {
  /// Offsets are &Delta;d&psi;, &Delta;d&epsilon; pairs (pre IAU 2006 precession-nutation
  /// model).
  POLE_OFFSETS_DPSI_DEPS = 1,

  /// Offsets are dx, dy pairs (IAU 2006 precession-nutation model)
  POLE_OFFSETS_X_Y
};
#endif

/**
 * The type of equinox used in the pre IAU 2006 (old) methodology.
 *
 * @sa ira_equinox()
 * @ingroup earth
 */
enum novas_equinox_type {
  NOVAS_MEAN_EQUINOX = 0,         /// Mean equinox: includes precession but not nutation
  NOVAS_TRUE_EQUINOX              /// True apparent equinox: includes both precession and nutation
};

/**
 * The origin of the ICRS system for referencing positions and velocities for solar-system bodies.
 *
 * @sa ephemeris(), NOVAS_ORIGIN_TYPES
 * @ingroup solar-system
 */
enum novas_origin {
  NOVAS_BARYCENTER = 0,           ///< Origin at the Solar-system baricenter (i.e. BCRS)
  NOVAS_HELIOCENTER               ///< Origin at the center of the Sun.
};

/**
 * The number of different ICSR origins available in NOVAS.
 *
 * @hideinitializer
 * @sa enum novas_origin
 */
#define NOVAS_ORIGIN_TYPES        (NOVAS_HELIOCENTER + 1)


/**
 * The types of coordinate transformations available for tranform_cat().
 *
 * @sa transform_cat(), make_cat_object_sys(), make_redshifted_object_sys(), NOVAS_TRANSFORM_TYPES
 * @ingroup source
 */
enum novas_transform_type {
  /// Updates the star's data to account for the star's space motion between
  /// the first and second dates, within a fixed reference frame.
  PROPER_MOTION = 1,

  /// applies a rotation of the reference frame
  /// corresponding to precession between the first and second dates,
  /// but leaves the star fixed in space.
  PRECESSION,

  /// The combined equivalent of PROPER_MOTION and PRECESSION together.
  CHANGE_EPOCH,

  /// A fixed rotation about very small angles (<0.1 arcsecond) to take data from the
  /// dynamical system of J2000.0 to the ICRS.
  CHANGE_J2000_TO_ICRS,

  /// The inverse transformation of J2000_TO_ICRS
  CHANGE_ICRS_TO_J2000
};

/**
 * The number of coordinate transfor types in NOVAS.
 *
 * @hideinitializer
 * @sa enum novas_transform_type
 */
#define NOVAS_TRANSFORM_TYPES     (ICRS_TO_J2000 + 1)

#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated This enum is used only by functions which have been deprecated. Hence you
 *             probably won't need this either.
 *
 * System in which CIO location is calculated.
 */
enum novas_cio_location_type {
  /// The location of the CIO relative to the GCRS frame.
  CIO_VS_GCRS = 1,

  /// The location of the CIO relative to the true equinox in the dynamical frame
  CIO_VS_EQUINOX
};
#endif

/**
 * Direction constants for polar wobble corrections via the wobble() function.
 *
 * @sa novas_app_to_hor(), novas_hor_to_app(), wobble(), NOVAS_WOBBLE_DIRECTIONS
 *
 * @ingroup nonequatorial
 */
enum novas_wobble_direction {
  /// use for wobble() to change from ITRS (Earth-fixed) to TIRS (pseudo Earth-fixed). It includes TIO
  /// longitude correction.
  /// @since 1.4
  WOBBLE_ITRS_TO_TIRS = 0,

  /// use for wobble() to change from TIRS (pseudo Earth-fixed) to ITRS (Earth-fixed). It includes TIO
  /// longitude correction.
  /// @since 1.4
  WOBBLE_TIRS_TO_ITRS,

  /// use for wobble() to change from ITRS (Earth-fixed) Pseudo Earth Fixed (PEF). It does not include
  /// TIO longitude correction. Otherwise, it's the same as WOBBLE_ITRS_TO_TIRS
  WOBBLE_ITRS_TO_PEF,

  /// use for wobble() to change from Pseudo Earth Fixed (PEF) to ITRS (Earth-fixed). It does not
  /// include TIO longitude correction. Otherwise, it's the same as WOBBLE_TIRS_TO_ITRS
  WOBBLE_PEF_TO_ITRS
};

/**
 * Number of values in enum novas_wobble_direction
 *
 * @hideinitializer
 * @since 1.4
 * @sa novas_wobble_direction
 */
#define NOVAS_WOBBLE_DIRECTIONS     (WOBBLE_PEF_TO_ITRS + 1)

/**
 * Direction constant to use for frame_tie(), to determine the direction of transformation
 * between J2000 and ICRS coordinates.
 *
 * @sa frame_tie(), gsrs_to_j2000(), j2000_to_gcrs(), J2000_TO_ICRS
 *
 * @ingroup equatorial
 */
enum novas_frametie_direction {
  /// Change coordinates from ICRS to the J2000 (dynamical) frame. (You can also use any
  /// negative value for the same effect).
  /// @sa j2000_to_gcrs()
  J2000_TO_ICRS = -1,

  /// Change coordinates from J2000 (dynamical) frame to the ICRS. (You can use any value
  /// &gt;=0 for the same effect).
  /// @sa gcrs_to_j2000()
  ICRS_TO_J2000
};

/**
 * Direction constant for nutation(), between mean and true equatorial coordinates.
 *
 * @sa nutation()
 *
 * @ingroup nonequatorial
 */
enum novas_nutation_direction {
  /// Change from true equator to mean equator (i.e. undo wobble corrections). You may use
  /// any non-zero value as well.
  NUTATE_TRUE_TO_MEAN = -1,

  /// Change from mean equator to true equator (i.e. apply wobble corrections)
  NUTATE_MEAN_TO_TRUE
};

/**
 * The plane in which values, such as orbital parameters are referenced.
 * @author Attila Kovacs
 * @since 1.2
 * @sa novas_orbital_system
 *
 * @c_source
 */
enum novas_reference_plane {
  NOVAS_ECLIPTIC_PLANE = 0,     ///< the plane of the ecliptic
  NOVAS_EQUATORIAL_PLANE,       ///< the plane of the equator
};

/**
 * Number of entries in enum novas_reference_plane
 * @hideinitializer
 * @since 1.6
 * @c_source
 */
#define NOVAS_REFERENCE_PLANES (NOVAS_EQUATORIAL_PLANE + 1)


/**
 * Fundamental Delaunay arguments of the Sun and Moon, from Simon section 3.4(b.3).
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa fund_args(), NOVAS_DELAUNAY_ARGS_INIT
 *
 * @c_earth
 */
typedef struct novas_delaunay_args {
  double l;           ///< [rad] mean anomaly of the Moon
  double l1;          ///< [rad] mean anomaly of the Sun
  double F;           ///< [rad] mean argument of the latitude of the Moon
  double D;           ///< [rad] mean elongation of the Moon from the Sun
  double Omega;       ///< [rad] mean longitude of the Moon's ascending node.
} novas_delaunay_args;

/**
 * Empty initializer for novas_delaunay_args
 *
 * @hideinitializer
 * @since 1.3
 * @author Attila Kovacs
 * @sa novas_delaunay_args
 * @c_earth
 */
#define NOVAS_DELAUNAY_ARGS_INIT { 0.0, 0.0, 0.0, 0.0, 0.0 }

/// Maximum number of bytes stored for object names including string termination.
/// @sa cat_entry
/// @ingroup source
#define SIZE_OF_OBJ_NAME 50

/// Maximum maximum bytes stored for catalog IDs including string termination.
/// @sa cat_entry
/// @ingroup source
#define SIZE_OF_CAT_NAME 6

/**
 * Basic astrometric data for any sidereal object located outside the solar system.
 *
 * Note, that despite the slightly expanded catalog name, this has the same memory footprint
 * as the original NOVAS C version, allowing for cross-compatible binary exchange (I/O) of
 * these structures.
 *
 * @sa novas_init_cat_entry(), make_cat_entry(), make_cat_object()
 * @sa CAT_ENTRY_INIT
 *
 * @c_source
 */

typedef struct novas_cat_entry {
  char starname[SIZE_OF_OBJ_NAME];  ///< name of celestial object
  char catalog[SIZE_OF_CAT_NAME];   ///< catalog designator (e.g., HIP)
  long starnumber;                  ///< integer identifier assigned to object
  double ra;                        ///< [h] ICRS right ascension
  double dec;                       ///< [deg] ICRS declination
  double promora;                   ///< [mas/yr] ICRS proper motion in right ascension
  double promodec;                  ///< [mas/yr] ICRS proper motion in declination
  double parallax;                  ///< [mas] parallax
  double radialvelocity;            ///< [km/s] catalog radial velocity (w.r.t. SSB)
  ///< To specify radial velocities defined in the Local Standard of Rest (LSR)
  ///< you might use novas_set_lsr_vel()
} cat_entry;


/**
 * Initializer for a NOVAS cat_entry structure.
 *
 * @hideinitializer
 * @author Attila Kovacs
 * @since 1.1.1
 *
 * @sa cat_entry
 * @c_source
 */
#define CAT_ENTRY_INIT { {'\0'}, {'\0'}, 0L, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

/**
 * Specification of an orbital system, in which orbital elements are defined. Systems can be defined around
 * all major planets and barycenters (and Sun, Moon, SSB..). They may be referenced to the GCRS, mean, or true equator
 * or ecliptic of date, or to a plane that is tilted relative to that.
 *
 * For example, The Minor Planet Center (MPC) publishes up-to-date orbital elements for asteroids and comets,
 * which are heliocentric and referenced to the GCRS ecliptic. Hence 'center' for these is `NOVAS_SUN`, the `plane`
 * is `NOVAS_ECLIPTIC_PLANE` and the `type` is `NOVAS_GCRS_EQUATOR`.
 *
 * The orbits of planetary satellites may be parametrized in their local Laplace planes, which are typically close
 * to the host planet's equatorial planes. You can, for example, obtain the RA/Dec orientation of the planetary
 * North poles of planets from JPL Horizons, and use them as a proxy for the Laplace planes for their satellite orbits.
 * In this case you would set the `center` to the host planet (e.g. `NOVAS_SATURN`), the reference plane to
 * `NOVAS_EQUATORIAL_PLANE` and the `type` to `NOVAS_GCRS_EQUATOR` (since the plane is defined by the North pole
 * orientation in GCRS equatorial RA/Dec). The obliquity is then 90&deg; - Dec<sub>pole</sub> (in radians), and `phi`
 * is RA<sub>pole</sub> (in radians).
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa novas_orbital, novas_set_orbsys_pole(), NOVAS_ORBITAL_SYSTEM_INIT
 * @c_source
 */
typedef struct novas_orbital_system {
  enum novas_planet center;          ///< major planet or barycenter at the center of the orbit.
  enum novas_reference_plane plane;  ///< reference plane NOVAS_ECLIPTIC_PLANE or NOVAS_EQUATORIAL_PLANE
  enum novas_reference_system type;  ///< the coordinate reference system used for the reference plane and orbitals.
                                     ///< It must be a system, which does not co-rotate with Earth (i.e. not ITRS or
                                     ///< TIRS).
  double obl;                        ///< [deg] relative obliquity of orbital reference plane
                                     ///<       (e.g. 90&deg; - &delta;<sub>pole</sub>)
  double Omega;                      ///< [deg] relative argument of ascending node of the orbital reference plane
                                     ///<       (e.g. &alpha;<sub>pole</sub> + 90&deg;). It is also the reference
                                     ///<       longitude of the orbit defined in this system.
} novas_orbital_system;


/**
 * Default orbital system initializer for heliocentric GCRS ecliptic orbits.
 *
 * @hideinitializer
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa novas_orbital_system
 * @c_source
 */
#define NOVAS_ORBITAL_SYSTEM_INIT { NOVAS_SUN, NOVAS_ECLIPTIC_PLANE, NOVAS_GCRS, 0.0, 0.0 }

/**
 * Keplerian orbital elements for `NOVAS_ORBITAL_OBJECT` type. Orbital elements can be used to provide
 * approximate positions for various Solar-system bodies. JPL publishes orbital elements (and their evolution)
 * for the major planets and their satellites. However, these are suitable only for very approximate
 * calculations, with up to degree scale errors for the gas giants for the time range between 1850 AD and
 * 2050 AD. Accurate positions and velocities for planets and their satellites should generally require the
 * use of precise ephemeris data instead, such as obtained from the JPL Horizons system.
 *
 * Orbital elements describe motion from a purely Keplerian perspective. However, for short periods, for
 * which the perturbing bodies can be ignored, this description can be quite accurate provided that an
 * up-to-date set of values are used. The Minor Planet Center (MPC) thus regularly publishes orbital
 * elements for all known asteroids and comets. For such objects, orbital elements can offer precise, and
 * the most up-to-date positions and velocities.
 *
 * REFERENCES:
 * <ol>
 * <li>Up-to-date orbital elements for asteroids, comets, etc from the Minor Planet Center (MPC):
 * https://minorplanetcenter.net/data</li>
 * <li>Mean elements for planetary satellites from JPL Horizons: https://ssd.jpl.nasa.gov/sats/elem/</li>
 * <li>Low accuracy mean elements for planets from JPL Horizons:
 * https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 * </ol>
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa make_orbital_object(), NOVAS_ORBIT_INIT, enum NOVAS_ORBITAL_OBJECT
 * @c_source
 */
typedef struct novas_orbital {
  struct novas_orbital_system system; ///< orbital reference system assumed for the parametrization
  double jd_tdb;                      ///< [day] Barycentri Dynamical Time (TDB) based Julian date of the parameters.
  double a;                           ///< [AU] semi-major axis
  double e;                           ///< eccentricity
  double omega;                       ///< [deg] argument of periapsis / perihelion, at the reference time. I.e., distance
                                      ///<       of apsis from ascending node.
  double Omega;                       ///< [deg] argument of ascending node on the reference plane, at the reference time.
  double i;                           ///< [deg] inclination of orbit to the reference plane
  double M0;                          ///< [deg] mean anomaly at the reference time. I.e., time from periapsis as a periodic angle.
  double n;                           ///< [deg/day] mean daily motion, i.e. (_GM_/_a_<sup>3</sup>)<sup>1/2</sup> for the central body,
  ///< or 360/T, where T is orbital period in days.
  double apsis_period;                ///< [day] Precession period of the apsis, if known.
  double node_period;                 ///< [day] Precession period of the ascending node, if known.

  // TODO Add tidal perturbation terms for orbits in potential gradients, e.g. for improved Moon orbital model.
  // double tide_angle;               ///< [deg] Tidal direction, measured from ascending node.
  // double elongation;               ///< Tidal elongation as fraction of major axis.
} novas_orbital;

/**
 * Initializer for novas_orbital for heliocentric orbits using GCRS ecliptic parametrization.
 *
 * @hideinitializer
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa novas_orbital
 *
 * @c_source
 */
#define NOVAS_ORBIT_INIT { NOVAS_ORBITAL_SYSTEM_INIT, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

/**
 * Celestial object of interest.
 *
 * Note, the memory footprint is different from NOVAS C due to the use of the enum vs short 'type'
 * and the long vs. short 'number' values -- hence it is not cross-compatible for binary data
 * exchange with NOVAS C 3.1.
 *
 * @sa make_cat_object(), make_redshifted_object(), make_planet(), make_ephem_object(), make_orbital_object()
 * @sa NOVAS_OBJECT_INIT, NOVAS_MERCURY_INIT, NOVAS_VENUS_INIT, NOVAS_EARTH_INIT, NOVAS_MARS_INIT,
 *     NOVAS_JUPITER_INIT, NOVAS_SATURN_INIT, NOVAS_URANUS_INIT, NOVAS_NEPTUNE_INIT,
 *     NOVAS_PLUTO_INIT, NOVAS_PLUTO_BARYCENTER_INIT, NOVAS_SUN_INIT, NOVAS_SSB_INIT,
 *     NOVAS_MOON_INIT, NOVAS_EMB_INIT
 * @sa novas_sky_pos(), novas_geom_posvel(), novas_rises_above(), novas_transit_time(), novas_sets_below()
 *     novas_sun_angle(), novas_moon_angle(), novas_object_sep()
 *
 * @c_source
 */
typedef struct novas_object {
  enum novas_object_type type;    ///< NOVAS object type
  long number;                    ///< enum novas_planet, or minor planet ID (e.g. NAIF), or star catalog ID.
  char name[SIZE_OF_OBJ_NAME];    ///< name of the object (0-terminated)
  struct novas_cat_entry star;    ///< basic astrometric data (in ICRS) for NOVAS_CATALOG_OBJECT type.
  struct novas_orbital orbit;     ///< orbital data for NOVAS_ORBITAL_OBJECT type. @since 1.2
} object;


/**
 * Empty object initializer.
 *
 * @hideinitializer
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa object
 * @c_source
 */
#define NOVAS_OBJECT_INIT { -1, 0L, {'\0'}, CAT_ENTRY_INIT, NOVAS_ORBIT_INIT }


/**
 * `object` initializer macro for major planets, the Sun, Moon, and barycenters.
 *
 * @param num     An `enum novas_planet` number
 * @param name    The designated planet name
 *
 * @hideinitializer
 * @since 1.2
 *
 * @sa object, make_planet()
 *
 */
#define NOVAS_PLANET_INIT(num, name) { NOVAS_PLANET, num, name, CAT_ENTRY_INIT, NOVAS_ORBIT_INIT }

/**
 * `object` initializer for the Solar System Barycenter (SSB)
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_SSB_INIT NOVAS_PLANET_INIT(NOVAS_SSB, "SSB")

/**
 * `object` initializer for the planet Venus
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_MERCURY_INIT NOVAS_PLANET_INIT(NOVAS_MERCURY, "Mercury")

/**
 * `object` initializer for the planet Mercury
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_VENUS_INIT NOVAS_PLANET_INIT(NOVAS_VENUS, "Venus")

/**
 * `object` initializer for the planet Earth
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_EARTH_INIT NOVAS_PLANET_INIT(NOVAS_EARTH, "Earth")

/**
 * `object` initializer for the planet Mars
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_MARS_INIT NOVAS_PLANET_INIT(NOVAS_MERCURY, "Mars")

/**
 * `object` initializer for the planet Jupiter
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_JUPITER_INIT NOVAS_PLANET_INIT(NOVAS_JUPITER, "Jupiter")

/**
 * `object` initializer for the planet Saturn
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_SATURN_INIT NOVAS_PLANET_INIT(NOVAS_SATURN, "Saturn")

/**
 * `object` initializer for the planet Uranus
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_URANUS_INIT NOVAS_PLANET_INIT(NOVAS_URANUS, "Uranus")

/**
 * `object` initializer for the planet Neptune
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_NEPTUNE_INIT NOVAS_PLANET_INIT(NOVAS_NEPTUNE, "Neptune")

/**
 * `object` initializer for the minor planet Pluto
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_PLUTO_INIT NOVAS_PLANET_INIT(NOVAS_PLUTO, "Pluto")

/**
 * `object` initializer for the Sun
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_SUN_INIT NOVAS_PLANET_INIT(NOVAS_SUN, "Sun")

/**
 * `object` initializer for the Moon
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_MOON_INIT NOVAS_PLANET_INIT(NOVAS_MOON, "Moon")

/**
 * `object` initializer for the the Earth-Moon Barycenter (EMB)
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_EMB_INIT NOVAS_PLANET_INIT(NOVAS_EMB, "EMB")

/**
 * `object` initializer for the Pluto system barycenter
 * @hideinitializer
 * @since 1.2
 * @sa object
 * @c_source
 */
#define NOVAS_PLUTO_BARYCENTER_INIT NOVAS_PLANET_INIT(NOVAS_PLUTO_BARYCENTER, "Pluto-Barycenter")


/**
 * Data for an observer's location on the surface of the Earth, and optional local weather data for
 * refraction calculations only.
 *
 * @sa make_itrf_site(), make_gps_site(), make_xyz_site(), make_observer_at_site(), ON_SURFACE_INIT
 * @sa make_observer_at_site(), make_airborne_observer()
 *
 * @c_observer
 */
typedef struct novas_on_surface {
  double latitude;      ///< [deg] geodetic (ITRS / GRS80) latitude; north positive
  double longitude;     ///< [deg] geodetic (ITRS / GRS80) longitude; east positive
  double height;        ///< [m] geodetic (ITRS / GRS80) altitude above sea level
  double temperature;   ///< [C] temperature (degrees Celsius); for optical refraction
  double pressure;      ///< [mbar] atmospheric pressure for optical refraction
  double humidity;      ///< [%] Relative humidity. @since 1.1
} on_surface;


/**
 * Initializer for a NOVAS on_surface data structure.
 *
 * @hideinitializer
 * @since 1.2
 * @author Attila Kovacs
 *
 * @sa on_surface, ON_SURFACE_LOC
 * @c_observer
 */
#define ON_SURFACE_INIT { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

/**
 * Initializer for a NOVAS `on_surface` data structure at a specified geodetic location
 *
 * @param lon     [deg] Geodetic longitude of observer (East is positive)
 * @param lat     [deg] Geodetic latitude of observer (North is positive)
 * @param alt     [m] Observer altitude above sea level.
 *
 * @hideinitializer
 * @since 1.2
 * @author Attila Kovacs
 *
 * @sa on_surface, make_itrf_site(), make_gps_site(), make_xyz_site(), ON_SURFACE_INIT
 * @c_observer
 */
#define ON_SURFACE_LOC(lon, lat, alt) { lon, lat, alt, 0.0, 0.0, 0.0 }

/**
 * data for an observer's location on Earth orbit
 *
 * @sa make_in_space(), IN_SPACE_INIT
 * @sa make_observer_in_space()
 * @c_observer
 */
typedef struct novas_in_space {
  double sc_pos[3];     ///< [km] geocentric (or [AU] ICRS barycentric) position vector (x, y, z)
  double sc_vel[3];     ///< [km/s] geocentric (or [AU/day] ICRS barycentric) velocity vector (x_dot, y_dot, z_dot)
} in_space;

/**
 * Initializer for a NOVAS in_space structure.
 *
 * @hideinitializer
 * @since 1.1.1
 * @author Attila Kovacs
 *
 * @sa in_space
 * @c_observer
 */
#define IN_SPACE_INIT   {{0.0}, {0.0}}

/**
 * Observer location.
 *
 * @sa make_itrf_observer(), make_gps_observer(), make_observer_at_site(),
 *     make_airborne_observer(), make_observer_at_geocenter(), make_observer_in_space(),
 *     make_solar_system_observer(), OBSERVER_INIT
 * @sa novas_make_frame()
 * @c_observer
 */
typedef struct novas_observer {
  enum novas_observer_place where;    ///< observer location type

  /// structure containing data for an observer's location on the surface of the Earth
  /// (if where = NOVAS_OBSERVER_ON_EARTH)
  struct novas_on_surface on_surf;

  /// data for an observer's location in orbit (if where = NOVAS_OBSERVER_IN_EARTH_ORBIT)
  /// As of v1.1 the same structure may be used to store heliocentric location and motion
  /// for any Solar-system observer also (if where = NOVAS_SOLAR_SYSTEM_OBSERVER).
  struct novas_in_space near_earth;
} observer;

/**
 * Empty initializer for observer
 *
 * @hideinitializer
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa observer
 * @c_observer
 */
#define OBSERVER_INIT   { NOVAS_OBSERVER_AT_GEOCENTER, ON_SURFACE_INIT, IN_SPACE_INIT }

/**
 * Celestial object's place on the sky; contains the output from place()
 *
 * @sa novas_sky_pos(), novas_transform_sky_pos(), SKY_POS_INIT
 * @c_apparent
 */
typedef struct novas_sky_pos {
  double r_hat[3];  ///< unit vector toward object (dimensionless)
  double ra;        ///< [h] apparent, topocentric, or astrometric right ascension (hours)
  double dec;       ///< [deg] apparent, topocentric, or astrometric declination (degrees)
  double dis;       ///< [AU] true (geometric, Euclidian) distance to solar system body or 0.0 for star (AU)
  double rv;        ///< [km/s] radial velocity (km/s). As of SuperNOVAS v1.3, this is always a proper
  ///< observer-based spectroscopic velocity measure, which relates the observed wavelength
  ///< to the rest wavelength as &lambda;<sub>obs</sub> = (1 + rv / c) &lambda;<sub>rest</sub>.
  ///< novas_ssb_to_lsr_vel(), novas_v2z()
} sky_pos;

/**
 * Initializer for a NOVAS sky_pos structure.
 *
 * @hideinitializer
 * @since 1.1.1
 * @author Attila Kovacs
 *
 * @sa sky_pos
 * @c_apparent
 */
#define SKY_POS_INIT { {0.0}, 0.0, 0.0, 0.0, 0.0 }

#ifndef _EXCLUDE_DEPRECATED
/**
 * [deprecated]
 *
 * @deprecated  The functions that use this structure have been deprecated also. There is no
 *              compelling reason why users should want/need it otherwise.
 *
 * Right ascension of the Celestial Intermediate Origin (CIO) with respect to the GCRS
 *
 * @sa cio_ra(), ira_equinox()
 */
typedef struct novas_ra_of_cio {
  double jd_tdb;    ///< [day] Barycentric Dynamical Time (TDB) based Julian date.
  double ra_cio;    ///< [arcsec] right ascension of the CIO with respect to the GCRS (arcseconds)
} ra_of_cio;
#endif


/**
 * Constants to reference various astrnomical timescales used
 *
 * @since 1.1
 *
 * @sa novas_set_time(), novas_get_time(), novas_diff_time(), novas_clock_skew(), NOVAS_TIMESCALES
 * @sa timescale.c
 *
 * @ingroup time
 */
enum novas_timescale {
  NOVAS_TCB = 0,    ///< Barycentric Coordinate Time (TCB)
  NOVAS_TDB,        ///< Barycentric Dynamical Time (TDB)
  NOVAS_TCG,        ///< Geocentric Coordinate Time (TCG)
  NOVAS_TT,         ///< Terrestrial Time (TT)
  NOVAS_TAI,        ///< Innternational Atomic Time (TAI)
  NOVAS_GPS,        ///< GPS Time
  NOVAS_UTC,        ///< Universal Coordinated Time (UTC)
  NOVAS_UT1,        ///< UT1 earth rotation time, based on the measured Earth orientation parameters published in IERS Bulletin A.
};

/**
 * The number of asronomical time scales supported.
 *
 * @hideinitializer
 * @since 1.1
 *
 * @sa novas_timescale
 */
#define NOVAS_TIMESCALES    (NOVAS_UT1 + 1)

/**
 * A structure, which defines a precise instant of time that can be extpressed in any of the
 * astronomical timescales. Precisions to picosecond accuracy are supported, which ought to be
 * plenty accurate for any real astronomical application.
 *
 * @since 1.1
 * @sa novas_set_time(), novas_get_time(), NOVAS_TIMESPEC_INIT, enum novas_timescale, timescale.c
 * @sa novas_make_frame()
 * @c_time
 */
typedef struct novas_timespec {
  long ijd_tt;        ///< [day] Integer part of the Terrestrial Time (TT) based Julian Date
  double fjd_tt;      ///< [day] Terrestrial time (TT) based fractional Julian day.
  double tt2tdb;      ///< [s] TDB - TT time difference
  double ut1_to_tt;   ///< [s] UT1 - TT time difference
  double dut1;        ///< [s] UT1 - UTC time difference
  // int leap_seconds;  -- add so we can access it also...  TODO for v2
} novas_timespec;

/**
 * Empty initializer for novas_timespec
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_timespec
 * @c_time
 */
#define NOVAS_TIMESPEC_INIT { 0, 0.0, 0.0, 0.0, 0.0 }

/**
 * A 3x3 matrix for coordinate transformations
 *
 * @since 1.1
 * @sa novas_transform, NOVAS_MATRIX_INIT, NOVAS_MATRIX_IDENTITY
 * @c_frame
 */
typedef struct novas_matrix {
  double M[3][3];     ///< matrix elements
} novas_matrix;

/**
 * Empty initializer for novas_matrix
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_matrix, NOVAS_MATRIX_IDENTITY
 */
#define NOVAS_MATRIX_INIT {{{0.0}}}

/**
 * novas_matric initializer for an indentity matrix
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_matrix, NOVAS_MATRIX_INIT
 */
#define NOVAS_MATRIX_IDENTITY {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}}

/**
 * Position and velocity data for a set of major planets (which may include the Sun and the Moon also).
 *
 * @since 1.1
 *
 * @sa enum novas_planet, NOVAS_PLANET_BUNDLE_INIT, grav_planets()
 */
typedef struct novas_planet_bundle {
  int mask;                      ///< Bitwise mask (1 << planet-number) specifying wich planets have pos/vel data
  double pos[NOVAS_PLANETS][3];  ///< [AU] Apparent positions of planets w.r.t. observer antedated for light-time
  double vel[NOVAS_PLANETS][3];  ///< [AU/day] Apparent velocity of planets w.r.t. barycenter antedated for light-time
} novas_planet_bundle;

/**
 * Empty initializer for novas_planet_bundle
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_planet_bundle
 */
#define NOVAS_PLANET_BUNDLE_INIT { 0, {{0.0}}, {{0.0}} }

/**
 * A set of parameters that uniquely define the place and time of observation. The user may
 * initialize the frame with novas_make_frame(). Once the observer frame is set up, it can be
 * used repeatedly to perform efficient calculations of multiple objects in the coordinate
 * system of choice, much faster than what place() can do. Frames also allow for transforming
 * coordinates calculated for one coordinate syste, into another coordinate system with
 * little effort.
 *
 * You should never set or change fields in this structure manually. Instead the structure
 * should always be initialized by an appropriate call to novas_make_frame(). After that you
 * may change the observer location, if need be, with novas_change_observer().
 *
 * The structure may expand with additional field in the future. Thus neither its size nor its
 * particular layout should be assumed fixed over SuperNOVAS releases.
 *
 * @since 1.1
 *
 * @sa novas_make_frame(), novas_change_observer(), novas_make_transform(), NOVAS_FRAME_INIT
 * @sa novas_sky_pos(), novas_geom_posvel(), novas_geom_to_app(), novas_app_to_geom(),
 *     novas_app_to_hor(), novas_hor_to_app(), novas_rises_above(), novas_sets_below(),
 *     novas_transit_time()
 *
 * @c_frame
 */
typedef struct novas_frame {
  uint64_t state;                     ///< An internal state for checking validity.
  enum novas_accuracy accuracy;       ///< NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
  struct novas_timespec time;         ///< The instant of time for which this observing frame is valid
  struct novas_observer observer;     ///< The observer location, or NULL for barycentric
  double mobl;                        ///< [rad] Mean obliquity
  double tobl;                        ///< [rad] True obliquity
  double ee;                          ///< [rad] Equation of the equinoxes
  double dpsi0;                       ///< [rad] Modeled Earth orientation &psi; (not including polar wobble)
  double deps0;                       ///< [rad] Modeled Earth orientation &epsilon; (not including polar wobble)
  double dx;                          ///< [mas] Polar wobble parameter dx.
  double dy;                          ///< [mas] Polar wobble parameter dy.
  double era;                         ///< [deg] Earth Rotation Angle (ERA);
  double gst;                         ///< [h] Greenwich (Apparent) Sidereal Time (GST / GAST)
  double obs_pos[3];                  ///< [AU] Observer position rel. to barycenter (ICRS)
  double obs_vel[3];                  ///< [AU/day] Observer movement rel. to barycenter (ICRS)
  double v_obs;                       ///< [AU/day] Magnitude of observer motion rel. to barycenter
  double beta;                        ///< Observer relativistic pseed, &beta; = _v_/_c_ rel SSB
  double gamma;                       ///< Observer Lorentz factor &Gamma; rel SSB
  double sun_pos[3];                  ///< [AU] Sun's geometric position, rel SSB. (ICRS)
  double sun_vel[3];                  ///< [AU/day] Sun's velocity, rel SSB. (ICRS)
  double earth_pos[3];                ///< [AU] Earth's geometric position, rel SSB. (ICRS)
  double earth_vel[3];                ///< [AU/day] Earth's velocity, rel SSB. (ICRS)
  struct novas_matrix icrs_to_j2000;  ///< ICRS to J2000 matrix
  struct novas_matrix precession;     ///< precession matrix
  struct novas_matrix nutation;       ///< nutation matrix (IAU 2006 model)
  struct novas_matrix gcrs_to_cirs;   ///< GCRS to CIRS conversion matrix
  struct novas_planet_bundle planets; ///< Planet positions and velocities (ICRS)
  // TODO [v2] add ra_cio
  // TODO [v2] add cirs_to_tirs
  // TODO [v2] add tirs_to_itrs
} novas_frame;


/**
 * Empty initializer for novas_frame
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_frame
 * @c_frame
 */
#define NOVAS_FRAME_INIT { 0, NOVAS_FULL_ACCURACY, NOVAS_TIMESPEC_INIT, OBSERVER_INIT, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
        0.0, 0.0, {0.0}, {0.0}, 0.0, 0.0, 0.0, {0.0}, {0.0}, {0.0}, {0.0}, NOVAS_MATRIX_INIT, NOVAS_MATRIX_INIT, \
        NOVAS_MATRIX_INIT, NOVAS_MATRIX_INIT, NOVAS_PLANET_BUNDLE_INIT }

/**
 * A transformation between two astronomical coordinate systems for the same observer
 * location and time. This allows for more elegant, generic, and efficient coordinate
 * transformations than the low-level NOVAS functions.
 *
 * The transformation can be (should be) initialized via novas_make_transform(), or via
 * novas_invert_transform().
 *
 * @since 1.1
 *
 * @sa novas_make_transform(), novas_invert_transform(), NOVAS_TRANSFORM_INIT
 * @sa novas_transform_vector(), novas_transform_sky_pos()
 * @c_frame
 */
typedef struct novas_transform {
  enum novas_reference_system from_system;  ///< The original coordinate system
  enum novas_reference_system to_system;    ///< The final coordinate system
  struct novas_frame frame;                 ///< The observer place and time for which the transform is valid
  struct novas_matrix matrix;               ///< Transformation matrix elements
} novas_transform;

/**
 * Empty initializer for NOVAS_TRANSFORM
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_transform
 * @c_frame
 */
#define NOVAS_TRANSFORM_INIT { -1, -1, NOVAS_FRAME_INIT, NOVAS_MATRIX_INIT }

/**
 * The type of elevation value for which to calculate a refraction.
 *
 * @sa RefractionModel, novas_app_to_hor(), novas_hor_to_app()
 *
 * @since 1.1
 * @c_refract
 */
enum novas_refraction_type {
  NOVAS_REFRACT_OBSERVED = -1,  ///< Refract observed elevation value
  NOVAS_REFRACT_ASTROMETRIC     ///< Refract astrometric elevation value
};

/**
 * Spherical and spectral coordinate set.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_track, NOVAS_OBSERVABLE_INIT
 * @c_apparent
 */
typedef struct novas_observable {
  double lon;           ///< [deg] apparent longitude coordinate in coordinate system
  double lat;           ///< [deg] apparent latitude coordinate in coordinate system
  double dist;          ///< [AU] apparent distance to source from observer
  double z;             ///< redshift
} novas_observable;

/**
 * Empty initializer for novas_observable
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_observable
 * @c_apparent
 */
#define NOVAS_OBSERVABLE_INIT { 0.0, 0.0, 0.0, 0.0 }

/**
 * The spherical and spectral tracking position of a source, and its first and second time derivatives. As such,
 * it may be useful for telescope drive control (position, velocity, and acceleration), or else for fast
 * extrapolation of momentary positions without a full, and costly, recalculation of the positions at high
 * rate over a suitable short period.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_hor_track(), novas_equ_track(), novas_track_pos(), NOVAS_TRACK_INIT
 * @c_apparent
 */
typedef struct novas_track {
  struct novas_timespec time;     ///< The astronomical time for which the track is calculated.
  struct novas_observable pos;    ///< [deg,AU,1] Apparent source position
  struct novas_observable rate;   ///< [deg/s,AU/s,1/s] Apparent position rate of change
  struct novas_observable accel;  ///< [deg/s<sup>2</sup>,AU/s<sup>2</sup>,1/s<sup>2</sup>] Apparent position acceleration.
} novas_track;

/**
 * Empty initializer for novas_track
 *
 * @hideinitializer
 * @since 1.3
 * @sa novas_track
 * @c_apparent
 */
#define NOVAS_TRACK_INIT { NOVAS_TIMESPEC_INIT, NOVAS_OBSERVABLE_INIT, NOVAS_OBSERVABLE_INIT, NOVAS_OBSERVABLE_INIT }

/**
 * The general order of date components for parsing.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_date_format()
 * @ingroup time
 */
enum novas_date_format {
  NOVAS_YMD = 0,          ///< year, then month, then day.
  NOVAS_DMY,              ///< day, then month, then year
  NOVAS_MDY               ///< month, then day, then year
};

/**
 * Constants to disambiguate which type of calendar yo use for interpreting calendar dates. Roman/Julian or Gregorian/
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_jd_from_date(), novas_jd_to_date()
 * @c_time
 */
enum novas_calendar_type {
  NOVAS_ROMAN_CALENDAR = -1,    ///< The Roman (a.k.a. Julian) calendar by Julius Caesar, introduced in -45 B.C.
  NOVAS_ASTRONOMICAL_CALENDAR,  ///< Roman (a.k.a. Julian) calendar until the Gregorian calendar reform of 1582,
  ///< after which it is the Gregorian calendar
  NOVAS_GREGORIAN_CALENDAR      ///< The Gregorian calendar introduced on 15 October 1582, the day after 4 October
  ///< 1582 in the Roman (a.k.a. Julian) calendar.
};

/**
 * Separator type to use for broken-down time/angle string representations in HMS/DMS formats.
 *
 * @since 1.3
 *
 * @sa novas_print_hms(), novas_print_dms()
 * @c_util
 */
enum novas_separator_type {
  NOVAS_SEP_COLONS = 0,       ///< Use colons between components, e.g. '12:34:56'
  NOVAS_SEP_SPACES,           ///< Use spaces between components, e.g. '12 34 56'
  NOVAS_SEP_UNITS,            ///< Use unit markers after each component, e.g. '12h34m56s'
  NOVAS_SEP_UNITS_AND_SPACES  ///< Useunit markers after each compoent, plus spaces between components, e.g. '12h 34m 56s'
};

/**
 * Type of Earth reference ellipsoid. Only ellipsoids commonly in use today are listed, ommitting many obsoleted
 * historical variants.
 *
 * @since 1.5
 *
 * @sa novas_cartesian_to_geodetic(), novas_geodetic_to_cartesian(), novas_geodetic_transform_site()
 * @c_observer
 */
enum novas_reference_ellipsoid {
  NOVAS_GRS80_ELLIPSOID = 0,  ///< GRS80 reference ellipsoid, used for the International Terrestrial Reference System (ITRS).
  NOVAS_WGS84_ELLIPSOID,      ///< WGS84 reference ellipsoid, used for GPS navigation.
  NOVAS_IERS_1989_ELLIPSOID,  ///< IERS (1989) reference ellipsoid, formerly used by the IERS conventions (but not for ITRS, which uses the GRS80 model).
  NOVAS_IERS_2003_ELLIPSOID   ///< IERS (2003) reference ellipsoid, used by the IERS conventions (but not for ITRS, which uses the GRS80 model).
};

/// \cond _PRIVATE
#ifndef _NUTATION_
#define _NUTATION_
/// \endcond

/**
 * Function type definition for the IAU 2000 nutation series calculation.
 *
 * @param jd_tt_high  [day] High-order part of the Terrestrial Time (TT) based Julian date. Typically
 *                    it may be the integer part of a split date for the highest precision, or the
 *                    full date for normal (reduced) precision.
 * @param jd_tt_low   [day] Low-order part of the Terrestrial Time (TT) based Julian date. Typically
 *                    it may be the fractional part of a split date for the highest precision, or 0.0
 *                    for normal (reduced) precision.
 * @param[out] dpsi   [rad] &delta;&psi; Nutation (luni-solar + planetary) in longitude, in radians.
 * @param[out] deps   [rad] &delta;&epsilon; Nutation (luni-solar + planetary) in obliquity, in radians.
 * @return            0 if successful, or else -1 (errno should be set to indicate the type of error).
 *
 * @sa nutation(), nutation_angles(), iau2000a(), iau2000b(), iau2000k()
 *
 * @author Attila Kovacs
 * @since 1.0
 * @ingroup earth
 */
typedef int (*novas_nutation_provider)(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps);

/**
 * Provides the position and velocity of major planets (as well as the Sun, Moon, Solar-system
 * Barycenter, and other barycenters). This version provides positions and velocities at regular
 * precision (see NOVAS_REDUCED_PRECISION).
 *
 * Since this is a function that may be provided by existing custom user implementations, we
 * keep the original argument types for compatibility, hence 'short' instead of the more
 * informative enums).
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), 1 if the input Julian date ('tjd') is out of
 *                      range, 2 if 'body' is invalid, or 3 if the ephemeris data cannot be
 *                      produced for other reasons.
 *
 * @sa set_planet_provider(), ephemeris(), novas_solarsystem_hp_func
 * @sa make_planet(), novas_sky_pos(), novas_geom_posvel()
 *
 * @since 1.0
 * @author Attila Kovacs
 * @ingroup solar-system
 */
typedef short (*novas_planet_provider)(double jd_tdb, enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);


/**
 * Provides the position and velocity of major planets (as well as the Sun, Moon, Solar-system
 * Barycenter, and other barycenters). This version provides positions and velocities at high
 * precision (see NOVAS_FULL_PRECISION).
 *
 * Since this is a function that may be provided by existing custom user implementations, we
 * keep the original argument types for compatibility, hence 'short' instead of the more
 * informative enums).
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date, broken into
 *                      high and low order components, respectively. Typically, as the integer
 *                      and fractional parts for the highest precision.
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), 1 if the input Julian date ('tjd') is out of
 *                      range, 2 if 'body' is invalid, or 3 if the ephemeris data cannot be
 *                      produced for other reasons.
 *
 * @sa set_planet_provider_hp(), novas_solarsystem_func, ephemeris()
 * @sa make_planet(), novas_sky_pos(), novas_geom_posvel(), grav_planets(), grav_undo_planets()
 * @since 1.0
 * @author Attila Kovacs
 * @ingroup solar-system
 */
typedef short (*novas_planet_provider_hp)(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);


/**
 * Function to obtain ephemeris data for minor planets, which are not handled by the
 * solarsystem() type calls. The library does not provide a default implementation, but users
 * can provide their own, either as a default statically compiled readeph() implementation,
 * or else a dynamically defined one via ephemeris_set_reader().
 *
 * Note, that implementations would typically use either the name or the ID argument
 * to identify the object for which ephemeris data is requested. As such you only need
 * to specify the one that is going to be used.
 *
 * @param name          The name of the solar-system body (in case the ephemeris provider
 *                      supports lookup by name), or NULL to force ID based lookup.
 * @param id            The ID number of the solar-system body for which the position in
 *                      desired. (Typically a NAIF ID, or else an appropriate ID for the
 *                      implementation -- corresponding minor planet objects should be created
 *                      with the same type of ID.). A value of -1 can be used to force name
 *                      based lookups (provided the implementation supports it).
 * @param jd_tdb_high   [day] The high-order part of Barycentric Dynamical Time (TDB) based
 *                      Julian date for which to find the position and velocity. Typically
 *                      this may be the integer part of the Julian date for high-precision
 *                      calculations, or else the entire Julian date for reduced precision.
 * @param jd_tdb_low    [day] The low-order part of Barycentric Dynamical Time (TDB) based
 *                      Julian date for which to find the position and velocity. Typically
 *                      this may be the fractional part of the Julian date for high-precision
 *                      calculations, or else 0.0 if the date is defined entirely by the
 *                      high-order component for reduced precision.
 * @param[out] origin   Set to NOVAS_BARYCENTER or NOVAS_HELIOCENTER to indicate relative to
 *                      which the ephemeris positions/velocities are reported.
 * @param[out] pos      [AU] position 3-vector to populate with rectangular equatorial
 *                      coordinates in AU. It may be NULL if position is not required.
 * @param[out] vel      [AU/day] velocity 3-vector to populate in rectangular equatorial
 *                      coordinates in AU/day. It may be NULL if velocities are not required.
 * @return              0 if successful, -1 if any of the pointer arguments are NULL, or some
 *                      non-zero value if the was an error s.t. the position and velocity
 *                      vector should not be used.
 *
 * @sa set_ephem_provider(), ephemeris(), NOVAS_EPHEM_OBJECT, solsys-ephem.c
 * @sa make_ephem_object(), novas_sky_pos(), novas_geom_posvel()
 *
 * @since 1.0
 * @author Attila Kovacs
 * @ingroup solar-system
 */
typedef int (*novas_ephem_provider)(const char *name, long id, double jd_tdb_high, double jd_tdb_low,
        enum novas_origin *restrict origin, double *restrict pos, double *restrict vel);


/**
 * A function that returns a refraction correction for a given date/time of observation at the
 * given site on earth, and for a given astrometric source elevation
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
 * @param el        [deg] Astrometric (unrefracted) source elevation
 * @return          [arcsec] Estimated refraction, or NAN if there was an error (it should
 *                  also set errno to indicate the type of error).
 *
 * @since 1.1
 *
 * @sa novas_app_to_hor(), novas_hor_to_app()
 * @ingroup refract
 */
typedef double (*RefractionModel)(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);


#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated This old ephemeris reader is prone to memory leaks, and lacks some useful
 *             functionality. Users are strongly encouraged to use the new
 *             `novas_ephem_provider` / `novas_set_ephem_provider()` instead, for dynamically
 *             configured implementations at runtime. This prototype is provided only to extend
 *             support for legacy NOVAS applications only, where an inplementation had to be
 *             linked always.
 *
 * Legacy NOVAS C function to handle position and velocity calculations for Solar-system sources,
 * beyond the major planets. This function can be defined by an external module, as per the NOVAS
 * C way, provided you compile SuperNOVAS with the `READPH_SOURCE` option set to specify the
 * source(s) that implement it (in `config.mk` or the environment). If `READEPH_SOURCE` in not
 * defined during the build of SuperNOVAS, then this function will not be used by __SuperNOVAS__,
 * nor will be implemented by it.
 *
 * @param mp            The ID number of the solar-system body for which the position are
 *                      desired. An actual implementation might use this and/or the name to
 *                      identify the object.
 * @param name          The name of the solar-system body (usually upper-case). An actual
 *                      implementation might use this and/or `mp` to identify the object.
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date for which to
 *                      find the position and velocity.
 * @param[out] error    Pointer to integer to populate with the error status: 0 if successful,
 *                      -1 if any of the pointer arguments are NULL, or some non-zero value
 *                      if the was an error s.t. the position and velocity vector should not
 *                      be used.
 * @return              [AU, AU/day] A newly allocated 6-vector in rectangular equatorial
 *                      coordinates, containing the heliocentric position coordinates in AU,
 *                      followed by hte heliocentric velocity components in AU/day. The caller
 *                      is responsible for calling free() on the returned value when it is no
 *                      longer needed.
 *
 * @sa novas_ephem_provider
 *
 */
double *readeph(int mp, const char *restrict name, double jd_tdb, int *restrict error);
#endif

// in nutation.c
/// @c_earth
int iau2000a(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps);

/// @c_earth
int iau2000b(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps);

/// @c_earth
int nu2000k(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps);

/// \cond _PRIVATE
#endif /* _NUTATION_ */
/// \endcond

// in ephemeris.c
/// @c_geometric
short ephemeris(const double *restrict jd_tdb, const object *restrict body, enum novas_origin origin,
        enum novas_accuracy accuracy, double *restrict pos, double *restrict vel);

// in place.c
/// @c_apparent
short app_star(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy,
        double *restrict ra, double *restrict dec);

/// @c_apparent
short virtual_star(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy,
        double *restrict ra, double *restrict dec);

/// @c_geometric
short astro_star(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy,
        double *restrict ra, double *restrict dec);

/// @c_apparent
short app_planet(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy,
        double *restrict ra, double *restrict dec, double *restrict dis);

/// @c_apparent
short virtual_planet(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy,
        double *restrict ra, double *restrict dec, double *restrict dis);

/// @c_geometric
short astro_planet(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy,
        double *restrict ra, double *restrict dec, double *restrict dis);

#ifndef _EXCLUDE_DEPRECATED
short topo_star(double jd_tt, double ut1_to_tt, const cat_entry *restrict star, const on_surface *restrict position,
        enum novas_accuracy accuracy, double *restrict ra, double *restrict dec);
#endif

#ifndef _EXCLUDE_DEPRECATED
short local_star(double jd_tt, double ut1_to_tt, const cat_entry *restrict star, const on_surface *restrict position,
        enum novas_accuracy accuracy, double *restrict ra, double *restrict dec);
#endif

#ifndef _EXCLUDE_DEPRECATED
short topo_planet(double jd_tt, const object *restrict ss_body, double ut1_to_tt, const on_surface *restrict position,
        enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis);
#endif

#ifndef _EXCLUDE_DEPRECATED
short local_planet(double jd_tt, const object *restrict ss_body, double ut1_to_tt, const on_surface *restrict position,
        enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis);
#endif

short mean_star(double jd_tt, double tra, double tdec, enum novas_accuracy accuracy,
        double *restrict ira, double *restrict idec);

short place(double jd_tt, const object *restrict source, const observer *restrict location, double ut1_to_tt,
        enum novas_reference_system coord_sys, enum novas_accuracy accuracy, sky_pos *restrict output);


// in coords.
/// @c_nonequatorial
int equ2gal(double ra, double dec, double *restrict glon, double *restrict glat);

/// @c_nonequatorial
short equ2ecl(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec,
        double *restrict elon, double *restrict elat);

/// @c_nonequatorial
short equ2ecl_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy,
        const double *in, double *out);

/// @c_nonequatorial
short ecl2equ_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy,
        const double *in, double *out);

#ifndef _EXCLUDE_DEPRECATED
int equ2hor(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp,
        const on_surface *restrict location, double ra, double dec, enum novas_refraction_model ref_option,
        double *restrict zd, double *restrict az, double *restrict rar, double *restrict decr);
#endif

// in system.c
/// @c_equatorial
int frame_tie(const double *in, enum novas_frametie_direction direction, double *out);

/// @c_equatorial
short gcrs2equ(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg,
        double *restrict ra, double *restrict dec);

#ifndef _EXCLUDE_DEPRECATED
short sidereal_time(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_equinox_type gst_type,
        enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, double *restrict gst);
#endif

#ifndef _EXCLUDE_DEPRECATED
short ter2cel(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot,
        enum novas_accuracy accuracy, enum novas_equatorial_class coordType, double xp, double yp, const double *in,
        double *out);
#endif

#ifndef _EXCLUDE_DEPRECATED
short cel2ter(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot,
        enum novas_accuracy accuracy, enum novas_equatorial_class coordType, double xp, double yp, const double *in,
        double *out);
#endif

// in util.c
/// @c_util
int spin(double angle, const double *in, double *out);

double d_light(const double *pos_src, const double *pos_body);

/// @c_util
short vector2radec(const double *restrict pos, double *restrict ra, double *restrict dec);

/// @c_util
int radec2vector(double ra, double dec, double dist, double *restrict pos);

// in earth.c
/// @c_earth
double era(double jd_ut1_high, double jd_ut1_low);

/// @c_nonequatorial
int wobble(double jd_tt, enum novas_wobble_direction direction, double xp, double yp, const double *in, double *out);

/// @c_earth
int terra(const on_surface *restrict location, double gast, double *restrict pos, double *restrict vel);

/// @c_earth
int e_tilt(double jd_tdb, enum novas_accuracy accuracy, double *restrict mobl, double *restrict tobl,
        double *restrict ee, double *restrict dpsi, double *restrict deps);

#ifndef _EXCLUDE_DEPRECATED
short cel_pole(double jd_tt, enum novas_pole_offset_type type, double dpole1, double dpole2);
#endif

// in equator.c
#ifndef _EXCLUDE_DEPRECATED
/// \cond PRIVATE
double ee_ct(double jd_tt_high, double jd_tt_low, enum novas_accuracy accuracy);
/// \endcond
#endif

/// @c_earth
int fund_args(double t, novas_delaunay_args *restrict a);

/// @c_earth
double accum_prec(double t);

/// @c_earth
double mean_obliq(double jd_tdb);

/// @c_equatorial
double ira_equinox(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy);

/// @c_earth
short precession(double jd_tdb_in, const double *in, double jd_tdb_out, double *out);

/// @c_earth
int nutation(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *in,
        double *out);

/// @c_earth
int nutation_angles(double t, enum novas_accuracy accuracy, double *restrict dpsi, double *restrict deps);

// in planets.c
double planet_lon(double t, enum novas_planet planet);

// in earth.c
/// @c_geometric
short geo_posvel(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *restrict obs,
        double *restrict pos, double *restrict vel);

/// @c_apparent
int limb_angle(const double *pos_src, const double *pos_obs, double *restrict limb_ang, double *restrict nadir_ang);

// in grav.c
short grav_def(double jd_tdb, enum novas_observer_place unused, enum novas_accuracy accuracy, const double *pos_src,
        const double *pos_obs, double *out);

int grav_vec(const double *pos_src, const double *pos_obs, const double *pos_body, double rmass, double *out);

// in spectral.c
/// @c_spectral
int rad_vel(const object *restrict source, const double *restrict pos_src, const double *vel_src, const double *vel_obs,
        double d_obs_geo, double d_obs_sun, double d_src_sun, double *restrict rv);

// in timescale.c
/// @c_time
double get_ut1_to_tt(int leap_seconds, double dut1);

/// @c_time
int tdb2tt(double jd_tdb, double *restrict jd_tt, double *restrict secdiff);

// in  cio.c
/// @c_equatorial
short cio_ra(double jd_tt, enum novas_accuracy accuracy, double *restrict ra_cio);

#ifndef _EXCLUDE_DEPRECATED
short cio_location(double jd_tdb, enum novas_accuracy accuracy, double *restrict ra_cio, short *restrict loc_type);
#endif

#ifndef _EXCLUDE_DEPRECATED
short cio_basis(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy,
        double *restrict x, double *restrict y, double *restrict z);
#endif

#ifndef _EXCLUDE_DEPRECATED
short cio_array(double jd_tdb, long n_pts, ra_of_cio *restrict cio);
#endif

// in refract.c
/// @c_refract
double refract(const on_surface *restrict location, enum novas_refraction_model model, double zd_obs);

// in calendar.c
/// @c_time
double julian_date(short year, short month, short day, double hour);

/// @c_time
int cal_date(double tjd, short *restrict year, short *restrict month, short *restrict day, double *restrict hour);

// in target.c
/// @c_source
short make_cat_entry(const char *restrict name, const char *restrict catalog, long cat_num, double ra, double dec,
        double pm_ra, double pm_dec, double parallax, double rad_vel, cat_entry *source);

/// @c_source
short transform_cat(enum novas_transform_type, double jd_tt_in, const cat_entry *in, double jd_tt_out, const char *out_id,
        cat_entry *out);

/// @c_source
int transform_hip(const cat_entry *hipparcos, cat_entry *hip_2000);

/// @c_source
int starvectors(const cat_entry *restrict star, double *restrict pos, double *restrict motion);

#ifndef _EXCLUDE_DEPRECATED
short make_object(enum novas_object_type, long number, const char *name, const cat_entry *star, object *source);
#endif

/// @c_source
int proper_motion(double jd_tdb_in, const double *pos, const double *restrict vel, double jd_tdb_out, double *out);

// in observer.c
#ifndef _EXCLUDE_DEPRECATED
short make_observer(enum novas_observer_place, const on_surface *loc_surface, const in_space *loc_space,
        observer *obs);
#endif

/// @c_observer
int make_observer_at_geocenter(observer *restrict obs);

/// @c_observer
int make_observer_on_surface(double latitude, double longitude, double height, double temperature, double pressure,
        observer *restrict obs);

/// @c_observer
int make_observer_in_space(const double *sc_pos, const double *sc_vel, observer *obs);

#ifndef _EXCLUDE_DEPRECATED
int make_on_surface(double latitude, double longitude, double height, double temperature, double pressure,
        on_surface *restrict loc);
#endif

/// @c_observer
int make_in_space(const double *sc_pos, const double *sc_vel, in_space *loc);

int bary2obs(const double *pos, const double *pos_obs, double *out, double *restrict lighttime);

int aberration(const double *pos, const double *vobs, double lighttime, double *out);

/// @c_geometric
short light_time(double jd_tdb, const object *restrict body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy,
        double *pos_src_obs, double *restrict tlight);


// -------------------------------------------------------------------------------------------------------------------
// SuperNOVAS API:

// in util.c
/// @ingroup util
void novas_debug(enum novas_debug_mode mode);

/// @ingroup util
enum novas_debug_mode novas_get_debug_mode();

/// @c_util
double novas_norm_ang(double angle);

// in target.c
/// @c_source
void novas_case_sensitive(int value);

/// @c_source
int make_planet(enum novas_planet num, object *restrict planet);

/// @ingroup source
int make_ephem_object(const char *name, long num, object *body);

// in cio.c
#ifndef _EXCLUDE_DEPRECATED
int set_cio_locator_file(const char *restrict filename);
#endif

// in ephemeris.c
/// @ingroup earth
int set_nutation_lp_provider(novas_nutation_provider func);

// in place.c
int place_star(double jd_tt, const cat_entry *restrict star, const observer *restrict obs, double ut1_to_tt,
        enum novas_reference_system system, enum novas_accuracy accuracy, sky_pos *restrict pos);

int place_icrs(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos);

/// @c_apparent
int place_gcrs(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos);

/// @c_apparent
int place_cirs(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos);

/// @c_apparent
int place_tod(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos);

int radec_star(double jd_tt, const cat_entry *restrict star, const observer *restrict obs, double ut1_to_tt,
        enum novas_reference_system sys, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec,
        double *restrict rv);

int radec_planet(double jd_tt, const object *restrict ss_body, const observer *restrict obs, double ut1_to_tt,
        enum novas_reference_system sys, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec,
        double *restrict dis, double *restrict rv);

// in refract.c
/// @c_refract
double refract_astro(const on_surface *restrict location, enum novas_refraction_model model, double zd_astro);

// in observer.c
/// @c_geometric
int light_time2(double jd_tdb, const object *restrict body, const double *restrict pos_obs, double tlight0,
        enum novas_accuracy accuracy, double *p_src_obs, double *restrict v_ssb, double *restrict tlight);

// in timescale.c
/// @c_time
double tt2tdb(double jd_tt);

/// @c_time
double get_ut1_to_tt(int leap_seconds, double dut1);

/// @c_time
double get_utc_to_tt(int leap_seconds);

// in system.c
/// @c_equatorial
int gcrs_to_cirs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out);

/// @c_equatorial
int cirs_to_itrs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp,
        double yp, const double *in, double * out);

/// @c_equatorial
int itrs_to_cirs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp,
        double yp, const double *in, double *out);

/// @c_equatorial
int cirs_to_gcrs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out);

/// @c_equatorial
int gcrs_to_j2000(const double *in, double *out);

/// @c_equatorial
int j2000_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out);

/// @c_equatorial
int tod_to_itrs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp,
        double yp, const double *in, double *out);

/// @c_equatorial
int itrs_to_tod(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp,
        double yp, const double *in, double *out);

/// @c_equatorial
int tod_to_j2000(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out);

/// @c_equatorial
int j2000_to_gcrs(const double *in, double *out);

// in coords.c
/// @c_nonequatorial
int itrs_to_hor(const on_surface *restrict location, const double *restrict itrs, double *restrict az, double *restrict za);

/// @c_nonequatorial
int hor_to_itrs(const on_surface *restrict location, double az, double za, double *restrict itrs);

/// @c_nonequatorial
int ecl2equ(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat,
        double *restrict ra, double *restrict dec);

/// @c_nonequatorial
int gal2equ(double glon, double glat, double *restrict ra, double *restrict dec);


// in ephemeris.c
/// @ingroup solar-system
int set_planet_provider(novas_planet_provider func);

/// @ingroup solar-system
int set_planet_provider_hp(novas_planet_provider_hp func);

/// @ingroup solar-system
int set_ephem_provider(novas_ephem_provider func);

/// @ingroup solar-system
novas_ephem_provider get_ephem_provider();


/// @c_solar-system
short earth_sun_calc(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position,
        double *restrict velocity);

/// @c_solar-system
short earth_sun_calc_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);

/// @c_solar-system
void enable_earth_sun_hp(int value);

/// @c_solar-system
short planet_ephem_provider(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position,
        double *restrict velocity);

/// @c_solar-system
short planet_ephem_provider_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);

// ---------------------- Added in 1.0.1 -------------------------

// in system.c
/// @c_equatorial
double cirs_to_app_ra(double jd_tt, enum novas_accuracy accuracy, double ra);

/// @c_equatorial
double app_to_cirs_ra(double jd_tt, enum novas_accuracy accuracy, double ra);

// ---------------------- Added in 1.1.0 -------------------------


// in grav.c
int grav_undef(double jd_tdb, enum novas_accuracy accuracy, const double *pos_app, const double *pos_obs, double *out);

int grav_planets(const double *pos_src, const double *pos_obs, const novas_planet_bundle *restrict planets, double *out);

int grav_undo_planets(const double *pos_app, const double *pos_obs, const novas_planet_bundle *restrict planets, double *out);

// in observer.c
/// @c_observer
int make_airborne_observer(const on_surface *location, const double *vel, observer *obs);

/// @c_observer
int make_solar_system_observer(const double *sc_pos, const double *sc_vel, observer *obs);

/// @c_geometric
int obs_posvel(double jd_tdb, double ut1_to_tt, enum novas_accuracy accuracy, const observer *restrict obs,
        const double *restrict geo_pos, const double *restrict geo_vel, double *restrict pos, double *restrict vel);

/// @c_solar-system
int obs_planets(double jd_tdb, enum novas_accuracy accuracy, const double *restrict pos_obs, int pl_mask,
        novas_planet_bundle *restrict planets);

// in target.c
/// @c_source
int make_cat_object(const cat_entry *star, object *source);

// in place.c
/// @c_apparent
int place_mod(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos);

/// @c_apparent
int place_j2000(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos);

// in system.c
/// @c_equatorial
int cirs_to_tod(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

/// @c_equatorial
int tod_to_cirs(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

// in spectral.c
/// @c_spectral
double rad_vel2(const object *restrict source, const double *pos_emit, const double *vel_src, const double *pos_det,
        const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun);

// in timescale.c
/// @c_time
int novas_set_time(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *restrict time);

/// @c_time
int novas_set_split_time(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1,
        novas_timespec *restrict time);

/// @c_time
double novas_get_time(const novas_timespec *restrict time, enum novas_timescale timescale);

/// @c_time
double novas_get_split_time(const novas_timespec *restrict time, enum novas_timescale timescale, long *restrict ijd);

/// @c_time
int novas_set_unix_time(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *restrict time);

/// @c_time
time_t novas_get_unix_time(const novas_timespec *restrict time, long *restrict nanos);

/// @c_time
double novas_diff_time(const novas_timespec *t1, const novas_timespec *t2);

/// @c_time
double novas_diff_tcb(const novas_timespec *t1, const novas_timespec *t2);

/// @c_time
double novas_diff_tcg(const novas_timespec *t1, const novas_timespec *t2);

/// @c_time
int novas_offset_time(const novas_timespec *time, double seconds, novas_timespec *out);

// in frames.c
/// @c_frame
int novas_make_frame(enum novas_accuracy accuracy, const observer *obs, const novas_timespec *time, double xp, double yp,
        novas_frame *frame);

/// @c_frame
int novas_change_observer(const novas_frame *orig, const observer *obs, novas_frame *out);

/// @c_geometric
int novas_geom_posvel(const object *restrict source, const novas_frame *restrict frame, enum novas_reference_system sys,
        double *restrict pos, double *restrict vel);

/// @c_geometric
int novas_geom_to_app(const novas_frame *restrict frame, const double *restrict pos, enum novas_reference_system sys,
        sky_pos *restrict out);

/// @c_apparent
int novas_sky_pos(const object *restrict object, const novas_frame *restrict frame, enum novas_reference_system sys,
        sky_pos *restrict out);

/// @c_apparent
int novas_app_to_hor(const novas_frame *restrict frame, enum novas_reference_system sys, double ra, double dec,
        RefractionModel ref_model, double *restrict az, double *restrict el);

/// @c_apparent
int novas_app_to_geom(const novas_frame *restrict frame, enum novas_reference_system sys, double ra, double dec,
        double dist, double *restrict geom_icrs);

/// @c_nonequatorial
int novas_hor_to_app(const novas_frame *restrict frame, double az, double el, RefractionModel ref_model,
        enum novas_reference_system sys, double *restrict ra, double *restrict dec);

/// @c_frame
int novas_make_transform(const novas_frame *frame, enum novas_reference_system from_system,
        enum novas_reference_system to_system, novas_transform *transform);

/// @c_frame
int novas_invert_transform(const novas_transform *transform, novas_transform *inverse);

/// @c_geometric
int novas_transform_vector(const double *in, const novas_transform *restrict transform, double *out);

/// @c_apparent
int novas_transform_sky_pos(const sky_pos *in, const novas_transform *restrict transform, sky_pos *out);


// in refract.c

/// @ingroup refract
double novas_standard_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);

/// @ingroup refract
double novas_optical_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);

/// @ingroup refract
double novas_radio_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);

/// @c_refract
double novas_inv_refract(RefractionModel model, double jd_tt, const on_surface *restrict loc, enum novas_refraction_type type, double el0);

// ---------------------- Added in 1.2.0 -------------------------

// in target.c
/// @c_source
int make_redshifted_cat_entry(const char *name, double ra, double dec, double z, cat_entry *source);

/// @c_source
int make_redshifted_object(const char *name, double ra, double dec, double z, object *source);

// in grav.c
/// @c_spectral
double grav_redshift(double M_kg, double r_m);

// in spectral.c
/// @c_spectral
double novas_z2v(double z);

/// @c_spectral
double novas_v2z(double vel);

/// @c_spectral
double redshift_vrad(double vrad, double z);

/// @c_spectral
double unredshift_vrad(double vrad, double z);

/// @c_spectral
double novas_z_add(double z1, double z2);

/// @c_spectral
double novas_z_inv(double z);

// in system.c
/// @c_equatorial
int gcrs_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out);

/// @c_equatorial
int tod_to_gcrs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out);

/// @c_equatorial
int gcrs_to_mod(double jd_tdb, const double *in, double *out);

/// @c_equatorial
int mod_to_gcrs(double jd_tdb, const double *in, double *out);

/// @ingroup solar-system
novas_planet_provider get_planet_provider();

/// @ingroup solar-system
novas_planet_provider_hp get_planet_provider_hp();

// in naif.c
/// @ingroup source
enum novas_planet naif_to_novas_planet(long id);

/// @ingroup source
long novas_to_naif_planet(enum novas_planet id);

/// @ingroup source
long novas_to_dexxx_planet(enum novas_planet id);

/// @c_source
enum novas_planet novas_planet_for_name(const char *restrict name);

// ---------------------- Added in 1.3.0 -------------------------

// in calendar.c
/// @c_time
int novas_jd_to_date(double tjd, enum novas_calendar_type calendar, int *restrict year, int *restrict month,
        int *restrict day, double *restrict hour);

/// @c_time
double novas_jd_from_date(enum novas_calendar_type calendar, int year, int month, int day, double hour);

// in spectral.c
/// @c_spectral
double novas_lsr_to_ssb_vel(double epoch, double ra, double dec, double vLSR);

/// @c_spectral
double novas_ssb_to_lsr_vel(double epoch, double ra, double dec, double vLSR);

// in observer.c
/// @ingroup nonequatorial
double novas_hpa(double az, double el, double lat);

/// @ingroup nonequatorial
double novas_epa(double ha, double dec, double lat);

/// @ingroup nonequatorial
int novas_h2e_offset(double daz, double del, double pa, double *restrict dra, double *restrict ddec);

/// @ingroup nonequatorial
int novas_e2h_offset(double dra, double ddec, double pa, double *restrict daz, double *restrict del);

/// @ingroup nonequatorial
int novas_los_to_xyz(const double *los, double lon, double lat, double *xyz);

/// @ingroup nonequatorial
int novas_xyz_to_los(const double *xyz, double lon, double lat, double *los);

/// @ingroup nonequatorial
int novas_xyz_to_uvw(const double *xyz, double ha, double dec, double *uvw);

/// @ingroup nonequatorial
int novas_uvw_to_xyz(const double *uvw, double ha, double dec, double *xyz);

// in util.c
/// @c_apparent
double novas_sep(double lon1, double lat1, double lon2, double lat2);

/// @c_apparent
double novas_equ_sep(double ra1, double dec1, double ra2, double dec2);

// in target.c
/// @c_source
int make_cat_object_sys(const cat_entry *star, const char *restrict system, object *source);

/// @c_source
int make_redshifted_object_sys(const char *name, double ra, double dec, const char *restrict system, double z, object *source);

// in parse.c
/// @c_time
double novas_epoch(const char *restrict system);

/// @c_util
double novas_hms_hours(const char *restrict hms);

/// @c_util
double novas_dms_degrees(const char *restrict dms);

/// @c_util
double novas_str_hours(const char *restrict hms);

/// @c_util
double novas_str_degrees(const char *restrict dms);

/// @ingroup util
double novas_parse_hms(const char *restrict str, char **restrict tail);

/// @ingroup util
double novas_parse_dms(const char *restrict str, char **restrict tail);

/// @ingroup util
double novas_parse_degrees(const char *restrict str, char **restrict tail);

/// @ingroup util
double novas_parse_hours(const char *restrict str, char **restrict tail);

// in frames.c
/// @c_time
double novas_frame_lst(const novas_frame *restrict frame);

/// @c_time
double novas_transit_time(const object *restrict source, const novas_frame *restrict frame);

/// @c_time
double novas_rises_above(double el, const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model);

/// @c_time
double novas_sets_below(double el, const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model);

/// @c_apparent
double novas_object_sep(const object *source1, const object *source2, const novas_frame *restrict frame);

/// @c_apparent
int novas_equ_track(const object *restrict source, const novas_frame *restrict frame, double dt, novas_track *restrict track);

/// @c_apparent
int novas_hor_track(const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model,
        novas_track *restrict track);

/// @c_apparent
int novas_track_pos(const novas_track *track, const novas_timespec *time, double *restrict lon, double *restrict lat,
        double *restrict dist, double *restrict z);

// in timescale.c
/// @c_time
double novas_date(const char *restrict date);

/// @c_time
double novas_date_scale(const char *restrict date, enum novas_timescale *restrict scale);

/// @ingroup util
double novas_parse_date(const char *restrict date, char **restrict tail);

/// @ingroup util
double novas_parse_iso_date(const char *restrict date, char **restrict tail);

/// @ingroup util
double novas_parse_date_format(enum novas_calendar_type calendar, enum novas_date_format format, const char *restrict date,
        char **restrict tail);

/// @c_time
int novas_iso_timestamp(const novas_timespec *restrict time, char *restrict dst, int maxlen);

/// @c_time
int novas_timestamp(const novas_timespec *restrict time, enum novas_timescale scale, char *restrict dst, int maxlen);

/// @ingroup time
enum novas_timescale novas_timescale_for_string(const char *restrict str);

/// @ingroup util
enum novas_timescale novas_parse_timescale(const char *restrict str, char **restrict tail);

/// @ingroup time
int novas_print_timescale(enum novas_timescale scale, char *restrict buf);

/// @c_util
int novas_print_hms(double hours, enum novas_separator_type sep, int decimals, char *restrict buf, int len);

/// @c_util
int novas_print_dms(double degrees, enum novas_separator_type sep, int decimals, char *restrict buf, int len);

// in ephemeris.c
/// @ingroup earth
novas_nutation_provider get_nutation_lp_provider();

// in orbital.c
/// @c_source
int novas_set_orbsys_pole(enum novas_reference_system type, double ra, double dec, novas_orbital_system *restrict sys);

/// @c_source
int make_orbital_object(const char *name, long num, const novas_orbital *orbit, object *body);

/// @c_source
int novas_orbit_posvel(double jd_tdb, const novas_orbital *restrict orbit, enum novas_accuracy accuracy,
        double *restrict pos, double *restrict vel);

// in target.c
/// @c_solar-system
double novas_helio_dist(double jd_tdb, const object *restrict source, double *restrict rate);

/// @c_solar-system
double novas_solar_power(double jd_tdb, const object *restrict source);

/// @c_apparent
double novas_solar_illum(const object *restrict source, const novas_frame *restrict frame);

/// @c_apparent
double novas_sun_angle(const object *restrict source, const novas_frame *restrict frame);

/// @c_apparent
double novas_moon_angle(const object *restrict source, const novas_frame *restrict frame);


// ---------------------- Added in 1.4.0 -------------------------

// in timescale.c
/// @c_earth
double novas_time_gst(const novas_timespec *restrict time, enum novas_accuracy accuracy);

/// @c_time
double novas_time_lst(const novas_timespec *restrict time, double lon, enum novas_accuracy accuracy);

/// @ingroup time
double tt2tdb_fp(double jd_tt, double limit);

/// @c_time
double tt2tdb_hp(double jd_tt);

// in refract.c
/// @ingroup refract
int novas_refract_wavelength(double microns);

/// @ingroup refract
double novas_wave_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);

// in calendar.c
/// @c_time
int novas_day_of_week(double tjd);

/// @c_time
int novas_day_of_year(double tjd, enum novas_calendar_type calendar, int *restrict year);

// in orbit.c
/// @c_geometric
int novas_orbit_native_posvel(double jd_tdb, const novas_orbital *restrict orbit, double *restrict pos, double *restrict vel);

// in planets.c
/// @c_source
int novas_make_planet_orbit(enum novas_planet id, double jd_tdb, novas_orbital *restrict orbit);

/// @ingroup source
int novas_make_moon_orbit(double jd_tdb, novas_orbital *restrict orbit);

/// @c_geometric
int novas_approx_heliocentric(enum novas_planet id, double jd_tdb, double *restrict pos, double *restrict vel);

/// @c_apparent
int novas_approx_sky_pos(enum novas_planet id, const novas_frame *restrict frame, enum novas_reference_system sys, sky_pos *restrict out);

/// @c_apparent
double novas_moon_phase(double jd_tdb);

/// @c_time
double novas_next_moon_phase(double phase, double jd_tdb);



// ---------------------- Added in 1.5.0 -------------------------

// in earth.c
/// @c_earth
double novas_gmst(double jd_ut1, double ut1_to_tt);

/// @c_earth
double novas_gast(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy);

/// @c_earth
int novas_diurnal_eop_at_time(const novas_timespec *restrict time, double *restrict dxp, double *restrict dyp, double *restrict dut1);

/// @c_earth
int novas_diurnal_eop(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict xp, double *restrict yp,
        double *restrict dut1);

/// @c_earth
int novas_diurnal_libration(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict xp, double *restrict yp,
        double *restrict dut1);

/// @c_earth
int novas_diurnal_ocean_tides(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict xp, double *restrict yp,
        double *restrict dut1);

// in itrf.c
/// @c_observer
int novas_itrf_transform(int from_year, const double *restrict from_coords, const double *restrict from_rates,
        int to_year, double *to_coords, double *to_rates);

/// @c_earth
int novas_itrf_transform_eop(int from_year, double from_xp, double from_yp, double from_dut1,
        int to_year, double *restrict to_xp, double *restrict to_yp, double *restrict to_dut1);

/// @c_observer
int novas_geodetic_to_cartesian(double lon, double lat, double alt, enum novas_reference_ellipsoid ellipsoid, double *xyz);

/// @c_observer
int novas_cartesian_to_geodetic(const double *restrict xyz, enum novas_reference_ellipsoid ellipsoid, double *restrict lon,
        double *restrict lat, double *restrict alt);

/// @c_observer
int novas_itrf_transform_site(int from_year, const on_surface *in, int to_year, on_surface *out);

/// @c_observer
int novas_geodetic_transform_site(enum novas_reference_ellipsoid from_ellipsoid, const on_surface *in,
        enum novas_reference_ellipsoid to_ellipsoid, on_surface *out);

// in timescale.c
/// @c_time
double novas_clock_skew(const novas_frame *frame, enum novas_timescale timescale);

/// @c_time
double novas_mean_clock_skew(const novas_frame *frame, enum novas_timescale timescale);

/// @c_time
int novas_set_current_time(int leap, double dut1, novas_timespec *restrict time);

/// @c_time
int novas_set_str_time(enum novas_timescale timescale, const char *restrict str, int leap, double dut1, novas_timespec *restrict time);

// in target.c
/// @c_source
int novas_init_cat_entry(cat_entry *restrict source, const char *restrict name, double ra, double dec);

/// @c_source
int novas_set_catalog(cat_entry *restrict source, const char *restrict catalog, long num);

/// @c_source
int novas_set_ssb_vel(cat_entry *source, double v_kms);

/// @c_source
int novas_set_lsr_vel(cat_entry *source, double epoch, double v_kms);

/// @c_source
int novas_set_redshift(cat_entry *source, double z);

/// @c_source
int novas_set_proper_motion(cat_entry *source, double pm_ra, double pm_dec);

/// @c_source
int novas_set_parallax(cat_entry *source, double mas);

/// @c_source
int novas_set_distance(cat_entry *source, double parsecs);

// in observer.c
/// @c_observer
int make_itrf_observer(double latitude, double longitude, double height, observer *obs);

/// @c_observer
int make_gps_observer(double latitude, double longitude, double height, observer *obs);

/// @c_observer
int make_observer_at_site(const on_surface *restrict site, observer *restrict obs);

/// @c_observer
int make_itrf_site(double latitude, double longitude, double height, on_surface *site);

/// @c_observer
int make_gps_site(double latitude, double longitude, double height, on_surface *site);

/// @c_observer
int make_xyz_site(const double *restrict xyz, on_surface *restrict site);

/// @c_observer
int novas_set_default_weather(on_surface *site);


// ---------------------- Added in 1.5.1 -------------------------

/// @ingroup source
int novas_make_moon_mean_orbit(double jd_tdb, novas_orbital *restrict orbit);

/// \endcond


// ---------------------- Added in 1.6.0 -------------------------


// in system.c
/// @ingroup observer
int novas_itrs_to_enu(const double *itrf, double lon, double lat, double *enu);

/// @ingroup observer
int novas_enu_to_itrs(const double *enu, double lon, double lat, double *itrf);


// <================= END of SuperNOVAS API =====================>


// <================= SuperNOVAS internals ======================>

/// \cond PRIVATE
#ifdef __NOVAS_INTERNAL_API__

#ifndef _CONSTS_
#  define _CONSTS_

#  define HALF_PI             (0.5 * M_PI)
#  define NOVAS_C2            (NOVAS_C * NOVAS_C)   ///< [m<sup>2</sup>/s<sup>2</sup>] Speed of light square
#  define XYZ_VECTOR_SIZE     (3 * sizeof(double))

// Use shorthand definitions for our constants
#  define JD_J2000            NOVAS_JD_J2000
#  define C_AUDAY             NOVAS_C_AU_PER_DAY
#  define AU_KM               NOVAS_AU_KM
#  define GS                  NOVAS_G_SUN
#  define GE                  NOVAS_G_EARTH


// Various locally used physical units
#  define DAY                 NOVAS_DAY
#  define DAY_HOURS           24.0
#  define DEG360              360.0
#  define JULIAN_YEAR_DAYS    NOVAS_JULIAN_YEAR_DAYS
#  define JULIAN_CENTURY_DAYS ( 100.0 * JULIAN_YEAR_DAYS )
#  define ARCSEC              NOVAS_ARCSEC
#  define DEGREE              NOVAS_DEGREE
#  define HOURANGLE           NOVAS_HOURANGLE
#  define MAS                 (1e-3 * ASEC2RAD)
#  define KMS                 NOVAS_KMS

#endif /* _CONSTS_ */

/// NAIF ID for the geocenter
#define NAIF_EARTH      399

/// NAIF ID for the Moon
#define NAIF_MOON       301

/// NAIF_ID for the Sun
#define NAIF_SUN        10

/// NAIF ID for the Solar-System Barycenter (SSB)
#define NAIF_SSB        0

/// NAIF ID for the Earth-Moon Barycenter (EMB)
#define NAIF_EMB        3

/// NAIF ID for the barycenter of the Pluto system
#define NAIF_PLUTO_BARYCENTER   9


/**
 * Default value for the maximum number of iterations allowed for inverse calculations.
 * @since 1.5
 * @sa novas_set_max_iter()
 */
#define NOVAS_DEFAULT_MAX_ITER    100

// On some older platform NAN may not be defined, so define it here if need be
#  ifndef NAN
#    define NAN               (0.0/0.0)
#  endif


#  ifndef THREAD_LOCAL
#    if __STDC_VERSION__ >= 202311L
#      define THREAD_LOCAL thread_local           ///< C23 standard for thread-local variables
#    elif __STDC_VERSION__ >= 201112L
#      define THREAD_LOCAL _Thread_local          ///< C11 standard for thread-local variables
#    elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#      define THREAD_LOCAL __thread               ///< pre C11 gcc >= 3.3 standard for thread-local variables
#    elif defined _MSVC_VER
#      define THREAD_LOCAL __declspec( thread )   ///< Microsoft Visual C thread local declaration
#    else
#      define THREAD_LOCAL                        ///< no thread-local variables
#    endif
#  endif


int novas_trace(const char *restrict loc, int n, int offset);
double novas_trace_nan(const char *restrict loc);
double novas_check_nan(const char *loc, double x);
void novas_trace_invalid(const char *loc);
void novas_set_errno(int en, const char *restrict from, const char *restrict desc, ...);
int novas_error(int ret, int en, const char *restrict from, const char *restrict desc, ...);

/**
 * Propagates an error (if any) with an offset. If the error is non-zero, it returns with the offset
 * error value. Otherwise it keeps going as if it weren't even there...
 *
 * @param loc   {string} function / location from where it's called
 * @param n     {int} error code or the call that produces the error code
 * @param d     {int} offset with which the error is propagated
 *
 * @sa error_return()
 */
#  define prop_error(loc, n, d) { \
        int __ret = novas_trace(loc, n, d); \
        if (__ret != 0) \
        return __ret; \
}

/**
 * Returns NaN if there was an error
 *
 * @param loc   {string} function / location from where it's called
 * @param n     {int} error code or the call that produces the error code
 *
 * @sa trace_nan()
 */
#  define prop_nan(loc, n) { \
        if (n) { \
          novas_trace_nan(loc); \
          return NAN; \
        } \
}



double novas_norm_ang(double angle);
int novas_time_equals(double jd1, double jd2);
int novas_time_equals_hp(double jd1, double jd2);
void novas_tiny_rotate(const double *in, double ax, double ay, double az, double *out);

double novas_add_beta(double beta1, double beta2);
double novas_add_vel(double v1, double v2);

double novas_vlen(const double *restrict v);
double novas_vdist(const double *v1, const double *v2);
double novas_vdist2(const double *v1, const double *v2);
double novas_vdot(const double *v1, const double *v2);

int polar_dxdy_to_dpsideps(double jd_tt, double dx, double dy, double *restrict dpsi, double *restrict deps);
int novas_frame_is_initialized(const novas_frame *frame);
double novas_gmst_prec(double jd_tdb);
double novas_cio_gcrs_ra(double jd_tdb);
void novas_set_max_iter(int n);

int novas_Rx(double angle, double *v);
int novas_Ry(double angle, double *v);
int novas_Rz(double angle, double *v);


/**
 * Deprecated.
 * @deprecated Use novas_set_max_iter() instead
 * @sa novas_set_max_iter()
 */
extern int novas_inv_max_iter;

#endif /* __NOVAS_INTERNAL_API__ */
/// \endcond

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif // __cplusplus

#endif /* _NOVAS_ */
