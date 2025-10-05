/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"

using namespace novas;


namespace supernovas {

EOP::EOP(int leap_seconds, double dut1, double xp, double yp)
: _leap(leap_seconds), _xp(xp), _yp(yp), _t(dut1) {}

EOP::EOP(int leap_seconds, double dut1, const Angle& xp, const Angle& yp)
: EOP(leap_seconds, dut1, xp.rad(), yp.rad()) {}

bool EOP::is_valid() const {
  return !isnan(_t) && _xp.is_valid() && _yp.is_valid();
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

double EOP::dUT1() const {
  return _t;
}

EOP EOP::itrf_transformed(int from_year, int to_year) const {
  double xp1, yp1, t1;
  novas_itrf_transform_eop(from_year, _xp.arcsec(), _yp.arcsec(), _t, to_year, &xp1, &yp1, &t1);
  return EOP(_leap, t1, xp1 * Unit::arcsec, yp1 * Unit::arcsec);
}

EOP EOP::diurnal_corrected(const Time& time) const {
  double dxp, dyp, dt;
  novas_diurnal_eop_at_time(time._novas_timespec(), &dxp, &dyp, &dt);

  // Apply correction rel. to prior correction.
  EOP eop = EOP(_leap, _t + dt - _dt, _xp.rad() + (dxp - _dxp) * Unit::arcsec, _yp.rad() + (dyp - _dyp) * Unit::arcsec);

  eop._dxp = dxp;
  eop._dyp = dyp;
  eop._dt = dt;

  return eop;
}

std::string EOP::str() const {
  char sx[20] = {'\0'}, sy[20] = {'\0'}, st[20] = {'\0'};
  snprintf(st, sizeof(st), "dUT1 = %.6f s", _t);
  snprintf(sx, sizeof(sx), "xp = %.3f mas", _xp);
  snprintf(sy, sizeof(sy), "yp = %.3f mas", _yp);
  return "EOP ( leap = " + std::to_string(_leap) + ", "   + st + ", " + sx + ", " + sy + " )";
}

static const EOP _invalid = EOP(0, NAN, NAN, NAN);
const EOP& EOP::invalid() {
  return _invalid;
}

} // namespace supernovas
