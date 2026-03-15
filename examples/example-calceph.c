/**
 * @file
 *
 * @date Created  on Jan 9, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for determining positions for
 *  Solary-system sources, with the CALCEPH C library providing access to ephemeris
 *  files.
 *
 *  You will need access to the CALCEPH library (unversioned `libcalceph.so` or else
 *  `libcalceph.a`) and C headers (`calceph.h`), and the SuperNOVAS
 *  `libsolsys-calceph.so` (or `libsolsys-calceph.a`) module.
 *
 *  Link with:
 *
 *  ```
 *   -lsupernovas -lsolsys-calceph -lcalceph
 *  ```
 */

#define _POSIX_C_SOURCE 199309L   ///< for clock_gettime()

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <novas.h>            ///< SuperNOVAS functions and definitions
#include <novas-calceph.h>    ///< CALCEPH adapter functions to SuperNOVAS

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

int main(int argc, char *argv[]) {
  // Program Options -------------------------------------------------------->
  char *datafile = "/path/to/de440s.bsp";  // Ephemeris file to use

  // SuperNOVAS variables used for the calculations ------------------------->
  object source;                    // observed source
  observer obs;                     // observer location
  novas_timespec obs_time;          // astrometric time of observation
  novas_frame obs_frame;            // observing frame defined for observing time and location
  enum novas_accuracy accuracy;     // NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
  sky_pos apparent;                 // calculated precise observed (apparent) position of source

  // Calculated quantities ------------------------------------------------->
  double az, el;                    // calculated azimuth and elevation at observing site

  // Intermediate variables we'll use -------------------------------------->
  t_calcephbin *de440;              // CALCEPH ephemeris binary


  // Command line argument can define the ephemeris data to use.
  if(argc > 1)
    datafile = argv[1];

  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);

  // -------------------------------------------------------------------------
  // We'll use the CALCEPH library to provide ephemeris data

  // First open one or more ephemeris files with CALCEPH to use
  // E.g. the DE440 (short-term) ephemeris data from JPL.
  de440 = calceph_open(datafile);
  if(!de440) {
    fprintf(stderr, "ERROR! could not open ephemeris data\n");
    return 1;
  }

  // Make de440 provide ephemeris data for the major planets.
  novas_use_calceph_planets(de440);

  // We could specify to use a calceph ephemeris binary for generic
  // Solar-systems sources also (including planets too if
  // novas_use_calceph_planets() is not called separately
  //
  // E.g. Jovian satellites:
  // t_calcephbin jovian = calceph_open("/path/to/jup365.bsp");
  // novas_use_calceph(jovian);

  // Since we have an ephemeris provider for major planets, we can unlock the
  // ultimate accuracy of SuperNOVAS.
  accuracy = NOVAS_FULL_ACCURACY;      // sub-uas precision


  // -------------------------------------------------------------------------
  // Define a Solar-system source

  // To define a major planet (or Sun, Moon, SSB, or EMB):
  if(make_planet(NOVAS_MARS, &source) != 0) {
    fprintf(stderr, "ERROR! defining planet.\n");
    return 1;
  }

  // ... Or, to define a minor body, such as an asteroid or satellite
  // with a name and ID number.
  /*
  if(make_ephem_object("Io", 501, &source) != 0) {
    fprintf(stderr, "ERROR! defining ephemeris body.\n");
    return 1;
  }
  */

  /*
  // If the object uses CALCEPH IDs instead of NAIF, then
  novas_calceph_use_ids(NOVAS_ID_CALCEPH);
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

  // Let's print the apparent position in CIRS
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

