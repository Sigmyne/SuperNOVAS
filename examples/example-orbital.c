/**
 * @file
 *
 * @date Created  on Jan 9, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for determining positions for
 *  Solar-system objects define through a set of orbital parameters.
 *
 *  For example, the IAU Minor Planet Center (MPC) publishes current orbital
 *  parameters for known asteroids, comets, and near-Earth objects. While orbitals are
 *  not super precise in general, they can provide sufficienly accurate positions on
 *  the arcsecond level (or below), and may be the best/only source of position data
 *  for newly discovered objects.
 *
 *  See https://minorplanetcenter.net/data
 *
 *  Link with
 *
 *  ```
 *   -lsupernovas
 *  ```
 */

#define _POSIX_C_SOURCE 199309L   ///< for clock_gettime()

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <novas.h>      ///< SuperNOVAS functions and definitions

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
using namespace novas;
#  endif
#endif

// Below are some Earth orientation values. Here we define them as constants, but they may
// of course be variables. They should be set to the appropriate values for the time
// of observation based on the IERS Bulletins...

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.

int main() {
  // SuperNOVAS variables used for the calculations ------------------------->
  novas_orbital orbit = NOVAS_ORBIT_INIT;     // Orbital parameters
  object source;                    // a celestial object: sidereal, planet, ephemeris or orbital source
  observer obs;                     // observer location
  novas_timespec obs_time;          // astrometric time of observation
  novas_frame obs_frame;            // observing frame defined for observing time and location
  enum novas_accuracy accuracy;     // NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
  sky_pos apparent;                 // calculated precise observed (apparent) position of source

  // Calculated quantities ------------------------------------------------->
  double az, el;                    // calculated azimuth and elevation at observing site

  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);


  // Orbitals assume Keplerian motion, and are never going to be accurate much below the
  // tens of arcsec level even for the most current MPC orbits. Orbitals for planetary
  // satellites are even less precise. So, with orbitals, there is no point on pressing
  // for ultra-high (sub-uas level) accuracy...
  accuracy = NOVAS_REDUCED_ACCURACY;      // mas-level precision, typically


  // -------------------------------------------------------------------------
  // Define a sidereal source

  // Orbital Parameters for the asteroid Ceres from the Minor Planet Center
  // (MPC) at JD 2460600.5
  orbit.jd_tdb = 2460600.5;   // [day] TDB date
  orbit.a = 2.7666197;        // [AU]
  orbit.e = 0.079184;
  orbit.i = 10.5879;          // [deg]
  orbit.omega = 73.28579;     // [deg]
  orbit.Omega = 80.25414;     // [deg]
  orbit.M0 = 145.84905;       // [deg]
  orbit.n = 0.21418047;       // [deg/day]

  // Define Ceres as the observed object (we can use whatever ID numbering
  // system here, since it's irrelevant to SuperNOVAS in this context).
  make_orbital_object("Ceres", 2000001, &orbit, &source);


  // ... Or, you could define orbitals for a satellite instead:
  /*
  // E.g. Callisto's orbital parameters from JPL Horizons
  // https://ssd.jpl.nasa.gov/sats/elem/sep.html
  // 1882700. 0.007 43.8  87.4  0.3 309.1 16.690440 277.921 577.264 268.7 64.8
  orbit.system.center = NOVAS_JUPITER;
  novas_set_orbsys_pole(NOVAS_GCRS, 268.7 / 15.0, 64.8, &orbit->system);

  orbit.jd_tdb = NOVAS_JD_J2000;
  orbit.a = 1882700.0 * 1e3 / AU;
  orbit.e = 0.007;
  orbit.omega = 43.8;
  orbit.M0 = 87.4;
  orbit.i = 0.3;
  orbit.Omega = 309.1;
  orbit.n = TWOPI / 16.690440;
  orbit.apsis_period = 277.921 * 365.25;
  orbit.node_period = 577.264 * 365.25;

  // Set Callisto as the observed object
  make_orbital_object("Callisto", 501, &orbit, &source);
  */


  // -------------------------------------------------------------------------
  // Define observer somewhere on Earth (we can also define observers in Earth
  // or Sun orbit, at the geocenter or at the Solary-system barycenter...)

  // Specify the location we are observing from
  // 50.7374 deg N, 7.0982 deg E, 60m elevation (GPS / WGS84)
  // (You can set local weather parameters after...)
  if(make_gps_observer(50.7374, 7.0982, 60.0, &obs) != 0) {
    fprintf(stderr, "ERROR! defining Earth-based observer location.\n");
    return 1;
  }


  // -------------------------------------------------------------------------
  // Set the astrometric time of observation...

  // Set the time of observation to the current UTC-based UNIX time
  // (We can set astromtric time using an other time measure also...)
  if(novas_set_current_time(LEAP_SECONDS, DUT1, &obs_time) != 0) {
    fprintf(stderr, "ERROR! failed to set time of observation.\n");
    return 1;
  }

  // ... Or you could set a time explicily in any known timescale.
  /*
  // Let's set a TDB-based time for the start of the J2000 epoch exactly...
  if(novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32, 0.0, &obs_time) != 0) {
    fprintf(stderr, "ERROR! failed to set time of observation.\n");
    return 1;
  }
  */


  // -------------------------------------------------------------------------
  // You might want to set a provider for precise planet positions so we might
  // calculate Earth, Sun and major planet positions accurately. It is needed
  // if you have orbitals defined around a major planet.
  //
  // There are many ways to set a provider of planet positions. For example,
  // you may use the CALCEPH library:
  //
  // t_calcephbin *planets = calceph_open("path/to/de440s.bsp");
  // novas_use_calceph(planets);


  // -------------------------------------------------------------------------
  // Initialize the observing frame with the given observing and Earth
  // orientation patameters.
  //
  if(novas_make_frame(accuracy, &obs, &obs_time, POLAR_DX, POLAR_DY, &obs_frame) != 0) {
    fprintf(stderr, "ERROR! failed to define observing frame.\n");
    return 1;
  }


  // -------------------------------------------------------------------------
  // Calculate the precise apparent position (e.g. in CIRS).
  if(novas_sky_pos(&source, &obs_frame, NOVAS_CIRS, &apparent) != 0) {
    fprintf(stderr, "ERROR! failed to calculate apparent position.\n");
    return 1;
  }

  // Let's print the apparent position
  // (Note, CIRS R.A. is relative to CIO, not the true equinox of date.)
  printf(" RA = %.9f h, Dec = %.9f deg, rad_vel = %.6f km/s\n", apparent.ra, apparent.dec, apparent.rv);


  // -------------------------------------------------------------------------
  // Convert the apparent position in CIRS on sky to horizontal coordinates
  // We'll use a standard (fixed) atmospheric model to estimate an optical refraction
  // (You might use other refraction models, or NULL to ignore refraction corrections)
  if(novas_app_to_hor(&obs_frame, NOVAS_CIRS, apparent.ra, apparent.dec, novas_standard_refraction, &az, &el) != 0) {
    fprintf(stderr, "ERROR! failed to calculate azimuth / elevation.\n");
    return 1;
  }

  // Let's print the calculated azimuth and elevation
  printf(" Az = %.6f deg, El = %.6f deg\n", az, el);

  return 0;
}

