/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

void EOP::validate() {
  static const char *fn = "EOP()";

  if(isnan(_dut1))
    novas_error(0, EINVAL, fn, "input dUT1 is NAN");
  else if(!_xp.is_valid())
    novas_error(0, EINVAL, fn, "input xp is NAN");
  else if(!_yp.is_valid())
    novas_error(0, EINVAL, fn, "input yp is NAN");
  else
    _valid = true;
}


EOP::EOP(int leap_seconds, double dut1_sec, double xp_rad, double yp_rad)
: _leap(leap_seconds), _xp(xp_rad), _yp(yp_rad), _dut1(dut1_sec) {
  validate();
}

EOP::EOP(int leap_seconds, const Interval& dut1, const Angle& xp, const Angle& yp)
: EOP(leap_seconds, dut1.seconds(), xp.rad(), yp.rad()) {
  validate();
}

int EOP::leap_seconds() const {
  return _leap;
}

const Angle& EOP::xp() const {
  return _xp;
}

const Angle& EOP::yp() const {
  return _yp;
}

Interval EOP::dUT1() const {
  return Interval(_dut1);
}

EOP EOP::itrf_transformed(int from_year, int to_year) const {
  double xp1, yp1, t1;
  novas_itrf_transform_eop(from_year, _xp.arcsec(), _yp.arcsec(), _dut1, to_year, &xp1, &yp1, &t1);
  return EOP(_leap, t1, xp1 * Unit::arcsec, yp1 * Unit::arcsec);
}

std::string EOP::to_string() const {
  char sx[20] = {'\0'}, sy[20] = {'\0'}, st[20] = {'\0'};
  snprintf(st, sizeof(st), "dUT1 = %.6f s", _dut1);
  snprintf(sx, sizeof(sx), "xp = %.3f mas", _xp.mas());
  snprintf(sy, sizeof(sy), "yp = %.3f mas", _yp.mas());
  return "EOP ( leap = " + std::to_string(_leap) + ", "   + st + ", " + sx + ", " + sy + " )";
}


const EOP& EOP::invalid() {
  static const EOP _invalid = EOP(0, NAN, NAN, NAN);
  return _invalid;
}

} // namespace supernovas
