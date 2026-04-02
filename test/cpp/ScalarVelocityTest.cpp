/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"




int main() {
  TestUtil test = TestUtil("ScalarVelocity");

  int n = 0;

  ScalarVelocity x(NAN);
  if(!test.check("is_valid(NAN)", !x.is_valid())) n++;
  if(!test.check("isnan(NAN)", isnan(x.m_per_s()))) n++;
  if(!test.equals("zero()", ScalarVelocity::stationary().m_per_s(), 0.0)) n++;
  if(!test.check("abs(NAN)", !x.abs().is_valid())) n++;
  if(!test.check("NAN.travel()", !x.travel(1.0).is_valid())) n++;
  if(!test.check("NAN.operator*()", !(x * Interval(1.0)).is_valid())) n++;
  if(!test.check("NAN.operator+()", !(x + ScalarVelocity::stationary()).is_valid())) n++;
  if(!test.check("NAN.operator-()", !(x - ScalarVelocity::stationary()).is_valid())) n++;

  if(!test.check("is_valid(> c)", !ScalarVelocity(Constant::c + 1.0).is_valid())) n++;

  ScalarVelocity a(45.0 * Unit::km / Unit::s);
  if(!test.check("is_valid(45 km/s)", a.is_valid())) n++;
  if(!test.equals("m_per_s()", a.m_per_s(), 45000.0, 1e-11)) n++;
  if(!test.equals("km_per_s()", a.km_per_s(), 45.0)) n++;
  if(!test.equals("au_per_day()", a.au_per_day(), 45.0 * Unit::km * Unit::day / Unit::au, 1e-10)) n++;
  if(!test.equals("beta()", a.beta(), 45.0 * Unit::km / Unit::sec / Constant::c, 1e-20)) n++;
  if(!test.equals("Gamma()", a.Gamma(), 1.0 / sqrt(1.0 - a.beta() * a.beta()), 1e-15)) n++;
  if(!test.equals("redshift()", a.redshift(), a.beta(), 1e-6)) n++;
  if(!test.equals("travel()", a.travel(Interval(10.0)).km(), 450.0, 1e-12)) n++;
  if(!test.equals("operator * Interval", (a * Interval(10.0)).km(), 450.0, 1e-12)) n++;
  if(!test.equals("in_direction()", a.in_direction(Position(0.0, 5.0, 0.0)).y().km_per_s(), 45.0, 1e-15)) n++;
  if(!test.equals("in_direction(x)", a.in_direction(Position(0.0, 5.0, 0.0)).x().m_per_s(), 0.0, 1e-15)) n++;
  if(!test.check("in_direction(invalid)", !a.in_direction(Position::undefined()).is_valid())) n++;
  if(!test.check("travel(NAN)", !a.travel(NAN).is_valid())) n++;
  if(!test.equals("SI_unit()", a.SI_unit(), "m/s")) n++;
  if(!test.equals("SI_value()", a.SI_value(), a.m_per_s(), 0.0)) n++;
  if(!test.check("operator*(invalid interval)", !(a * Interval(NAN)).is_valid())) n++;
  if(!test.check("operator+(invalid)", !(a + x).is_valid())) n++;
  if(!test.check("operator-(invalid)", !(a - x).is_valid())) n++;

  ScalarVelocity b(-30.0 * Unit::km / Unit::s);
  if(!test.equals("km_per_s(-30 km/s)", b.km_per_s(), -30.0)) n++;
  if(!test.equals("abs(-30 km/s)", b.abs().km_per_s(), 30.0)) n++;

  a.equals(a, ScalarVelocity(0.1));

  if(!test.check("a.equals(a, tol)", a.equals(a, ScalarVelocity(0.1)))) n++;
  if(!test.check("!a.equals(b, tol)", !a.equals(b, ScalarVelocity(0.1)))) n++;
  if(!test.check("operator ==", a == a)) n++;
  if(!test.check("operator !=", a != b)) n++;

  if(!test.equals("operator +", (a + b).km_per_s(), 15.0, 1e-5)) n++;
  if(!test.equals("operator -", (a - b).km_per_s(), 75.0, 1e-5)) n++;

  if(!test.check("from_redshift(NAN)", !ScalarVelocity::from_redshift(NAN).is_valid())) n++;

  ScalarVelocity c = ScalarVelocity::from_redshift(0.1);
  if(!test.equals("from_redshift()", c.km_per_s(), novas_z2v(0.1) , 1e-11)) n++;

  ScalarVelocity d = Coordinate(90.0 * Unit::km) / Interval(2.0 * Unit::sec);
  if(!test.equals("from_redshift()", d.km_per_s(), 45.0, 1e-14)) n++;

  if(!test.equals("to_string()", a.to_string(), "45.000 km/s")) n++;

  std::cout << "ScalarVelocity.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
