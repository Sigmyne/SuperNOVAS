/**
 * @file
 *
 * @date Created  on Sep 29, 2025
 * @author Attila Kovacs
 * @version 0.1.0
 *
 *  !!! Under construction !!!
 */

#ifndef INCLUDE_SUPERMOVAS_H_
#define INCLUDE_SUPERMOVAS_H_

#define SUPERNOVAS_CPP_API_VERSION    0.1.0   ///< C++ API version (different from library version)

#if __cplusplus

#include <string>
#include <time.h>
#include <errno.h>

#define NOVAS_NAMESPACE                       /// Make C API available under the 'novas' namespace
#include <novas.h>


namespace supernovas {


// Forward class declarations.
class Unit;
class Constant;
class Validating;
class Vector;
class   Position;
class   Velocity;
class CatalogSystem;
class Distance;
class Interval;
class Angle;
class   TimeAngle;
class Speed;
class Spherical;
class   Horizontal;
class   Equatorial;
class   Ecliptic;
class   Galactic;
class EOP;
class Time;
class Temperature;
class Pressure;
class Weather;
class Site;
class Observer;
class   GeodeticObserver;
class   GeocentricObserver;
class   SolarSystemObserver;
class CatalogEntry;
class Source;
class   CatalogSource;
class   SolarSystemSource;
class     Planet;
class     EphemerisSource;
class     OrbitalSource;
class Frame;
class Apparent;
class Geometric;

/**
 * Various physical units for converting quantities expressed in conventional units to SI, and
 * vice versa. The SuperNOVAS C++ API uses SI quantities consistently for inputs, so these units
 * can be useful for converting quantities in other units to SI. For example, if you have distance
 * in AU, then you can use Unit::au to convert it to SI (i.e. meters), e.g.:
 *
 * ```c
 *   double d = 2.4103 * Unit::au;
 * ```
 *
 * @sa Constant
 * @ingroup util
 */
class Unit {
private:
  // Private Constructor
  Unit() {}

  //Unit(Unit const&);            // Don't Implement
  void operator=(Unit const&);    // Don't implement

public:
  // Deleting the copy constructor to prevent copies
  Unit(const Unit& obj) = delete;
  //Unit(Unit const&)     = delete;

  static constexpr double au = NOVAS_AU;                  /// [m] 1 Astronomical Unit in meters.
  static constexpr double m = 1.0;                        /// [m] 1 meter (standard unit of distance)
  static constexpr double cm = 0.01;                      /// [m] 1 centimeter in meters
  static constexpr double mm = 1e-3;                      /// [m] 1 millimeter in meters
  static constexpr double um = 1e-6;                      /// [m] 1 micrometer (=micron) in meters
  static constexpr double micron = um;                    /// [m] 1 micron (=micrometer) in meters, by another name.
  static constexpr double nm = 1e-9;                      /// [m] 1 nanometer in meters
  static constexpr double angstrom = 1e-10;               /// [m] 1 Angstrom in meters
  static constexpr double km = NOVAS_KM;                  /// [m] 1 kilometer in meters
  static constexpr double pc = NOVAS_PARSEC;              /// [m] 1 parsec in meters
  static constexpr double kpc = 1000.0 * pc;              /// [m] 1 kiloparsec in meters
  static constexpr double Mpc = 1e6 * pc;                 /// [m] 1 megaparsec in meters
  static constexpr double Gpc = 1e9 * pc;                 /// [m] 1 gigaparsec in meters
  static constexpr double lyr = NOVAS_LIGHT_YEAR;         /// [m] 1 light-year in meters

  static constexpr double ns = 1e-9;                      /// [s] 1 nanoseconds in seconds
  static constexpr double us = 1e-6;                      /// [s] 1 microsecond in seconds
  static constexpr double ms = 1e-3;                      /// [s] 1 millisecond in seconds
  static constexpr double sec = 1.0;                      /// [s] 1 second (standard unit of time)
  static constexpr double min = 60.0;                     /// [s] 1 minute in seconds
  static constexpr double hour = 3600.0;                  /// [s] 1 hour in seconds
  static constexpr double day = NOVAS_DAY;                /// [s] 1 day in seconds
  static constexpr double week = 7 * day;                 /// [s] 1 week in seconds
  /// [s] 1 tropical calendar year in seconds (at J2000)
  static constexpr double yr = NOVAS_TROPICAL_YEAR_DAYS * NOVAS_DAY;
  /// [s] 1 tropycal calendar century in seconds (at J2000)
  static constexpr double cy = 100.0 * yr;
  /// [s] 1 Julian year in seconds
  static constexpr double julianYear = NOVAS_JULIAN_YEAR_DAYS * NOVAS_DAY;
  /// [s] 1 Julian century in seconds
  static constexpr double julianCentury = NOVAS_JULIAN_YEAR_DAYS * NOVAS_DAY;

  static constexpr double rad = 1.0;                      /// [rad] 1 radian (standard unit of angle)
  static constexpr double hourAngle = NOVAS_HOURANGLE;    /// [rad] 1 hour of angle in radians
  static constexpr double deg = NOVAS_DEGREE;             /// [rad] 1 degree in radians
  static constexpr double arcmin = deg / 60.0;            /// [rad] 1 minute of arc in radians
  static constexpr double arcsec = NOVAS_ARCSEC;          /// [rad] 1 second or arc in radians
  static constexpr double mas = 1e-3 * arcsec;            /// [rad] 1 millisecond of arc in radians
  static constexpr double uas = 1e-6 * arcsec;            /// [rad] 1 microsecond of arc in radians

  static constexpr double Pa = 1.0;                       /// [Pa] 1 pascal (standard unit of perssure)
  static constexpr double hPa = 100.0;                    /// [Pa] 1 hectopascal in pascals
  static constexpr double mbar = hPa;                     /// [Pa] 1 millibar in pascals
  static constexpr double bar = 1000.0 * mbar;            /// [Pa] 1 bar in pascals
  static constexpr double kPa = 1000.0;                   /// [Pa] 1 kilopascal in pascals
  static constexpr double MPa = 1e6;                      /// [Pa] 1 megapascal in pascals
  static constexpr double torr = 133.3223684211;          /// [Pa] 1 torr (mm of Hg) in pascals
  static constexpr double atm = 101325.0;                 /// [Pa] 1 atmosphere in pascals

  static constexpr double R_earth = NOVAS_GRS80_RADIUS;   /// [m] 1 Earth quatorial radius (GRS80) in meters
};

/**
 * Various physical constants that SuperNOVAS uses for astromtric calculations, all expressed in
 * terms of SI units. You can use them also. For example, you might use Constant::c to turn a
 * velocity (im m/s) to a unitless &beta;:
 *
 * ```c
 *   double beta = 29500.0 / Constant::c;
 * ```
 *
 * or combined with units, say if the velocity is in AU/day:
 *
 * ```c
 *   double beta = 0.00614 * (Unit::km / Unit::sec) / Constant::c;
 * ```
 *
 * @sa Unit
 * @ingroup util
 */
class Constant {
private:
  // Private Constructor
  Constant() {}

  //Constant(Constant const&);        // Don't Implement
  void operator=(Constant const&);    // Don't implement

public:
  // Deleting the copy constructor to prevent copies
  Constant(const Constant& obj) = delete;
  //Constant(Constant const&)     = delete;

  static constexpr double pi = M_PI;                    /// [rad] &pi;
  static constexpr double twoPi = TWOPI;                /// [rad] 2&pi;
  static constexpr double halfPi = 0.5 * pi;            /// [rad] &pi;/2

  static constexpr double c = NOVAS_C;                  ///< [m/s] speed of light
  static constexpr double G = 6.67428e-1;               ///< [m<sup>3</sup> kg<sup>-1</sup> s<sup>-2</sup>]

  static constexpr double L_B = 1.550519768e-8;         ///< Barycentric clock rate increment over TT
  static constexpr double L_G = 6.969290134e-10;        ///< Geocentric clock rate increment over TT

  /// GRS80 Earth flattening
  static constexpr double F_earth = NOVAS_GRS80_FLATTENING;
  static constexpr double GM_sun = NOVAS_G_SUN;	        ///< [m<sup>3</sup> s<sup>-2</sup>] Solar graviational constant
  static constexpr double GM_earth = NOVAS_G_EARTH;     ///< [m<sup>3</sup> s<sup>-2</sup>] Earth graviational constant
  static constexpr double M_sun = GM_sun / G;           ///< [kg] Mass of the Sun
  static constexpr double M_earth = GM_sun / G;         ///< [kg] Earth mass
};


/**
 * A simple interface class handling validation checking for classes that inherit it.
 *
 */
class Validating {
protected:
  /**
   * the state variable. Constructors should set it to `true` before returning if the instance has
   * been initialized in a valid state.
   *
   * @sa is)valid()
   */
  bool _valid = false;

  /// dummy constructor;
  Validating() {}

public:

  /**
   * Returns the previously set 'valid' stae of the implementing instance. Generally 'valid' means
   * that the class has all fields defined with sane values, such as floating-point values that
   * are not NAN, and object fields that are themselves 'valid'. Some implementing classes may
   * also apply additional range checking for values, for example, ensuring that a velocity does
   * not exceed the speed of light, etc.
   *
   * @return    `true` if the instance is in a 'valid' state, or else `false`.
   */
  bool is_valid() const { return _valid; }
};

/**
 * Celestial coordinate reference system. This class does not include Earth-rotating systems, such
 * as TIRS, PER, or ITRS.
 *
 * @sa CatalogEntry, Equatorial, Ecliptic, Apparent, Geometric
 * @ingroup source
 */
class CatalogSystem : public Validating {
private:
  CatalogSystem(const std::string& name, double jd_tt);

protected:
  std::string _name;
  double _jd;

public:
  CatalogSystem(const std::string& name);

  CatalogSystem(double jd_tt);

  double jd() const;

  double epoch() const;

  const std::string& name() const;

  std::string str() const;

  static CatalogSystem at_julian_date(double year);

  static CatalogSystem at_besselian_epoch(double year);

  static const CatalogSystem& icrs();

  static const CatalogSystem& j2000();

  static const CatalogSystem& hip();

  static const CatalogSystem& b1950();

  static const CatalogSystem& b1900();
};

/// \ingroup util
class Distance : public Validating {
private:
  double _meters;

public:
  Distance(double meters);

  double m() const;

  double km() const;

  double au() const;

  double lyr() const;

  double pc() const;

  double kpc() const;

  double Mpc() const;

  double Gpc() const;

  Angle parallax() const;

  std::string str() const;

  static Distance from_parallax(double parallax);

  static const Distance& at_Gpc();
};

/// \ingroup time
class Interval : public Validating {
private:

  double _seconds;
  enum novas::novas_timescale _scale;

public:

  Interval(double seconds, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Interval operator+(const Interval& r) const;

  Interval operator-(const Interval& r) const;

  TimeAngle operator+(const TimeAngle& base) const;

  Time operator+(const Time& base) const;

  Distance operator*(const Speed& v) const;

  Position operator*(const Velocity& v) const;

  bool is_equal(const Interval& interval, double precision = Unit::us) const;

  enum novas::novas_timescale timescale() const;

  Interval inv() const;

  double milliseconds() const;

  double seconds() const;

  double minutes() const;

  double hours() const;

  double days() const;

  double years() const;

  double julian_years() const;

  double julian_centuries() const;
};

/// \ingroup util
class Angle : public Validating {
protected:
  double _rad;

public:

  virtual ~Angle() {};

  Angle(double x);

  Angle(const std::string& str);

  Angle operator+(const Angle& r);

  Angle operator-(const Angle& r);

  bool is_equal(const Angle& angle, double precision = Unit::uas) const;

  double rad() const;

  double deg() const;

  double arcmin() const;

  double arcsec() const;

  double mas() const;

  double uas() const;

  double fraction() const;

  virtual std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

/// \ingroup time util
class TimeAngle : public Angle {
public:

  TimeAngle(double x);

  TimeAngle(const std::string& str);

  TimeAngle(const Angle& angle);

  TimeAngle operator+(const Interval& other);

  TimeAngle operator-(const Interval& other);

  double hours() const;

  double minutes() const;

  double seconds() const;

  std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;
};

/// \ingroup util
class Vector : public Validating {
protected:
  double _component[3];

  Vector(double x = 0.0, double y = 0.0, double z = 0.0);

  Vector(const double v[3]);

public:

  virtual ~Vector() {}; // something virtual to make class polymorphic for dynamic casting.

  Vector operator*(double factor) const;

  bool is_equal(const Vector& v, double precision) const;

  Vector scaled(double factor) const;

  const double *_array() const;

  double abs() const;

  double dot(const Vector& v) const;

  double projection_on(const Vector& v) const;

  Vector unit_vector() const;

  virtual std::string str() const;
};

/// \ingroup geometric
class Position : public Vector {
public:

  Position(double x_m = 0.0, double y_m = 0.0, double z_m = 0.0);

  Position(const double pos[3], double unit = 1.0);

  Position operator+(const Position &r) const;

  Position operator-(const Position &r) const;

  double x_m() const;

  double y_m() const;

  double z_m() const;

  Distance distance() const;

  Position inv() const;

  Spherical as_spherical() const;

  std::string str() const override;

  static const Position& origin();

  static const Position& invalid();
};

/// \ingroup geometric
class Velocity : public Vector {
public:
  Velocity(double x_ms = 0.0, double y_ms = 0.0, double z_ms = 0.0);

  Velocity(const double vel[3], double unit = 1.0);

  Velocity operator+(const Velocity& r) const;

  Velocity operator-(const Velocity& r) const;

  bool is_equal(const Interval& interval, double precision = Unit::us) const;

  Speed speed() const;

  double x_m_per_s() const;

  double y_m_per_s() const;

  double z_m_per_s() const;

  Velocity inv() const;

  Speed along(const Vector& direction) const;

  Position travel(double seconds) const;

  Position travel(const Interval& t) const;

  Position operator*(const Interval& t) const { return travel(t); }

  std::string str() const override;

  static const Velocity& stationary();

  static const Velocity& invalid();
};

/// \ingroup util spectral
class Speed : public Validating {
protected:
  double _ms;

public:
  Speed(double ms);

  Speed(const Distance d, const Interval& time);

  Speed operator+(const Speed& r) const;

  Speed operator-(const Speed& r) const;

  double m_per_s() const;

  double km_per_s() const;

  double au_per_day() const;

  double beta() const;

  double Gamma() const;

  double redshift() const;

  Distance travel(double seconds) const;

  Distance travel(const Interval& time) const;

  Distance operator*(const Interval& time) const { return travel(time); }

  Velocity in_direction(const Vector& direction) const;

  std::string str() const;

  static Speed from_redshift(double z);

  static const Speed& stationary();
};

/// \ingroup util
class Spherical : public Validating {
protected:
  Angle _lon, _lat;
  Distance _distance;

  Spherical();

public:
  virtual ~Spherical() {}; // something virtual to make class polymorphic for dynamic casting.

  Spherical(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  Spherical(const Position& pos);

  Position xyz() const;

  const Angle& longitude() const;

  const Angle& latitude() const;

  const Distance& distance() const;

  virtual const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

/// \ingroup equatorial
class Equatorial : public Spherical {
private:
  CatalogSystem _sys;

  void validate();

public:
  Equatorial(double ra, double dec, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Equatorial(const Angle& ra, const Angle& dec, const CatalogSystem& system = CatalogSystem::icrs(), const Distance& distance = Distance::at_Gpc());

  Equatorial(const Position& pos, const CatalogSystem& system = CatalogSystem::icrs());

  TimeAngle ra() const;

  const Angle& dec() const;

  const CatalogSystem& system() const;

  Ecliptic as_ecliptic() const;

  Galactic as_galactic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Equatorial& invalid();
};

/// \ingroup nonequatorial
class Ecliptic : public Spherical {
private:
  CatalogSystem _sys;

  void validate();

public:
  Ecliptic(double longitude, double latitude, const std::string& system = "ICRS", double distance = NOVAS_DEFAULT_DISTANCE);

  Ecliptic(const Angle& ra, const Angle& dec, const CatalogSystem &system = CatalogSystem::icrs(), const Distance& distance = Distance::at_Gpc());

  Ecliptic(const Position& pos, const CatalogSystem& system = CatalogSystem::icrs());

  const CatalogSystem& system() const;

  Equatorial as_equatorial() const;

  Galactic as_galactic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Ecliptic& invalid();
};

/// \ingroup nonequatorial
class Galactic : public Spherical {
public:
  Galactic(double longitude, double latitude, double distance = NOVAS_DEFAULT_DISTANCE);

  Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  Galactic(const Position& pos, const Distance& distance = Distance::at_Gpc());

  Equatorial as_equatorial() const;

  Ecliptic as_ecliptic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Galactic& invalid();
};

/// \ingroup util
class Temperature : public Validating {
private:
  double _deg_C;

  Temperature(double deg_C);

public:
  double celsius() const;

  double kelvin() const;

  double farenheit() const;

  std::string str() const;

  static Temperature celsius(double value);

  static Temperature kelvin(double value);

  static Temperature farenheit(double value);
};

/// \ingroup util
class Pressure : public Validating {
private:
  double _pascal;

  Pressure(double value);

public:
  double Pa() const;

  double hPa() const;

  double kPa() const;

  double mbar() const;

  double bar() const;

  double torr() const;

  double atm() const;

  std::string str() const;

  static Pressure Pa(double value);

  static Pressure hPa(double value);

  static Pressure kPa(double value);

  static Pressure mbar(double value);

  static Pressure bar(double value);

  static Pressure torr(double value);

  static Pressure atm(double value);
};

/// \ingroup refract
class Weather : public Validating {
private:
  Temperature _temperature;
  Pressure _pressure;
  double _humidity;

  void validate();

public:
  Weather(const Temperature& T, const Pressure& p, double humidity_percent);

  Weather(double celsius, double pascal, double humidity_percent);

  const Temperature& temperature() const;

  const Pressure& pressure() const;

  double humidity() const;

  double humidity_fraction() const;

  std::string str() const;

  static Weather guess(const Site& site);
};

/// \ingroup earth nonequatorial
class EOP : public Validating {
private:
  int _leap;
  Angle _xp, _yp;
  double _dut1;
  double _dxp = 0.0, _dyp = 0.0, _dt = 0.0;  // [arcsec, s] Applied corrections, in novas units.

  void validate();

public:
  EOP(int leap_seconds, double dut1 = 0.0, double xp = 0.0, double yp = 0.0);

  EOP(int leap_seconds, double dut1, const Angle& xp, const Angle& yp);

  int leap_seconds() const;

  const Angle& xp() const;

  const Angle& yp() const;

  double dUT1() const;

  EOP itrf_transformed(int from_year, int to_year) const;

  EOP diurnal_corrected(const Time& time) const;

  std::string str() const;

  static const EOP& invalid();
};

/// \ingroup observer
class Site : public Validating {
private:
  novas::on_surface _site;

  Site();

public:

  Site(double longitude, double latitude, double altitude=0.0, enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID);

  Site(const Position& xyz);

  const novas::on_surface *_on_surface() const;

  const Angle longitude() const;

  const Angle latitude() const;

  const Distance altitude() const;

  const Position xyz(enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID) const;

  Site itrf_transformed(int from_year, int to_year) const;

  Position xyz() const;

  std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;

  static Site from_GPS(double longitude, double latitude, double altitude = 0.0);

  static Site from_xyz(const Position& xyz);

  static Site from_xyz(double x, double y, double z);
};

/// \ingroup observer
class Observer : public Validating {
protected:
  novas::observer _observer = {};

  Observer() {};

public:

  virtual ~Observer() {}; // something virtual to make class polymorphic for dynamic casting.

  const novas::observer * _novas_observer() const;

  enum novas::novas_observer_place type() const;

  virtual bool is_geodetic() const { return false; }

  virtual bool is_geocentric() const { return false; }

  static GeodeticObserver on_earth(const Site& site, const EOP& eop);

  static GeodeticObserver on_earth(const Site& geodetic, const Velocity& vel, const EOP& eop);

  static GeocentricObserver in_earth_orbit(const Position& pos, const Velocity& vel);

  static GeocentricObserver at_geocenter();

  static SolarSystemObserver in_solar_system(const Position& pos, const Velocity& vel);

  static SolarSystemObserver at_ssb();

  static const Observer& invalid();
};

/// \ingroup observer
class GeocentricObserver : public Observer {
public:
  GeocentricObserver();

  GeocentricObserver(const Position& pos, const Velocity& vel);

  bool is_geocentric() const override { return true; }

  Position geocentric_position() const;

  Velocity geocentric_velocity() const;
};

/// \ingroup observer
class SolarSystemObserver : public Observer {
public:

  SolarSystemObserver();

  SolarSystemObserver(const Position& pos, const Velocity& vel);

  Position ssb_position() const;

  Velocity ssb_velocity() const;
};

/// \ingroup observer
class GeodeticObserver : public Observer {
private:
  EOP _eop;

public:
  GeodeticObserver(const Site& site, const EOP& eop);

  GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop);

  bool is_geodetic() const override { return true; }

  Site site() const;

  const EOP& eop() const;
};


/// \ingroup time
class Time : public Validating {
private:
  novas::novas_timespec _ts;

  Time() {};

  bool is_valid_parms(double dUT1, enum novas::novas_timescale timescale) const;

public:

  Time(double jd, int leap_seconds, double dUT1, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(double jd, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(long ijd, double fjd, int leap_seconds, double dUT1, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(long ijd, double fjd, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_TT);

  Time(const std::string& timestamp, int leap_seconds, double dUT1, enum novas::novas_timescale timescale = novas::NOVAS_UTC);

  Time(const std::string& timestamp, const EOP& eop, enum novas::novas_timescale timescale = novas::NOVAS_UTC);

  Time(const struct timespec t, int leap_seconds, double dUT1);

  Time(const struct timespec t, const EOP& eop);

  Time(const novas::novas_timespec *t);

  Interval operator-(const Time &other) const;

  Interval operator-(const novas::novas_timespec *other) const;

  Time operator+(const Interval &delta) const;

  Time operator-(const Interval &delta) const;

  bool operator<(const Time &other) const;

  bool operator<(const novas::novas_timespec *other) const;

  bool operator>(const Time &other) const;

  bool operator>(const novas::novas_timespec *other) const;

  bool operator<=(const Time &other) const;

  bool operator<=(const novas::novas_timespec *other) const;

  bool operator>=(const Time &other) const;

  bool operator>=(const novas::novas_timespec *other) const;

  bool equals(const Time& time, double precision = Unit::sec / Unit::day) const;

  bool equals(const Time& time, const Interval& interval) const;

  const novas::novas_timespec * _novas_timespec() const;

  double jd(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  int leap_seconds() const;

  Interval dUT1() const;

  TimeAngle time_of_day(enum novas::novas_timescale timescale = novas::NOVAS_TT) const;

  int day_of_week(enum novas::novas_timescale timescale) const;

  double epoch() const;

  TimeAngle gst(enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  TimeAngle gmst() const;

  TimeAngle lst(const Site& site, enum novas::novas_accuracy accuracy) const;

  TimeAngle era() const;

  std::string str(enum novas::novas_timescale timescale = novas::NOVAS_UTC) const;

  std::string iso_str() const;

  std::string epoch_str() const;

  Time shifted(double seconds) const;

  Time shifted(Interval offset) const;

  static Time now(const EOP& eop);

  static const Time& j2000();

  static const Time& hip();

  static const Time& b1950();

  static const Time& b1900();

  static const Time& invalid();
};

/// \ingroup frame
class Frame : public Validating {
private:
  novas::novas_frame _frame;
  Observer _observer;
  Time _time;

public:
  Frame(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY);

  const novas::novas_frame *_novas_frame() const;

  const Observer& observer() const;

  const Time& time() const;

  enum novas::novas_accuracy accuracy() const;

  Apparent approx_apparent(const Planet& planet, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  bool has_planet_data(enum novas::novas_planet planet) const;

  bool has_planet_data(const Planet& planet) const;

  Position ephemeris_position(enum novas::novas_planet planet) const;

  Position ephemeris_position(const Planet& planet) const;

  Velocity ephemeris_velocity(enum novas::novas_planet planet) const;

  Velocity ephemeris_velocity(const Planet& planet) const;

  double clock_skew(enum novas::novas_timescale = novas::NOVAS_TT) const;

  static const Frame& invalid();
};


/// \ingroup source
class Source : public Validating {
protected:
  struct novas::novas_object _object;

  Source() {}

public:
  virtual ~Source() {}; // something virtual to make class polymorphic for dynamic casting.

  const struct novas::novas_object *_novas_object() const;

  std::string name() const;

  Apparent apparent(const Frame &frame, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  Geometric geometric(const Frame &frame, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  Angle sun_angle(const Frame &frame) const;

  Angle moon_angle(const Frame &frame) const;

  Angle angle_to(const Source& source, const Frame& frame) const;

  Time rises_above(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  Time transits(const Frame &frame) const;

  Time sets_below(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  static void set_case_sensitive(bool value);
};

/// \ingroup source spectral
class CatalogEntry : public Validating {
private:
  double _epoch;
  novas::cat_entry _entry;
  CatalogSystem _sys;

  void set_epoch();

public:
  CatalogEntry(const std::string &name, double RA, double Dec, const std::string& system = "ICRS");

  CatalogEntry(const std::string &name, const Angle& RA, const Angle& Dec, const CatalogSystem& system = CatalogSystem::icrs());

  CatalogEntry(novas::cat_entry e, const std::string& system = "ICRS");

  const novas::cat_entry* _cat_entry() const;

  const CatalogSystem& system() const;

  std::string name() const;

  long number() const;

  Angle ra() const;

  Angle dec() const;

  Speed v_lsr() const;

  Speed radial_velocity() const;

  Distance distance() const;

  Angle parallax() const;

  Equatorial equatorial() const;

  CatalogEntry& proper_motion(double ra, double dec);

  CatalogEntry& parallax(double radians);

  CatalogEntry& parallax(const Angle& angle);

  CatalogEntry& distance(double meters);

  CatalogEntry& distance(const Distance& dist);

  CatalogEntry& v_lsr(double v_ms);

  CatalogEntry& v_lsr(const Speed& v);

  CatalogEntry& radial_velocity(double v_ms);

  CatalogEntry& radial_velocity(const Speed& v);

  CatalogEntry& redshift(double z);

  CatalogEntry& catalog(const std::string& name, long number);
};

/// \ingroup source
class CatalogSource : public Source {
private:
  CatalogSystem _system;

public:
  CatalogSource(const CatalogEntry& e);

  const novas::cat_entry * _cat_entry() const;

  CatalogEntry catalog_entry() const;
};

/// \ingroup source
class SolarSystemSource : public Source {
protected:
  SolarSystemSource() {}

public:
  double solar_illumination(const Frame& frame) const;

  double helio_distance(const Time& time, double *rate = NULL) const;

  double solar_power(const Time& time) const;
};

/// \ingroup source
class Planet : public SolarSystemSource {
public:
  Planet(enum novas::novas_planet number);

  Planet(const std::string& name);

  enum novas::novas_planet novas_id() const;

  int naif_id() const;

  int de_number() const;

  double mean_radius() const;

  double mass() const;

  static const Planet& ssb();

  static const Planet& mercury();

  static const Planet& venus();

  static const Planet& earth();

  static const Planet& mars();

  static const Planet& jupiter();

  static const Planet& saturn();

  static const Planet& uranus();

  static const Planet& neptune();

  static const Planet& pluto();

  static const Planet& sun();

  static const Planet& moon();

  static const Planet& emb();

  static const Planet& pluto_system();

};

/// \ingroup source
class EphemerisSource : public SolarSystemSource {
public:
  EphemerisSource(const std::string &name, long number);

  long number() const;
};

/// \ingroup source
class OrbitalSource : public SolarSystemSource {
public:
  OrbitalSource(const std::string& name, long number, const novas::novas_orbital *orbit);

  const novas::novas_orbital *_novas_orbital() const;

  Position orbital_position(const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  Velocity orbital_velocity(const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;
};

/// \ingroup apparent spectral
class Apparent : public Validating {
private:
  Frame _frame;
  enum novas::novas_reference_system _sys;

  novas::sky_pos _pos;

  Apparent(const Frame& frame, enum novas::novas_reference_system system);

public:
  Apparent(const Frame& frame, const Equatorial& eq, double rv = 0.0, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  Apparent(const Frame& frame, const Equatorial& eq, const Speed& rv, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  Apparent(const Frame& frame, novas::sky_pos p, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  const novas::sky_pos *_sky_pos() const;

  const Frame& frame() const;

  enum novas::novas_reference_system system() const;

  Angle ra() const;

  Angle dec() const;

  Speed radial_velocity() const;

  Distance distance() const;

  Equatorial equatorial() const;

  Ecliptic ecliptic() const;

  Galactic galactic() const;

  Horizontal horizontal() const;

  Apparent in_system(enum novas::novas_reference_system system) const;

  Apparent in_icrs() const { return in_system(novas::NOVAS_ICRS); }

  Apparent in_j2000() const { return in_system(novas::NOVAS_J2000); }

  Apparent in_mod() const { return in_system(novas::NOVAS_MOD); }

  Apparent in_tod() const { return in_system(novas::NOVAS_TOD); }

  Apparent in_cirs() const { return in_system(novas::NOVAS_CIRS); }

  Apparent in_tirs() const { return in_system(novas::NOVAS_TIRS); }

  Apparent in_itrs() const { return in_system(novas::NOVAS_ITRS); }

  static const Apparent& invalid();
};

/// \ingroup geometric
class Geometric : public Validating {
private:
  Frame _frame;
  enum novas::novas_reference_system _sys;

  Position _pos;
  Velocity _vel;

public:
  Geometric(const Frame& frame, enum novas::novas_reference_system system, const Position& p, const Velocity& v);

  const Frame& frame() const;

  enum novas::novas_reference_system system() const;

  const Position& position() const;

  const Velocity& velocity() const;

  Equatorial equatorial() const;

  Ecliptic ecliptic() const;

  Galactic galactic() const;

  Geometric in_system(enum novas::novas_reference_system system) const;

  Geometric in_icrs() const { return in_system(novas::NOVAS_ICRS); }

  Geometric in_j2000() const { return in_system(novas::NOVAS_J2000); }

  Geometric in_mod() const { return in_system(novas::NOVAS_MOD); }

  Geometric in_tod() const { return in_system(novas::NOVAS_TOD); }

  Geometric in_cirs() const { return in_system(novas::NOVAS_CIRS); }

  Geometric in_tirs() const { return in_system(novas::NOVAS_TIRS); }

  Geometric in_itrs() const { return in_system(novas::NOVAS_ITRS); }

  static const Geometric& invalid();
};

/// \ingroup nonequatorial refract
class Horizontal : public Spherical {
private:
  Horizontal();

  int location_with_weather(const Frame& frame, const Weather& weather, novas::on_surface *s);

public:
  Horizontal(double azimuth, double elevation, double distance = NOVAS_DEFAULT_DISTANCE);

  Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance = Distance::at_Gpc());

  const Angle& azimuth() const;

  const Angle& elevation() const;

  Horizontal to_refracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  Horizontal to_unrefracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  Apparent to_apparent(const Frame& frame, double rv = 0.0, double distance = NOVAS_DEFAULT_DISTANCE) const;

  Apparent to_apparent(const Frame& frame, const Speed& rv = Speed::stationary(), const Distance& distance = Distance::at_Gpc()) const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Horizontal& invalid();
};



} // namespace supernovas

#  endif /* cplusplus */
#endif /* INCLUDE_SUPERMOVAS_H_ */
