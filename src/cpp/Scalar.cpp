
/**
 * @file
 *
 * @date Created  on Apr  2, 2026
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"



namespace supernovas {

/**
 * Instantiates a scalar quantity with an initializing value in standard S.I. units.
 *
 * @param si_value    the initializing value in S.I. units.
 */
Scalar::Scalar(double si_value)
: _value(si_value) {
  if(isfinite(_value))
    _valid = true;
  else
    novas_set_errno(EINVAL, "Scalar()", "initializing value is NAN or infinite.");
}

/**
 * The value of this scalar quantity in S.I. units. All concrete subclasses must implement
 * this.
 *
 * @return    the value in the standard S.I. units.
 */
double Scalar::SI_value() const {
  return _value;
}

/**
 * Checks if this scalar quantity is the same as another, within the specified precision.
 * Two scalar quantities are equal only if they have the same S.I. units and the same
 * S.I. values within the specified precision.
 *
 * @param other       Another scalar quantity.
 * @param precision   Precision for the comparison in the same S.I. units as this quantity.
 * @return            `true` if the two scalars have the same S.I. units and values
 *                    within the specified precision, otherwise `false`.
 */
bool Scalar::equals(const Scalar& other, double precision) const {
  return fabs(_value - other._value) <= precision && SI_unit() == other.SI_unit();
}

/**
 * Returns a human readable representation of this scalar quantity. Subclasses may override the
 * default implementation to provide their own.
 *
 * @param decimals    (optional) [0:16] Number of decimal places to print (default: 3).
 * @return            a string representation of this scalar quantity.
 */
std::string Scalar::to_string(int decimals) const {
  char str[40] = {'\0'};
  novas_print_decimal(_value, decimals, str, (int) sizeof(str));
  return std::string(str) + " " + SI_unit();
}

} // namespace supernovas
