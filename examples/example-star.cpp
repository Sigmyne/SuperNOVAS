/**
 * @file
 *
 * @date Created  on Jan 9, 2026
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C++ library for determining positions for
 *  nearby (non-high-z) sidereal sources, such as a star.
 *
 *  Link with
 *
 *  ```
 *   -lsupernovas++
 *  ```
 */

#include <iostream>

#include <supernovas.h>      ///< SuperNOVAS functions and definitions

using namespace novas;
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
  // Define a sidereal source

  // Let's assume we have B1950 (FK4) coordinates...
  // 16h26m20.1918s, -26d19m23.138s (B1950), proper motion -12.11, -23.30 mas/year,
  // parallax 5.89 mas, radial velocity -3.4 km/s.
  //
  // NOTE, here we set a barycentric radial velocity, but you can set LSR velocities or redshifts
  // also, instead.
  CatalogEntry e = CatalogEntry("Antares", Equatorial("16h26m20.1918s", "-26d19m23.138s", Equinox::b1950()))
          .proper_motion(-12.11 * Unit::mas / Unit::yr, -23.30 * Unit::mas / Unit::yr)
          .parallax(5.98 * Unit::mas)
          .radial_velocity(-3.4 * Unit::km / Unit::s);

  // Define a source from the catalog coordinates
  CatalogSource source(e);



  // -------------------------------------------------------------------------
  // Define observer somewhere on Earth (we can also define observers in Earth
  // or Sun orbit, at the geocenter or at the Solary-system barycenter...)

  // Specify the location we are observing from
  // 50.7374 deg N, 7.0982 deg E, 60m elevation (GPS / WGS84)
  // (You can set local weather parameters after...)
  GeodeticObserver obs = Observer::on_earth(Site::from_GPS(50.7374, 7.0982, 60.0), eop);


  // -------------------------------------------------------------------------
  // Set the astrometric time of observation...

  // Set the time of observation to the current UTC-based UNIX time
  Time t = Time::now(eop)

  // ... Or you could set a time from a string calendar date
  /*
  std::optional<CalendarDate> date = Calendar::gregorian().parse_date("2026-01-09 12:33:15.342+0200");
  if(!date.has_value()) {
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
  // Initialize the observing frame with the given observer location and
  // time of observation
  //
  // Without a planet provider, we are stuck with reduced (mas) precisions
  // only...
  Frame frame(obs, t, NOVAS_REDUCED_ACCURACY);

  // -------------------------------------------------------------------------
  // Calculate the precise apparent position.
  Apparent apparent = source.apparent(frame);

  // Let's print the apparent position
  std::cout << apparent.to_string()) << "\n";

  // -------------------------------------------------------------------------
  // Convert the apparent position on sky to horizontal coordinates
  // We'll use an optical refraction model with local weather parameters...
  // (6 C deg, 985 mbar, 74% humidity)
  Weather weather(Temperature::celsius(6.0), Pressure::mbar(985.0), 74.0);

  Horizontal hor = apparent.to_horizontal().value()
          .to_refracted(frame, novas_optical_refraction, weather);

  // Let's print the calculated azimuth and elevation
  std::cout << hor.to_string()) << "\n";

  return 0;
}

