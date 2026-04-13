/**
 * @file
 *
 * @date Created  on Jan 12, 2026
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for calculating the
 *  position or the phase of the Moon, using the ELP2000 / MPP02 model by
 *  Chapront &amp; Francou (2003)
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
// of observation based on the IERS Bulletins or data service...

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.

int main() {
  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);


  // -------------------------------------------------------------------------
  // Earth orientation parameters (EOP), as appropriate for the time of observation,
  // e.g. as obtained from IERS bulletins or data service:
  EOP eop(LEAP_SECONDS, DUT1, POLAR_DX * Unit::mas, POLAR_DY * Unit::mas);


  // -------------------------------------------------------------------------
  // Define observer somewhere on Earth (we can also define observers in Earth
  // or Sun orbit, at the geocenter or at the Solary-system barycenter...)

  // Specify the location we are observing from
  // 50.7374 deg N, 7.0982 deg E, 60m elevation (GPS / WGS84)
  // (You can set local weather parameters after...)
  auto obs = Observer::on_earth(Site::from_GPS(7.0982 * Unit::deg, 50.7374 * Unit::deg, 60.0 * Unit::m), eop);


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
  // novas_use_calceph_planets(planets);


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
  // Calculate the precise geometric position velocity vectors of the Moon
  // w.r.t. the observer
  //Geometric g = frame.geometric_moon_elp2000();


  // -------------------------------------------------------------------------
  // Calculate the precise apparent position of the Moon
  Apparent apparent = frame.apparent_moon_elp2000();

  // Let's print the apparent position
  std::cout << "Moon:\n";
  std::cout << "  " << apparent.equatorial().to_string() << ", " << apparent.radial_velocity().to_string() << "\n";


  // -------------------------------------------------------------------------
  // Convert the apparent position on sky to horizontal coordinates
  // We'll use an optical refraction model with local weather parameters...
  // (6 C deg, 985 mbar, 74% humidity)
  Weather weather(Temperature::celsius(6.0), Pressure::mbar(985.0), 74.0 * Unit::percent);

  Horizontal hor = apparent.to_horizontal().to_refracted(novas_optical_refraction, weather);
  if(!hor) {
    std::cerr << "  ERROR! observer has no Earth-based horizon.";
    return 1;
  }

  // Let's print the calculated azimuth and elevation
  std::cout << "  " << hor.to_string() << "\n";


  // -------------------------------------------------------------------------
  // Let's print the phase of the moon
  std::cout << "  phase = " <<  t.moon_phase().deg() << " deg\n";


  // -------------------------------------------------------------------------
  // Let's figure out when the next full moon is
  std::cout << "  next full moon is at " << t.next_moon_phase(Angle(180.0 * Unit::deg)).to_string() << "\n";


  // -------------------------------------------------------------------------
  // Clean up before we exit...
  //calceph_close(planets);

  return 0;
}

