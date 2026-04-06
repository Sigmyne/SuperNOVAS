/**
 * @file
 *
 * @date Created  on Jan 9, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for determining positions for
 *  Solar-system sources, with the CALCEPH C library providing access to ephemeris
 *  files.
 *
 *  You will need access to the CALCEPH library (unversioned `libcalceph.so` or else
 *  `libcalceph.a`) and C headers (`calceph.h`), and the SuperNOVAS
 *  `libsolsys-calceph.so` (or `libsolsys-calceph.a`) module.
 *
 *  Link with:
 *
 *  ```
 *   -lsupernovas -lsupernovas++ -lsolsys-calceph -lcalceph
 *  ```
 */

#include <iostream>

#include <supernovas.h>       ///< SuperNOVAS functions and definitions
#include <novas-calceph.h>    ///< CALCEPH adapter functions to SuperNOVAS

using namespace supernovas;

// Below are some Earth orientation values. Here we define them as constants, but they may
// of course be variables. They should be set to the appropriate values for the time
// of observation based on the IERS Bulletins...

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.

int main(int argc, const char *argv[]) {
  // Program Options -------------------------------------------------------->
  std::string datafile = "/path/to/de440s.bsp";  // Ephemeris file to use


  // Intermediate variables we'll use -------------------------------------->
  t_calcephbin *de440;              // CALCEPH ephemeris binary


  // Command line argument can define the ephemeris data to use.
  if(argc > 1)
    datafile = std::string(argv[1]);

  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);


  // -------------------------------------------------------------------------
  // We'll use the CALCEPH library to provide ephemeris data

  // First open one or more ephemeris files with CALCEPH to use
  // E.g. the DE440 (short-term) ephemeris data from JPL.
  de440 = calceph_open(datafile.c_str());
  if(!de440) {
    std::cerr << "ERROR! could not open ephemeris data\n";
    return 1;
  }

  // Make de440 provide ephemeris data for the major planets.
  novas_use_calceph_planets(de440);

  // We could also use further ephemeris binaries for generic
  // Solar-systems sources also  is not called separately
  //
  // E.g. Jovian satellites:
  // t_calcephbin jovian = calceph_open("/path/to/jup365.bsp");
  // novas_use_calceph(jovian);
  //
  // Or, *both* the main Jovian and Saturnian satellites:
  // t_calcephbin moons = calceph_open_array("/path/to/jup365.bsp", "/path/to/sat441.bsp");
  // novas_use_calceph(moons);


  // -------------------------------------------------------------------------
  // Define a Solar-system source

  // To define a major planet (or Sun, Moon, SSB, or EMB):
  auto source = Planet::mars();

  // ... Or, to define a minor body, such as an asteroid or satellite
  //auto source = EphemerisSource("Io", 501);


  // -------------------------------------------------------------------------
  // If the object uses CALCEPH IDs instead of NAIF, then
  //novas_calceph_use_ids(NOVAS_ID_CALCEPH);


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
  // Initialize the observing frame with the given observer location and
  // time of observation. Without a planet provider, we are stuck with reduced
  // (mas) precisions only...
  auto frame = obs.frame_at(t);

  // Or, if you are using CALCEPH without planetary ephemeris data (e.g.
  // de440s.bsp), you will be stuck with reduced (mas) precisions only...
  //auto frame = obs.reduced_accuracy_frame_at(t);


  // -------------------------------------------------------------------------
  // Calculate the precise apparent position.
  Apparent apparent = source.apparent_in(frame);

  // Let's print the apparent position
  std::cout << source.name() << ":\n";
  std::cout << "  " << apparent.equatorial().to_string() << "\n";

  // -------------------------------------------------------------------------
  // Convert the apparent position on sky to horizontal coordinates
  // We'll use an optical refraction model with local weather parameters...
  // (6 C deg, 985 mbar, 74% humidity)
  Weather weather(Temperature::celsius(6.0), Pressure::mbar(985.0), 74.0);

  Horizontal hor = apparent.to_horizontal().to_refracted(novas_optical_refraction, weather);
  if(!hor) {
    std::cerr << "  ERROR! observer has no Earth-based horizon.";
    return 1;
  }

  // Let's print the calculated azimuth and elevation
  std::cout << "  " << hor.to_string() << "\n";


  // -------------------------------------------------------------------------
  // Clean up before we exit...
  calceph_close(de440);
  //calceph_close(jovian);
  //calceph_close(moons);

  return 0;
}

