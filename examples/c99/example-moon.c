/**
 * @file
 *
 * @date Created  on Jan 9, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for calculating the
 *  position or the phase of the Moon, using the ELP2000 / MPP02 model by
 *  Chapront &amp; Francou (2003)
 *
 *  Link with:
 *
 *  ```
 *   -lsupernovas
 *  ```
 *
 */

#define _POSIX_C_SOURCE 199309L   ///< for clock_gettime()

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <novas.h>      ///< SuperNOVAS functions and definitions

// Below are some Earth orientation values. Here we define them as constants, but they may
// of course be variables. They should be set to the appropriate values for the time
// of observation based on the IERS Bulletins...

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.

int main() {
  // SuperNOVAS variables used for the calculations ------------------------->
  observer obs;                     // observer location
  novas_timespec obs_time;          // astrometric time of observation
  novas_frame obs_frame;            // observing frame defined for observing time and location
  enum novas_accuracy accuracy;     // NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
  sky_pos apparent;                 // calculated precise observed (apparent) position of source

  // Calculated quantities ------------------------------------------------->
  double az, el;                    // calculated azimuth and elevation at observing site
  double jd_tdb_full;               // (TDB-based) Julian date of next full moon
  novas_timespec full_time = {};    // calculated astrometric time of next full moon
  char ts[40] = {'\0'};             // timestamp string

  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);


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
  // novas_use_calceph_planets(planets);
  //
  // accuracy = NOVAS_FULL_ACCURACY;      // sub-uas precision


  // -------------------------------------------------------------------------
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
  // Calculate geometric the position and velocity of the Moon relative to the
  // observer (e.g. in TOD)
  //
  // double pos[3] = {0.0}, vel[3] = {0.0}; // [AU, AU/day]
  //
  //novas_moon_elp_posvel(&obs_frame, NOVAS_TOD, pos, vel);

  // -------------------------------------------------------------------------
  // Calculate the apparent position (e.g. in CIRS).
  novas_moon_elp_sky_pos(&obs_frame, NOVAS_CIRS, &apparent);

  // Let's print the apparent position in CIRS
  // (Note, CIRS R.A. is relative to CIO, not the true equinox of date.)
  printf("Moon:\n");
  printf("  RA = %.9f h, Dec = %.9f deg, rv = %.3f m.s\n", apparent.ra, apparent.dec, 1000.0 * apparent.rv);

  // -------------------------------------------------------------------------
  // Convert the apparent position in CIRS on sky to horizontal coordinates
  // We'll use a standard (fixed) atmospheric model to estimate an optical refraction
  // (You might use other refraction models, or NULL to ignore refraction corrections)
  if(novas_app_to_hor(&obs_frame, NOVAS_CIRS, apparent.ra, apparent.dec, novas_standard_refraction, &az, &el) != 0) {
    fprintf(stderr, "ERROR! failed to calculate azimuth / elevation.\n");
    return 1;
  }

  // Let's print the calculated azimuth and elevation
  printf("  Az = %.6f deg, El = %.6f deg\n", az, el);


  // -------------------------------------------------------------------------
  // Let's print the phase of the moon

  printf(" phase = %.3f deg\n", novas_moon_phase(novas_get_time(&obs_time, NOVAS_TDB)));


  // -------------------------------------------------------------------------
  // Let's figure out when the next full moon is

  // TDB-based JUlian date of next full moon (phase = 180 deg).
  jd_tdb_full = novas_next_moon_phase(180.0, novas_get_time(&obs_time, NOVAS_TDB));

  // Set astrometric time of full-moon to TDB-based Julian Date
  novas_set_time(NOVAS_TDB, jd_tdb_full, LEAP_SECONDS, DUT1, &full_time);

  // Print timestamp of full-moon
  novas_timestamp(&full_time, NOVAS_UTC, ts, sizeof(ts));
  printf(" next full moon is at %s\n", ts);


  // -------------------------------------------------------------------------
  // Clean up before we exit...
  //calceph_close(planets);

  return 0;
}

