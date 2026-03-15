/**
 * @file
 *
 * @date Created  on Jan 9, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for determining positions for
 *  nearby (non-high-z) sidereal sources, such as a star.
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
  cat_entry star = CAT_ENTRY_INIT;  // catalog information about a sidereal source
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


  // -------------------------------------------------------------------------
  // Define a sidereal source

  // Let's assume we have B1950 (FK4) coordinates...
  // 16h26m20.1918s, -26d19m23.138s (B1950), proper motion -12.11, -23.30 mas/year,
  // parallax 5.89 mas, radial velocity -3.4 km/s.
  //
  // NOTE, that make_cat_entry() expects radial velocities defined relative to the
  // Solar-System Barycenter (SSB). But you can convert LSR-based velocities to
  // the required SSB-based radial velocities using novas_lsr_to_ssb_vel() if needed.

  // E.g. initialize with string coordinates to hours/degrees...
  if(novas_init_cat_entry(&star, "Antares", novas_str_hours("16h26m20.1918s"), novas_str_degrees("-26d19m23.138s")) != 0) {
    fprintf(stderr, "ERROR! defining cat_entry.\n");
    return 1;
  }

  // Optionally, we might store the catalog information: catalog ID (5-chars max) and number.
  novas_set_catalog(&star, "HIP", 80763);

  // Set the proper motion
  novas_set_proper_motion(&star, -12.11, -23.30);

  // Set the parallax. Alternatively we could set distance with `novas_set_distance()` instead.
  novas_set_parallax(&star, 5.89);

  // Set radial velocity. Or we could set LSR velocity with 'novas_set_lsr_vel()' instead
  novas_set_ssb_vel(&star, -3.4);


  // -------------------------------------------------------------------------
  // Convert to ICRS coordinates and wrap in a generic object structure.
  if(make_cat_object_sys(&star, "B1950", &source) != 0) {
    fprintf(stderr, "ERROR! configuring observed object\n");
    return 1;
  }


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
  // You might want to set a provider for precise planet positions so we might
  // calculate Earth, Sun and major planet positions accurately. If an planet
  // provider is configured, we can unlock the ultimate (sub-uas) accuracy of
  // SuperNOVAS.
  //
  // There are many ways to set a provider of planet positions. For example,
  // you may use the CALCEPH library:
  //
  // t_calcephbin *planets = calceph_open("path/to/de440s.bsp");
  // novas_use_calceph(planets);
  //
  // accuracy = NOVAS_FULL_ACCURACY;      // sub-uas precision

  // Without a planet provider, we are stuck with reduced (mas) precisions
  // only...
  accuracy = NOVAS_REDUCED_ACCURACY;      // mas-level precision, typically


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

