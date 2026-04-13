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

int main() {

  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);

  // -------------------------------------------------------------------------
  // Earth orientation parameters (EOP), as appropriate for the time of observation,
  // e.g. as obtained from IERS bulletins or data service:
  EOP eop(LEAP_SECONDS, DUT1, POLAR_DX * Unit::mas, POLAR_DY * Unit::mas);

  // -------------------------------------------------------------------------
  // Define an orbital source

  // Ceres' orbit is a heliocentric orbit in the ecliptic.
  OrbitalSystem system = OrbitalSystem::ecliptic();

  // Orbital Parameters for the asteroid Ceres from the Minor Planet Center
  // (MPC) at JD 2460600.5
  // Start with the essential parameters...
  Orbital orbit = Orbital::from_mean_motion(system,
          2460600.5,                            // TDB-based Julian date of parameters
          2.7666197 * Unit::AU,                 // Semi-major axis (a)
          145.84905 * Unit::deg,                // mean anomaly (M0)
          0.21418047 * Unit::deg / Unit::day);  // mean motion (n)

  // Add eccentricity (e) and argument of periapsis (omega)
  orbit.eccentricity(0.079184, 73.28579 * Unit::deg);

  // Add inclination (i) and argument of rising node (Omega)
  orbit.inclination(10.5879 * Unit::deg, 80.25414 * Unit::deg);

  // Alternatively, instead of (i, Omega), we could define the orientation of the orbital pole
  // relative to the orbital system
  //orbit.pole(longitude, latitude);

  // Define Ceres as the observed object
  auto ceres = orbit.to_source("Ceres");


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
  // Calculate the precise apparent position.
  Apparent apparent = ceres.apparent_in(frame);

  // Let's print the apparent position
  std::cout << ceres.name() << ":\n";
  std::cout << "  " << apparent.equatorial().to_string() << ", " << apparent.radial_velocity().to_string() << "\n";


  // -------------------------------------------------------------------------
  // Define local weather (for refraction correction)
  // We'll use an optical refraction model with local weather parameters...
  // (6 C deg, 985 mbar, 74% humidity)
  Weather weather(Temperature::celsius(6.0), Pressure::mbar(985.0), 74.0 * Unit::percent);


  // -------------------------------------------------------------------------
  // Calculate refracted horizontal coordinates
  Horizontal hor = apparent.to_horizontal().to_refracted(novas_optical_refraction, weather);

  std::cout << "  " << hor.to_string() << "\n";


  // -------------------------------------------------------------------------
  // ... Or, you could define orbitals for a satellite instead:
  // E.g. Callisto's orbital parameters from JPL Horizons
  // https://ssd.jpl.nasa.gov/sats/elem/sep.html
  // 1882700. 0.007 43.8  87.4  0.3 309.1 16.690440 277.921 577.264 268.7 64.8

  // Callisto's orbit is around Jupiter, and defined w.r.t. to Jpiter's
  // orbital system, whose pole is defined in ICRS equatorial coordinates
  //
  // However, to get positions for Callisto from it's orbit you will need an
  // ephemeris provider for Jupiter's position at the orbital center (see
  // commented CALCEPH section further above).
  /*
  system = OrbitalSystem::ecliptic(Planet::jupiter())
          .pole(268.7 * Unit::deg, 64.8 * Unit::deg, Equinox::icrs());

  // Start with the essential parameters...
  orbit = Orbital(system,
          NOVAS_JD_J2000,           // TDB-based Julian date of parameters
          1882700.0 * Unit::km,     // Semi-major axis (a)
          87.4 * Unit::deg,         // mean anomaly (M0)
          16.690440 * Unit::day);   // orbital period (T)

  // Add eccentricity (e) and argument of periapsis (omega), and apsis rotation
  orbit.eccentricity(0.007, 43.8 * Unit::deg);
  orbit.apsis_period(277.921 * Unit::yr);

  // Add inclination (i) and argument of rising node (Omega), and node rotation
  orbit.inclination(0.3 * Unit::deg, 309.1 * Unit::deg);
  orbit.node_period(577.264 * Unit::yr);

  // Set Callisto as the observed object
  auto callisto = orbit.to_source("Callisto");

  // No calculate apparent and horizontal positions for Callisto
  apparent = callisto.apparent_in(frame);
  hor = apparent.to_horizontal().to_refracted(novas_optical_refraction, weather);

  std::cout << callisto.name() << ":\n";
  std::cout << "  " << apparent.equatorial().to_string() << ", " << apparent.radial_velocity().to_string() << "\n";
  std::cout << "  " << hor.to_string() << "\n";
  */


  // -------------------------------------------------------------------------
  // Clean up before we exit...
  //calceph_close(planets);

  return 0;
}

