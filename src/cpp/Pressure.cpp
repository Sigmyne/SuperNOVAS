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
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), mbar(), bar(), torr(), atm()
 */
Pressure::Pressure(double value) : Scalar(value) {
  static const char *fn = "Pressure()";

  if(!is_valid())
    novas_trace_invalid(fn);
  else if(value < 0.0) {
    novas_set_errno(EINVAL, fn, "input value is negative");
    _valid = false;
  }
}

/**
 * Returns the atmospheric pressure value in pascals.
 *
 * @return    [Pa] the atmospheric pressure
 *
 * @since 1.6
 * @sa hPa(), kPa(), mbar(), bar(), torr(), atm()
 */
double Pressure::Pa() const {
  return _value;
}

/**
 * Returns the atmospheric pressure value in hectopascals.
 *
 * @return    [hPa] the atmospheric pressure
 *
 * @since 1.6
 * @sa Pa(), kPa(), mbar(), bar(), torr(), atm()
 */
double Pressure::hPa() const {
  return 0.01 * _value;
}

/**
 * Returns the atmospheric pressure value in kilopascals.
 *
 * @return    [kPa] the atmospheric pressure
 *
 * @since 1.6
 * @sa Pa(), hPa(), mbar(), bar(), torr(), atm()
 */
double Pressure::kPa() const {
  return 1e-3 * _value;
}

/**
 * Returns the atmospheric pressure value in millibars.
 *
 * @return    [mbar] the atmospheric pressure
 *
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), bar(), torr(), atm()
 */
double Pressure::mbar() const {
  return _value / Unit::mbar;
}

/**
 * Returns the atmospheric pressure value in bars.
 *
 * @return    [bar] the atmospheric pressure
 *
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), mbar(), torr(), atm()
 */
double Pressure::bar() const {
  return _value / Unit::bar;
}

/**
 * Returns the atmospheric pressure value in millimeters of Hg (torr).
 *
 * @return    [torr] the atmospheric pressure (millimeters of Hg).
 *
 * @since 1.6
 * @sa Pressure::Pa(), Pressure::hPa(), Pressure::kPa(), Pressure::mbar(), Pressure::bar(),
 *     Pressure::atm()
 */
double Pressure::torr() const {
  return _value / Unit::torr;
}

/**
 * Returns the atmospheric pressure value in atmospheres.
 *
 * @return    [atm] the atmospheric pressure.
 *
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), mbar(), bar(), torr()
 */
double Pressure::atm() const {
  return _value / Unit::atm;
}

std::string Pressure::SI_unit() const {
  return "Pa";
}

/**
 * Returns a human-readable string representation of this atmospheric pressure in
 * millibars.
 *
 * @param decimals  (optional) [0:16] decimal places to print (default: 3).
 * @return          a new string representation of this pressure in millibars.
 *
 * @since 1.6
 */
std::string Pressure::to_string(int decimals) const {
  char s[40] = {'\0'};
  snprintf(s, sizeof(s), "%.1f mbar", _value / Unit::mbar);
  return std::string(s);
}

/**
 * Returns a new pressure object, with the specified value defined in pascals.
 *
 * @param value   [Pa] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @since 1.6
 * @sa hPa(), kPa(), mbar(), bar(), torr(), atm()
 */
Pressure Pressure::Pa(double value) {
  Pressure p(value);
  if(!p.is_valid())
    novas_trace_invalid("Pressure::Pa(double)");
  return p;
}

/**
 * Returns a new pressure object, with the specified value defined in hectopascals.
 *
 * @param value   [hPa] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @since 1.6
 * @sa Pa(), kPa(), mbar(), bar(), torr(), atm()
 */
Pressure Pressure::hPa(double value) {
  Pressure p(100.0 * value);
  if(!p.is_valid())
    novas_trace_invalid("Pressure::hPa(double)");
  return p;
}

/**
 * Returns a new pressure object, with the specified value defined in kilopascals.
 *
 * @param value   [kPa] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @since 1.6
 * @sa Pa(), hPa(), mbar(), bar(), torr(), atm()
 */
Pressure Pressure::kPa(double value) {
  Pressure p(1000.0 * value);
  if(!p.is_valid())
    novas_trace_invalid("Pressure::kPa(double)");
  return p;
}

/**
 * Returns a new pressure object, with the specified value defined in millibars.
 *
 * @param value   [mbar] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), bar(), torr(), atm()
 */
Pressure Pressure::mbar(double value) {
  Pressure p(value * Unit::mbar);
  if(!p.is_valid())
    novas_trace_invalid("Pressure::mbar(double)");
  return p;
}

/**
 * Returns a new pressure object, with the specified value defined in bars.
 *
 * @param value   [bar] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), mbar(), torr(), atm()
 */
Pressure Pressure::bar(double value) {
  Pressure p(value * Unit::bar);
  if(!p.is_valid())
    novas_trace_invalid("Pressure::bar(double)");
  return p;
}

/**
 * Returns a new pressure object, with the specified value defined in millimeters of Hg (torr).
 *
 * @param value   [torr] atmospheric pressure value in millimeters of Hg.
 * @return        A new pressure object with the specified value.
 *
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), mbar(), bar(), atm()
 */
Pressure Pressure::torr(double value) {
  Pressure p(value * Unit::torr);
  if(!p.is_valid())
    novas_trace_invalid("Pressure::torr(double)");
  return p;
}

/**
 * Returns a new pressure object, with the specified value defined in atmopsheres.
 *
 * @param value   [atm] atmospheric pressure value
 * @return        A new pressure object with the specified value.
 *
 * @since 1.6
 * @sa Pa(), hPa(), kPa(), mbar(), bar(), torr()
 */
Pressure Pressure::atm(double value) {
  Pressure p(value * Unit::atm);
  if(!p.is_valid())
    novas_trace_invalid("Pressure::atm(double)");
  return p;
}

} // namespace supernovas
