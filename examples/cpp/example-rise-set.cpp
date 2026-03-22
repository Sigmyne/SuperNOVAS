/**
 * @file
 *
 * @date Created  on Jan 9, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for checking rise, set, or transit times
 *  for sources observed from Earth's surface or airspace.
 *
 *  Link with
 *
 *  ```
 *   -lsupernovas -lsupernovas++
 *  ```
 */

#include <iostream>

#include <supernovas.h>      ///< SuperNOVAS functions and definitions

using namespace supernovas;

// Below are some Earth orientation values. Here we define them as constants, but they may
// of course be variables. They should be set to the appropriate values for the time
// of observation based on the IERS Bulletins...

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.

/*
 * example-rise-set [elevation]
 *
 * Arguments:
 *
 *   elevation    [deg] elevation angle (default 0.0).
 *
 */
int main(int argc, const char *argv[]) {
  // Input parameters
  Angle el = Angle(0.0);            // [deg] elevation angle (set via command-line argument)

  // Check if called with argument.
  if(argc > 1)
    el = Angle(strtod(argv[1], NULL) * Unit::deg);

  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);


  // -------------------------------------------------------------------------
  // Earth orientation parameters (EOP), as appropriate for the time of observation,
  // e.g. as obtained from IERS bulletins or data service:
  EOP eop(LEAP_SECONDS, DUT1, POLAR_DX * Unit::mas, POLAR_DY * Unit::mas);

  // -------------------------------------------------------------------------
  // Define a source
  // (We'll use the Sun, but see other examples for other types of sources...

  auto source = Planet::sun();



  // -------------------------------------------------------------------------
  // Define observer somewhere on Earth (we can also define observers in Earth
  // or Sun orbit, at the geocenter or at the Solary-system barycenter...)

  // Specify the location we are observing from
  // 50.7374 deg N, 7.0982 deg E, 60m elevation (GPS / WGS84)
  // (You can set local weather parameters after...)
  auto obs = Observer::on_earth(Site::from_GPS(50.7374, 7.0982, 60.0), eop);



  // -------------------------------------------------------------------------
  // Set the astrometric time of observation...

  // Set the time of observation to the current UTC-based UNIX time
  Time t = Time::now(eop);

  // ... Or you could set a time from a string calendar date
  /*
      CalendarDate date = Calendar::gregorian().parse_date("2026-01-09 12:33:15.342+0200");
      if(!date) {
        std::cerr << "ERROR! could not parse date string.\n";
        return 1;
      }
      Time t = date.value().to_time(eop, NOVAS_UTC);
   */

  // ... Or you could set a time as a Julian date any known timescale.
  //Time t(NOVAS_JD_J2000, 32, 0.0);

  // ... Or you could set a time via a POSIX timespec.
  //struct timespec ts = ...;     // the POSIX time specification
  //Time t(&ts, eop);



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

  // -------------------------------------------------------------------------
  // Without a planet provider, we are stuck with reduced (mas) precisions
  // only...
  enum novas_accuracy accuracy = NOVAS_REDUCED_ACCURACY;

  // -------------------------------------------------------------------------
  // Initialize the observing frame with the given observer location and
  // time of observation. Without a planet provider, we are stuck with reduced
  // (mas) precisions only...
  auto frame = obs.frame_at(t, accuracy);

  // -------------------------------------------------------------------------
  // Print source name to output
  std::cout << source.to_string() << "observed from " << obs.site().to_string() << ":\n";



  // -------------------------------------------------------------------------
  // Define local weather (for refraction correction)
  // We'll use an optical refraction model with local weather parameters...
  // (6 C deg, 985 mbar, 74% humidity)
  Weather weather(Temperature::celsius(6.0), Pressure::mbar(985.0), 74.0);



  // -------------------------------------------------------------------------
  // Calculate next UTC-based date/time source rises above 20 degrees elevation
  // (as corrected for optical refraction under a standard atmosphere)
  Time t_rise = source.rises_above(el, frame, novas_optical_refraction, weather);

  if(!t_rise) {
    std::cout << " will not rise above " << el.to_string() << " degrees\n";
  }
  else {
    std::cout << " will rise above " << el.to_string() << " degrees at  : " << t_rise.to_string() << "\n";
  }

  // -------------------------------------------------------------------------
  // Calculate next UTC-based date/time source transits at observer location
  Time t_transit = source.transits_in(frame);

  std::cout << " will transit at                   : " << t_transit.to_string() << "\n";


  // -------------------------------------------------------------------------
  // Calculate next UTC-based date/time source sets below 20 degrees elevation
  // (as corrected for optical refraction under a standard atmosphere)
  Time t_set = source.sets_below(el, frame, novas_optical_refraction, weather);

  if(!t_set) {
    std::cout << " will not set below " << el.to_string() << " degrees\n";
  }
  else {
    std::cout << " will set below " << el.to_string() << " degrees at  : " << t_rise.to_string() << "\n";
  }

  return 0;
}

