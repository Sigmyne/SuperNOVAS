/**
 * @file
 *
 * @date Created  on Oct 1, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

namespace supernovas {


/**
 * Instantiates an atmopsheric pressure with the specified SI value (in pascals).
 *
 * @param value [Pa] the atmospheric pressure
 *
 * @sa Pa(double), hPa(double), kPa(double), mbar(double), bar(double), torr(double), atm(double)
 */
Pressure::Pressure(double value) : _pascal(value) {
  static const char *fn = "Pressure()";

  if(isnan(value))
    novas::novas_error(0, EINVAL, fn, "input value is NAN");
  else if(value < 0.0)
    novas::novas_error(0, EINVAL, fn, "input value is negative");
  else
    _valid = true;
}

/**
 * Returns the atmospheric pressure value in pascals.
 *
 * @return    [Pa] the atmospheric pressure
 *
 * @sa hPa(), kPa(), mbar(), bar(), torr(), atm()
 */
double Pressure::Pa() const {
  return _pascal;
}

/**
 * Returns the atmospheric pressure value in hectopascals.
 *
 * @return    [hPa] the atmospheric pressure
 *
 * @sa Pa(), kPa(), mbar(), bar(), torr(), atm()
 */
double Pressure::hPa() const {
  return 0.01 * _pascal;
}

/**
 * Returns the atmospheric pressure value in kilopascals.
 *
 * @return    [kPa] the atmospheric pressure
 *
 * @sa Pa(), hPa(), mbar(), bar(), torr(), atm()
 */
double Pressure::kPa() const {
  return 1e-3 * _pascal;
}

/**
 * Returns the atmospheric pressure value in millibars.
 *
 * @return    [mbar] the atmospheric pressure
 *
 * @sa Pa(), hPa(), kPa(), bar(), torr(), atm()
 */
double Pressure::mbar() const {
  return _pascal / Unit::mbar;
}

/**
 * Returns the atmospheric pressure value in bars.
 *
 * @return    [bar] the atmospheric pressure
 *
 * @sa Pa(), hPa(), kPa(), mbar(), torr(), atm()
 */
double Pressure::bar() const {
  return _pascal / Unit::bar;
}

/**
 * Returns the atmospheric pressure value in millimeters of Hg (torr).
 *
 * @return    [torr] the atmospheric pressure (millimeters of Hg).
 *
 * @sa Pressure::Pa(), Pressure::hPa(), Pressure::kPa(), Pressure::mbar(), Pressure::bar(),
 *     Pressure::atm()
 */
double Pressure::torr() const {
  return _pascal / Unit::torr;
}

/**
 * Returns the atmospheric pressure value in atmospheres.
 *
 * @return    [atm] the atmospheric pressure.
 *
 * @sa Pa(), hPa(), kPa(), mbar(), bar(), torr()
 */
double Pressure::atm() const {
  return _pascal / Unit::atm;
}

/**
 * Returns a human-readable string representation of this atmospheric pressure in
 * millibars.
 *
 * @return    a new string representation of this pressure in millibars.
 */
std::string Pressure::to_string() const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.1f mbar", _pascal / Unit::mbar);
  return std::string(s);
}

/**
 * Returns a new pressure object, with the specified value defined in pascals.
 *
 * @param value   [Pa] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @sa hPa(double), kPa(double), mbar(double), bar(double), torr(double), atm(double)
 */
Pressure Pressure::Pa(double value) {
  return Pressure(value);
}

/**
 * Returns a new pressure object, with the specified value defined in hectopascals.
 *
 * @param value   [hPa] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @sa Pa(double), kPa(double), mbar(double), bar(double), torr(double), atm(double)
 */
Pressure Pressure::hPa(double value) {
  return Pressure(100.0 * value);
}

/**
 * Returns a new pressure object, with the specified value defined in kilopascals.
 *
 * @param value   [kPa] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @sa Pa(double), hPa(double), mbar(double), bar(double), torr(double), atm(double)
 */
Pressure Pressure::kPa(double value) {
  return Pressure(1000.0 * value);
}

/**
 * Returns a new pressure object, with the specified value defined in millibars.
 *
 * @param value   [mbar] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @sa Pa(double), hPa(double), kPa(double), bar(double), torr(double), atm(double)
 */
Pressure Pressure::mbar(double value) {
  return Pressure(value * Unit::mbar);
}

/**
 * Returns a new pressure object, with the specified value defined in bars.
 *
 * @param value   [bar] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @sa Pa(double), hPa(double), kPa(double), mbar(double), torr(double), atm(double)
 */
Pressure Pressure::bar(double value) {
  return Pressure(value * Unit::bar);
}

/**
 * Returns a new pressure object, with the specified value defined in millimeters of Hg (torr).
 *
 * @param value   [torr] atmospheric pressure value in millimeters of Hg.
 * @return        A new pressure object with the specified value.
 *
 * @sa Pa(double), hPa(double), kPa(double), mbar(double), bar(double), atm(double)
 */
Pressure Pressure::torr(double value) {
  return Pressure(value * Unit::torr);
}

/**
 * Returns a new pressure object, with the specified value defined in atmopsheres.
 *
 * @param value   [atm] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @sa Pa(double), hPa(double), kPa(double), mbar(double), bar(double), torr(double)
 */
Pressure Pressure::atm(double value) {
  return Pressure(value * Unit::atm);
}

} // namespace supernovas
