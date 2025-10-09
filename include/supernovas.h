/**
 * @file
 *
 * @date Created  on Sep 29, 2025
 * @author Attila Kovacs
 * @since 1.6
 * @version 0.2.0
 *
 *  !!! Under construction !!!
 */

#ifndef INCLUDE_SUPERMOVAS_H_
#define INCLUDE_SUPERMOVAS_H_

#define SUPERNOVAS_CPP_API_VERSION    0.2.0   ///< C++ API version (different from library version)

#if __cplusplus

#include <string>
#include <optional>
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
class EquatorialSystem;
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
  /// \cond PRIVATE
  // Deleting the copy constructor to prevent copies
  Unit(const Unit& obj) = delete;
  //Unit(Unit const&)     = delete;
  /// \endcond

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
  static constexpr double s = 1.0;                        /// [s] 1 second (standard unit of time)
  static constexpr double sec = s;                        /// [s] 1 second by another name
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
 * Various physical constants that SuperNOVAS uses for astrometric calculations, all expressed in
 * terms of SI units. You can use them also. For example, you might use Constant::c to turn a
 * velocity (im m/s) to a unitless &beta;:
 *
 * ```c
 *   double beta = (29.5 * Unit::km / Unit::sec) / Constant::c;
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
  /// \cond PRIVATE
  // Deleting the copy constructor to prevent copies
  Constant(const Constant& obj) = delete;
  //Constant(Constant const&)     = delete;
  /// \endcond

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
 * Equatorial (RA/Dec) coordinate system. This class does not include the Earth-rotating systems
 * TIRS and ITRS.
 *
 * @sa CatalogEntry, Equatorial, Ecliptic, Apparent, Geometric
 * @ingroup source
 */
class EquatorialSystem : public Validating {
private:
  EquatorialSystem(const std::string& name, double jd_tt);

protected:
  std::string _name;    ///< name of the catalog system, e.g. 'ICRS' or 'J2000'
  enum novas::novas_reference_system _system; ///< Coordinate reference system.
  double _jd;           ///< [day] Julian date of the dynamical equator (or closest to it) that
                        ///< matches the system

public:
  EquatorialSystem(enum novas::novas_reference_system system, double jd_tt = NOVAS_JD_J2000);

  EquatorialSystem(enum novas::novas_reference_system system, const Time& time);

  bool operator==(const EquatorialSystem& system) const;

  double jd() const;

  double epoch() const;

  const std::string& name() const;

  enum novas::novas_reference_system reference_system() const;

  bool is_icrs() const;

  bool is_mod() const;

  bool is_true() const;

  std::string str() const;

  static std::optional<EquatorialSystem> from_string(const std::string& name);

  static EquatorialSystem at_julian_date(double jd_tt);

  static EquatorialSystem at_besselian_epoch(double year);

  static const EquatorialSystem& icrs();

  static const EquatorialSystem& j2000();

  static const EquatorialSystem& hip();

  static const EquatorialSystem& b1950();

  static const EquatorialSystem& b1900();
};


/**
 * A scalar separation between two points in space. It may be signed, such that that the distance
 * from __A__ to __B__ is the negative of the distance __B__ to __A__, i.e.:
 *
 *  \| __A__ - __B__ \| = -\| __B__ - __A__ \|
 *
 * for two vector locations __A__ and __B__.
 *
 * @sa Position
 * @ingroup util
 */
class Distance : public Validating {
private:
  double _meters;         ///< [m] stored distance

public:
  Distance(double meters);

  /**
   * Returns the magnitude of this distance, as a unsigned distance.
   *
   * @return    The absolute value of the (possibly signed) distance value represented by this instance.
   */
  Distance abs() const;

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

  static Distance from_parallax(const Angle& parallax);

  static const Distance& at_Gpc();
};

/**
 * A signed time interval between two instants of time, in the astronomical timescale of choice.
 *
 * @sa Time, TimeAngle
 * @ingroup time
 */
class Interval : public Validating {
private:

  double _seconds;                      ///< [s] stored time of the interval
  enum novas::novas_timescale _scale;   ///< store timescale of the interval

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

/**
 * A representation of a regularized angle, which can be expressed in various commonly used
 * angular units as needed. It can also be used to instantiate angles from decimal or
 * [+]DDD:MM:SS.SSS string representations of the angle in degrees.
 *
 * @sa TimeAngle
 * @ingroup util
 */
class Angle : public Validating {
protected:
  double _rad;      ///< [rad] stored angle value, usually [-&pi;:&pi;), but can be different for subclasses.

public:

  virtual ~Angle() {};

  Angle(double radians);

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

  static constexpr int east = 1;      ///< East direction sign, e.g `19.5 * Unit::deg * Angle::east` for 19.5 deg East.

  static constexpr int west = -1;     ///< West direction sign, e.g `155.1 * Unit::deg * Angle::west` for 155.1 deg West.

  static constexpr int north = 1;     ///< North direction sign, e.g `33.4 * Unit::deg * Angle::north` for 33.4 deg North.

  static constexpr int south = -1;    ///< South direction sign, e.g `90.0 * Unit::deg * Angle::south` for the South pole.

};

/**
 * A representation of a regularized angle, which can also be represented as a time value in the 0
 * to 24h range. It can be expressed both in terms various commonly used angular units, or in terms
 * of time units, as needed.  It may also be used to instantiate time-angles from decimal or
 * from HH:MM:SS.SSS string representations of time in hours.
 *
 * @sa Time, Interval
 * @ingroup time util
 */
class TimeAngle : public Angle {
public:

  TimeAngle(double radians);

  TimeAngle(const std::string& str);

  TimeAngle(const Angle& angle);

  TimeAngle operator+(const Interval& other);

  TimeAngle operator-(const Interval& other);

  double hours() const;

  double minutes() const;

  double seconds() const;

  std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;
};

/**
 * A generic 3D spatial vector, expressed in arbitrary units.
 *
 * @sa Position, Velocity
 * @ingroup util
 */
class Vector : public Validating {
protected:
  double _component[3];       ///< [arb.u] Array containing the x, y, z components.

  Vector(double x = 0.0, double y = 0.0, double z = 0.0);

  Vector(const double v[3], double unit = 1.0);

public:

  virtual ~Vector() {}; // something virtual to make class polymorphic for dynamic casting.

  Vector operator*(double factor) const;

  double x() const;

  double y() const;

  double z() const;

  bool is_equal(const Vector& v, double precision) const;

  Vector scaled(double factor) const;

  const double *_array() const;

  double abs() const;

  double dot(const Vector& v) const;

  double projection_on(const Vector& v) const;

  Vector unit_vector() const;

  virtual std::string str() const;
};

/**
 * A 3D physical location vector in space.
 *
 * @sa Velocity, Geometric
 * @ingroup geometric
 */
class Position : public Vector {
public:

  Position(double x_m = 0.0, double y_m = 0.0, double z_m = 0.0);

  Position(const double pos[3], double unit = Unit::m);

  Position operator+(const Position &r) const;

  Position operator-(const Position &r) const;

  Distance distance() const;

  Position inv() const;

  Spherical as_spherical() const;

  std::string str() const override;

  static const Position& origin();

  static const Position& invalid();
};

/**
 * A 3D physical velocity vector in space.
 *
 * @sa Velocity, Geometric
 * @ingroup geometric
 */
class Velocity : public Vector {
public:
  Velocity(double x_ms = 0.0, double y_ms = 0.0, double z_ms = 0.0);

  Velocity(const double vel[3], double unit = 1.0);

  Velocity operator+(const Velocity& r) const;

  Velocity operator-(const Velocity& r) const;

  bool is_equal(const Interval& interval, double precision = Unit::us) const;

  Speed speed() const;

  Velocity inv() const;

  Speed along(const Vector& direction) const;

  Position travel(double seconds) const;

  Position travel(const Interval& t) const;

  Position operator*(const Interval& t) const { return travel(t); }

  std::string str() const override;

  static const Velocity& stationary();

  static const Velocity& invalid();
};

/**
 * A scalar rate of movement in space reflecting the magnitude of a (relative) velocity and
 * possibly signed to reflect its direction (+ away vs - towards) also. If the speed is signed,
 * it is such that that for a velocity __v__ it is:
 *
 *  Speed( - __v__ ) = - Speed( __v__ ).
 *
 *
 * @sa Position
 * @ingroup util, spectral
 */
class Speed : public Validating {
protected:
  double _ms;       ///< [m/s] stored speed

public:
  Speed(double m_per_s);

  Speed(const Distance& d, const Interval& time);

  Speed operator+(const Speed& r) const;

  Speed operator-(const Speed& r) const;

  /**
   * Returns the magnitude of this speed, as a unsigned speed.
   *
   * @return    The absolute value of the (possibly signed) speed value represented by this instance.
   */
  Speed abs() const;

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

/**
 * Spherical coordinates (longitude, latitude, and distance), representing a direction on sky /
 * location in space.
 *
 * @sa Position, Equatorial, Ecliptic, Galactic, Horizontal
 * @ingroup util
 */
class Spherical : public Validating {
protected:
  Angle _lon;           ///< [rad] stored longitude value
  Angle _lat;           ///< [rad] stored latitude value
  Distance _distance;   ///< [m] stored distance

  Spherical();

public:
  virtual ~Spherical() {}; // something virtual to make class polymorphic for dynamic casting.

  Spherical(double longitude_rad, double latitude_rad, double distance_m = NOVAS_DEFAULT_DISTANCE);

  Spherical(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  Spherical(const Position& pos);

  Position xyz() const;

  const Angle& longitude() const;

  const Angle& latitude() const;

  const Distance& distance() const;

  virtual const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const;
};

/**
 * Equatorial coordinates (RA, Dec = &alpha;, &delta;) and distance, representing the direction on
 * the sky, or location in space, for a particular type of equatorial coordinate reference system,
 * relative to the equator and equinox on that system.
 *
 * @ingroup equatorial
 */
class Equatorial : public Spherical {
private:
  EquatorialSystem _sys;
  double _jd;

  void validate();

public:
  Equatorial(double ra_rad, double dec_rad, const EquatorialSystem &system = EquatorialSystem::icrs(), double distance_m = NOVAS_DEFAULT_DISTANCE);

  Equatorial(const Angle& ra, const Angle& dec, const EquatorialSystem& system = EquatorialSystem::icrs(), const Distance& distance = Distance::at_Gpc());

  Equatorial(const Position& pos, const EquatorialSystem& system = EquatorialSystem::icrs());

  TimeAngle ra() const;

  const Angle& dec() const;

  const EquatorialSystem& system() const;

  enum novas::novas_reference_system reference_system() const;

  Equatorial at_jd(long jd_tt) const;

  Equatorial at_time(const Time& time) const;

  Equatorial to_system(const EquatorialSystem& system) const;

  Equatorial to_icrs() const;

  Ecliptic as_ecliptic() const;

  Galactic as_galactic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Equatorial& invalid();
};

/**
 * Ecliptic coordinates (_l_, _b_ or &lambda;, &beta;) and distance, representing the direction on
 * the sky, or location in space, for a particular type of equatorial coordinate reference system,
 * relative to the ecliptic and equinox of that system.
 *
 * @ingroup nonequatorial
 */
class Ecliptic : public Spherical {
private:
  EquatorialSystem _sys;     ///< stored catalog system

  void validate();

public:
  Ecliptic(double longitude_rad, double latitude_rad, const EquatorialSystem &system = EquatorialSystem::icrs(), double distance_m = NOVAS_DEFAULT_DISTANCE);

  Ecliptic(const Angle& ra, const Angle& dec, const EquatorialSystem &system = EquatorialSystem::icrs(), const Distance& distance = Distance::at_Gpc());

  Ecliptic(const Position& pos, const EquatorialSystem& system = EquatorialSystem::icrs());

  const EquatorialSystem& system() const;

  Ecliptic at_jd(long jd_tt) const;

  Ecliptic at_time(const Time& time) const;

  Ecliptic to_system(const EquatorialSystem& system) const;

  enum novas::novas_reference_system reference_system() const;

  Ecliptic to_icrs() const;

  Equatorial as_equatorial() const;

  Galactic as_galactic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Ecliptic& invalid();
};

/**
 * Galactic coordinates (_l_, _b_) and distance, representing the direction on the sky, or
 * location in space relative to the galactic plane and the nominal Galactic center location.
 *
 * @ingroup nonequatorial
 */
class Galactic : public Spherical {
public:
  Galactic(double longitude_rad, double latitude_rad, double distance_m = NOVAS_DEFAULT_DISTANCE);

  Galactic(const Angle& longitude, const Angle& latitude, const Distance& distance = Distance::at_Gpc());

  Galactic(const Position& pos);

  Equatorial as_equatorial() const;

  Ecliptic as_ecliptic() const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Galactic& invalid();
};

/**
 * A physical temperature value, which can be instantiated, and then expressed, in different
 * commonly used temperature units (C, K, or F). Within SuperNOVAS it is normally used to
 * express ambient temperatures at an observing location, but users may utilize it in any
 * other context also.
 *
 * @sa Weather
 * @ingroup util
 */
class Temperature : public Validating {
private:
  double _deg_C;        ///< [C] stored temperature

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

/**
 * A physical pressure value, which can be instantiated, and then expressed, in different
 * commonly used pressure units (kPa, mbar, torr, atm, and more). Within SuperNOVAS it is
 * normally used to express atmospheric pressure at an observing location, but users may
 * utilize it in any other context also.
 *
 * @sa Weather
 * @ingroup util
 */
class Pressure : public Validating {
private:
  double _pascal;     ///< [Pa] stored pressure

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

/**
 * Weather data, mainly for atmopsheric refraction correction for Earth-based (geodetic)
 * observers.
 *
 * @sa Horizontal::to_refracted(), Horizontal::to_unrefracted(), Site
 * @ingroup refract
 */
class Weather : public Validating {
private:
  Temperature _temperature;   ///< stored temperature value
  Pressure _pressure;         ///< stored pressure value
  double _humidity;           ///< [%] stored humidity value

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

/**
 * IERS Earth Orientation Parameters (EOP). IERS publishes daily values, short-term and medium
 * term forecasts, and historical data for the measured, unmodelled (by the IAU 2006
 * precession-nutation model), _x_<sub>p</sub>, _y_<sub>p</sub> pole offsets, leap-seconds (UTC -
 * TAI difference), and the current UT1 - UTC time difference.
 *
 * The _x_<sub>p</sub>, _y_<sub>p</sub> pole offsets define the true rotationa pole of Earth vs
 * the dynamical equator of date, while the leap_seconds and UT1 - UTC time difference trace the
 * variations in Earth's rotation.
 *
 * Beyond the published values, one may further apply corrections for diurnal effects of
 * libration and the ocean tides, if precision below the milliarcsecond (mas) level is desired.
 * And, the EOP values can be converted to different ITRF realizations to match the ITRF Site
 * specification, if &mu;as precision is required (e.g for VLBI interferometry).
 *
 * EOP are necessary both for defining or accessing astronomical times of the UT1 timescale (e.g.
 * for sidereal time or Earth-rotation angle (ERA) calculations), or for converting coordinates
 * between the preudo Earth-fixed Terrestrial Intermediate Reference System (TIRS) on the
 * dynamical equator of date, and the Earth-fixed International Terrestrial Reference System
 * (ITRS) on the true rotational equator.
 *
 * @sa Time, GeodeticObserver, Apparent::to_itrs(), Geometric::to_itrs(), Horizontal::to_apparent()
 * \ingroup earth nonequatorial
 */
class EOP : public Validating {
private:
  int _leap;          ///< [s] store leap seconds (UTC - TAI time difference).
  Angle _xp;          ///< stored x pole offset (at midhight UTC).
  Angle _yp;          ///< stored y pole offset (at midnight UTC).
  double _dut1;       ///< [s] stored UT1 - UTC time difference.
  double _dxp = 0.0;  ///< [arcsec] applied x pole correction, in NOVAS units.
  double _dyp = 0.0;  ///< [arcsec] applied y pole correction, in NOVAS units.
  double _dt = 0.0;   ///< [s] applied dUT1 corrections, in NOVAS units.

  void validate();

public:
  EOP(int leap_seconds, double dut1_sec = 0.0, double xp_rad = 0.0, double yp_rad = 0.0);

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

/**
 * An Earth-based (geodetic) observer site location, or airborne observer location. Positions may
 * be defined as GPS / WGS84 or else as ITRF / GRS80 geodetic locations, or as Cartesian geocentric
 * _xyz_ positions in the International Terrestrial Reference Frame (ITRF).
 *
 * The class provides the means to convert between ITRF realizations, e.g. to match the ITRF
 * realization used for the Eath Orientation Parameters (EOP) obtained from IERS, for &mu;as
 * precision. (This is really only necessary for VLBI interferometry). Alternatively, one may also
 * transform the EOP values to match the ITRF realization of the site.
 *
 * @sa GeodeticObserver, EOP
 * @ingroup observer
 */
class Site : public Validating {
private:
  novas::on_surface _site;    ///< stored site information

  Site();

public:

  Site(double longitude_rad, double latitude_rad, double altitude_m = 0.0, enum novas::novas_reference_ellipsoid ellipsoid = novas::NOVAS_GRS80_ELLIPSOID);

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
};

/**
 * An observer location. Both Earth-bound (geodetic sites, airborne, or Earth-orbit), and locations elsewhere in
 * the Solar-system are supported.
 *
 * @sa Frame
 * @ingroup observer
 */
class Observer : public Validating {
protected:
  novas::observer _observer = {};   ///< stored observer data

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

/**
 * An observer location at a geodetic (longitude, latitude, altitude) location at the surface or
 * above it (e.g. in an aircraft or balloon). The observer may be fixed at that location, or else
 * moving with some velocity over the ground.
 *
 * @sa GeocentricObserver
 * @ingroup observer
 */
class GeodeticObserver : public Observer {
private:
  EOP _eop;     ///< stored Earth orientation parameters

public:
  GeodeticObserver(const Site& site, const EOP& eop);

  GeodeticObserver(const Site& site, const Velocity& vel, const EOP& eop);

  bool is_geodetic() const override { return true; }

  Site site() const;

  const EOP& eop() const;
};

/**
 * An observer location and motion, defined relative to the geocenter, such as for an Earth-orbit
 * sattelite, or for a virtual observer located at the geocenter itself.
 *
 * @sa GeodeticObserver
 * @ingroup observer
 */
class GeocentricObserver : public Observer {
public:
  GeocentricObserver();

  GeocentricObserver(const Position& pos, const Velocity& vel);

  bool is_geocentric() const override { return true; }

  Position geocentric_position() const;

  Velocity geocentric_velocity() const;
};

/**
 * An observer location anywhere in the Solar System, defined by its momentary barycentric
 * position and velocity vectors.
 *
 * @ingroup observer
 */
class SolarSystemObserver : public Observer {
public:

  SolarSystemObserver();

  SolarSystemObserver(const Position& pos, const Velocity& vel);

  Position ssb_position() const;

  Velocity ssb_velocity() const;
};

/**
 * Precise astronomical time specification, supporting all relevant astronomical timescales (UT1,
 * UTC, TAI, GPS, TT, TDB, TCG, and TCB).
 *
 * @sa Interval, TimeAngle, Observer
 * @ingroup time
 */
class Time : public Validating {
private:
  novas::novas_timespec _ts;    ///< stored astronomical time specification

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

  Time operator+(const Interval &delta) const;

  Time operator-(const Interval &delta) const;

  bool operator<(const Time &other) const;

  bool operator>(const Time &other) const;

  bool operator<=(const Time &other) const;

  bool operator>=(const Time &other) const;

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

/**
 * An observing frame, defined by an observer location and precise time of observation. Frames can
 * be created with full (default) and reduced accuracy, supporting calculations with mas, or &mu;as
 * precisions typically. However note that full accuracy frames require SuperNOVAS to be configured
 * with an appropriate high-precision planet ephemeris provider (see e.g. `novas_use_calceph()` or
 * `novas_use_cspice()`), or else the resulting full-accuracy frame will be invalid.
 *
 * Reduced accuracy frames may also be invalid if the low precision planet ephemeris provider (
 * which, by default, calculates approximate positions for the Earth and Sun only) cannot provide
 * position for the Earth, Sun, the observer.
 *
 * Therefore, one is strongly advised to check the validity of an observing frame after instantiation
 * (using the is_valid() method), or else use the static Frame::create() function to return an
 * optional.
 *
 * @sa Source
 * @ingroup frame
 */
class Frame : public Validating {
private:
  novas::novas_frame _frame = {}; ///< Stored frame data
  Observer _observer;             ///< stored observer data
  Time _time;                     ///< stored time data

public:
  // TODO Should we use an optional here in case of high-accuracy failure?
  Frame(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY);

  const novas::novas_frame *_novas_frame() const;

  const Observer& observer() const;

  const Time& time() const;

  enum novas::novas_accuracy accuracy() const;

  Apparent approx_apparent(const Planet& planet, enum novas::novas_reference_system system = novas::NOVAS_TOD) const;

  bool has_planet_data(enum novas::novas_planet planet) const;

  bool has_planet_data(const Planet& planet) const;

  std::optional<Position> ephemeris_position(enum novas::novas_planet planet) const;

  std::optional<Position> ephemeris_position(const Planet& planet) const;

  std::optional<Velocity> ephemeris_velocity(enum novas::novas_planet planet) const;

  std::optional<Velocity> ephemeris_velocity(const Planet& planet) const;

  double clock_skew(enum novas::novas_timescale = novas::NOVAS_TT) const;

  static std::optional<Frame> create(const Observer& obs, const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY);

  static const Frame& invalid();
};


/**
 * An astronomical source, or target of observation.
 *
 * @sa CatalogSource, Planet, EphemerisSource, OrbitalSource
 * @ingroup source
 */
class Source : public Validating {
protected:
  struct novas::novas_object _object = {};     /// stored data on source

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

  std::optional<Time> rises_above(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  std::optional<Time> transits(const Frame &frame) const;

  std::optional<Time> sets_below(double el, const Frame &frame, novas::RefractionModel ref, const Weather& weather) const;

  static void set_case_sensitive(bool value);
};

/**
 * Defines the cataloged parameters of a sidereal source, such as a star, a Galactic cloud, a distant
 * galaxy or a quasar.
 *
 * @sa CatalogSource
 * @ingroup source spectral
 */
class CatalogEntry : public Validating {
private:
  novas::cat_entry _entry = {};  ///< stored catalog entry
  EquatorialSystem _sys;            ///< stored catalog system

  void set_epoch();

public:
  CatalogEntry(const std::string &name, const Equatorial& coords);

  CatalogEntry(const std::string &name, const Ecliptic& coords);

  CatalogEntry(const std::string &name, const Galactic& coords);

  CatalogEntry(novas::cat_entry e, const EquatorialSystem& system = EquatorialSystem::icrs());

  const novas::cat_entry* _cat_entry() const;

  const EquatorialSystem& system() const;

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

/**
 * A sidereal source, defined by its catalog coordinates and other catalog parameters.
 *
 * @ingroup source
 */
class CatalogSource : public Source {
private:
  EquatorialSystem _system;      ///< stored catalog system

public:
  CatalogSource(const CatalogEntry& e);

  const novas::cat_entry * _cat_entry() const;

  CatalogEntry catalog_entry() const;
};

/**
 * An abstract class of a source in the Solar-system.
 *
 * @sa Planet, EphemerisSource, OrbitalSource
 * @ingroup source
 */
class SolarSystemSource : public Source {
protected:
  SolarSystemSource() {}

public:
  double solar_illumination(const Frame& frame) const;

  double helio_distance(const Time& time, double *rate = NULL) const;

  double solar_power(const Time& time) const;
};

/**
 * A major planet (including Pluto), or the Sun, Moon, Solar-system Barycenter (SSB), Earth-Moon
 * Barycenter (EMB), or the Pluto-system barycenter. Planet positions are usually provided
 * by the JPL DE ephemeris files, such as DE440 or DE440s. By default SuperNOVAS calculates
 * approximate position for the Earth and Sun only. Thus to provide ephemeris positions for
 * all planet-type osurces, you will have to configure a Solar-system ephemeris provider, e.g.
 * via `novas::novas_use_calceph()` or `novas::novas_use_cspice()`.
 *
 * @sa EphemerisSource, OrbitalSource, novas::novas_use_calceph(), novas::novas_use_cspice().
 * @ingroup source
 */
class Planet : public SolarSystemSource {
public:
  Planet(enum novas::novas_planet number);

  enum novas::novas_planet novas_id() const;

  int naif_id() const;

  int de_number() const;

  double mean_radius() const;

  double mass() const;

  static std::optional<Planet> for_naif_id(long naif);

  static std::optional<Planet> for_name(const std::string& name);

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

/**
 * A Solar-system source, whose positions / velocities are provided from ephemeris data. SuperNOVAS
 * does not support ephemeris data by itself, but can interface to other libraries (e.g. CALCEPH or
 * CSPICE), or provide them via user-selected function(s). Depending on the external implementation
 * that provides ephemeris data, sources may be looked up by name or ID number. Name-based lookup
 * may be case-sensitive (in which case you may want to use `novas::novas_case_sensitive()` to enable
 * case-sensitive source names in SuperNOVAS). ID-based lookup may use NAIF IDs, or else some other
 * numbering convention. In any case, you should construct your ephemeris source to match the
 * lookup method used by the ephemeris provider function(s) or library you will be using.
 *
 * @sa Planet, OrbitalSource, novas::novas_use_calceph(), novas::novas_use_cspice(),
 *     novas::novas_case_sensitive()
 * @ingroup source
 */
class EphemerisSource : public SolarSystemSource {
public:
  EphemerisSource(const std::string &name, long number);

  long number() const;
};

/**
 * A Solar-system source, whose position and velocity can be calculated using Keplerian orbital
 * elements. While Keplerian orbitals are not typically accurate for long-term predictions, they
 * can be accurate in the short term, provided that one uses appropriate up-to-date orbital
 * elements, e.g. such as published by the Minor Planet Center (daily or otherwise regularly)
 * for asteroids, comets, an Near-Earth Objects (NEOs). For newly discovered objects, the
 * Keplerian orbital elements by the MPC may be the most accurate, or the only, source of
 * information.
 *
 * @sa EphemerisSource, Planet
 * @ingroup source
 */
class OrbitalSource : public SolarSystemSource {
private:
  OrbitalSource(const std::string& name, long number, const novas::novas_orbital orbit);

public:
  const novas::novas_orbital *_novas_orbital() const;

  Position orbital_position(const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  Velocity orbital_velocity(const Time& time, enum novas::novas_accuracy accuracy = novas::NOVAS_FULL_ACCURACY) const;

  static std::optional<OrbitalSource> from_orbit(const std::string& name, long number, const novas::novas_orbital orbit);
};


/**
 * Apparent position on sky as seen by an observer at a specific time of observation. Apparent
 * positions are corrected for aberration for a movig observer, and gravitational deflection
 * around the major Solar-system bodies along the path of visibility.
 *
 * The apparent position of a source is where it appears to the observer on the celestial sphere.
 * As such it is mainly a direction on sky, which is corrected for light-travel time (i.e. where
 * the source was at the time light originated from the Solar-system body, or the differential
 * light-travel time between the Solar-system barycenter and the observer location for sidereal
 * sources).
 *
 * Unlike geometric positions, the apparent location is also corrected for the observer's motion
 * (aberration), as well as gravitational deflection around the major Solar-system bodies. Also,
 * the radial valocity _v_<sub>r</sub> is calculated to reflect a spectroscopic measure, defined
 * by the relation:
 *
 * &lambda;<sub>obs</sub> / &lambda;<sub>rest</sub> = ( (1.0 + _v_<sub>r</sub> / _c_) / (1.0 - _v_<sub>r</sub> / _c_) )<sup>1/2</sup>
 *
 * As such it contains appropriate relativistic corrections for the observer's relative motion
 * as well as gravitational redshift corrections for the surface of major Solar-system bodies,
 * where light is assumed to originate, and at the observer location. It is also corrected for
 * the viewing angle difference when light is gravitationally deflected around major Solar-system
 * bodies.
 *
 * Apparent positions can also come directly from observations, such as from unrefracted
 * horizontal coordinates.
 *
 * @sa Source::apparent(), Horizontal::to_apparent()
 * \ingroup apparent spectral
 */
class Apparent : public Validating {
private:
  Frame _frame;                               ///< stored frame data
  enum novas::novas_reference_system _sys;    ///< stored coordinate reference system type

  novas::sky_pos _pos;                        ///< stored apparent position data

  Apparent(const Frame& frame, enum novas::novas_reference_system system);

  Apparent(novas::sky_pos p, const Frame& frame, enum novas::novas_reference_system system = novas::NOVAS_TOD);

public:
  Apparent(double ra_rad, double dec_rad, const Frame& frame, double rv_ms = 0.0, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  Apparent(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv, enum novas::novas_reference_system system = novas::NOVAS_TOD);

  const novas::sky_pos *_sky_pos() const;

  const Frame& frame() const;

  enum novas::novas_reference_system system() const;

  Position xyz() const;

  Speed radial_velocity() const;

  Distance distance() const;

  Equatorial equatorial() const;

  Ecliptic ecliptic() const;

  Galactic galactic() const;

  std::optional<Horizontal> horizontal() const;

  Apparent in_system(enum novas::novas_reference_system system) const;

  Apparent in_icrs() const { return in_system(novas::NOVAS_ICRS); }

  Apparent in_j2000() const { return in_system(novas::NOVAS_J2000); }

  Apparent in_mod() const { return in_system(novas::NOVAS_MOD); }

  Apparent in_tod() const { return in_system(novas::NOVAS_TOD); }

  Apparent in_cirs() const { return in_system(novas::NOVAS_CIRS); }

  Apparent in_tirs() const { return in_system(novas::NOVAS_TIRS); }

  std::optional<Apparent> in_itrs(const EOP& eop = EOP::invalid()) const;

  static std::optional<Apparent> from_sky_pos(novas::sky_pos p, const Frame& frame,
          enum novas::novas_reference_system system = novas::NOVAS_TOD);

  static const Apparent& invalid();
};

/**
 * The geometric (3D) position and velocity of a source relative to an observer location. It
 * denotes spatial location and velocity of the source at the time light originated from it, prior
 * to detection by the observer. As such, geometric positions are necessarily antedated for light
 * travel time (for Solar-system sources) or corrected for the differential light-travel between
 * the Solar-system barycenter and the observer location (for sidereal sources).
 *
 * In other words, geometric positions are not the same as ephemeris positions for the equivalent
 * time for Solar-system bodies. Rather, geometric positions match the ephemeris positions for
 * an earlier time, when the observed light originated from the source.
 *
 * @sa apparent
 * @ingroup geometric
 */
class Geometric : public Validating {
private:
  Frame _frame;                             ///< stored frame data
  Position _pos;                            ///< stored geometric position w.r.t. observer
  Velocity _vel;                            ///< stored geometric velocity w.r.t. observer
  enum novas::novas_reference_system _sys;  ///< stored coordinate reference system type

public:
  Geometric(const Position& p, const Velocity& v, const Frame& frame, enum novas::novas_reference_system system = novas::NOVAS_TOD);

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

  std::optional<Geometric> in_itrs(const EOP& eop = EOP::invalid()) const;

  static const Geometric& invalid();
};

/**
 * Horizontal (azimuth, elevation = Az/El) sky coordinates at a geodetic observing location, such
 * as an observatory site, an aircraft, or a balloon. These represent positions relative to the
 * local horizon and meridian, and can be used for both unrefracted (astrometric) or refracted
 * (observed) values or for conbverting between those two.
 *
 * @sa Apparent, Site, Weather
 * @ingroup nonequatorial
 */
class Horizontal : public Spherical {
private:
  Horizontal();

public:
  Horizontal(double azimuth, double elevation, double distance = NOVAS_DEFAULT_DISTANCE);

  Horizontal(const Angle& azimuth, const Angle& elevation, const Distance& distance = Distance::at_Gpc());

  const Angle& azimuth() const;

  const Angle& elevation() const;

  const Angle zenith_angle() const;

  Horizontal to_refracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  Horizontal to_unrefracted(const Frame &frame, novas::RefractionModel ref, const Weather& weather);

  std::optional<Apparent> to_apparent(const Frame& frame, double rv = 0.0, double distance = NOVAS_DEFAULT_DISTANCE) const;

  std::optional<Apparent> to_apparent(const Frame& frame, const Speed& rv = Speed::stationary(), const Distance& distance = Distance::at_Gpc()) const;

  const std::string str(enum novas::novas_separator_type separator = novas::NOVAS_SEP_UNITS_AND_SPACES, int decimals = 3) const override;

  static const Horizontal& invalid();
};




} // namespace supernovas

#  endif /* cplusplus */
#endif /* INCLUDE_SUPERMOVAS_H_ */
