/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include <string.h>
#include <time.h>

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("Time");

  int n = 0;

  EOP eop(32.0, 0.1, 0.2, 0.3);

  if(!test.check("invalid", !Time::invalid().is_valid())) n++;
  if(!test.check("invalid jd = NAN", !Time(NAN, 32, 0.1, NOVAS_UTC).is_valid())) n++;
  if(!test.check("invalid fjd = NAN", !Time((long)NOVAS_JD_J2000, NAN, 32, 0.1, NOVAS_UTC).is_valid())) n++;
  if(!test.check("invalid timescale", !Time(NOVAS_JD_J2000, 32, 0.0, (enum novas_timescale) -1).is_valid())) n++;
  if(!test.check("invalid DUT1 = NAN", !Time(NOVAS_JD_J2000, 32, NAN, NOVAS_UTC).is_valid())) n++;
  if(!test.check("invalid DUT1 < -1.0", !Time(NOVAS_JD_J2000, 32, -1.01, NOVAS_UTC).is_valid())) n++;
  if(!test.check("invalid DUT1 > 1.0", !Time(NOVAS_JD_J2000, 32, 1.01, NOVAS_UTC).is_valid())) n++;
  if(!test.check("invalid struct timespec  = NULL", !Time((struct timespec *) NULL, eop).is_valid())) n++;
  if(!test.check("invalid novas_timespec  = NULL", !Time((novas_timespec *) NULL).is_valid())) n++;
  if(!test.check("invalid 'blah'", !Time("blah", eop).is_valid())) n++;

  Time a((long) NOVAS_JD_J2000, 0.0, eop, NOVAS_UTC);

  if(!test.check("is_valid()", a.is_valid())) n++;
  if(!test.equals("leap_seconds()", a.leap_seconds(), 32)) n++;
  if(!test.equals("dUT1()", a.dUT1().seconds(), 0.1, 1e-3)) n++;
  if(!test.equals("jd(UTC)", a.jd(NOVAS_UTC), NOVAS_JD_J2000, 1e-8)) n++;
  if(!test.equals("mjd(UTC)", a.mjd(NOVAS_UTC), NOVAS_JD_J2000 - NOVAS_JD_MJD0, 1e-8)) n++;
  if(!test.equals("jd_day(UTC)", (a + 1e-9).jd_day(NOVAS_UTC), (long) NOVAS_JD_J2000)) n++; // avoid rounding error at midnight...
  if(!test.equals("jd_frac(UTC)", (a + 1e-9).jd_frac(NOVAS_UTC), 0.0, 1e-8)) n++;
  if(!test.equals("mjd_day(UTC)", a.mjd_day(NOVAS_UTC), (long) (NOVAS_JD_J2000 - NOVAS_JD_MJD0))) n++;
  if(!test.equals("mjd_frac(UTC)", a.mjd_frac(NOVAS_UTC), 0.5, 1e-8)) n++;
  if(!test.equals("time_of_day(UTC)", a.time_of_day(NOVAS_UTC).hours(), 12.0, 1e-7)) n++;
  if(!test.equals("day_of_week(UTC)", a.day_of_week(NOVAS_UTC), novas_day_of_week(a.jd(NOVAS_UTC)), 1e-7)) n++;
  if(!test.equals("moon_phase()", a.moon_phase().deg(), novas_moon_phase(a.jd(NOVAS_TDB)), 1e-9)) n++;
  if(!test.equals("next_moon_phase()", a.next_moon_phase(Angle(90.0 * Unit::deg)).jd(NOVAS_TDB), novas_next_moon_phase(a.jd(NOVAS_TDB), 90.0), 1e-9)) n++;
  if(!test.equals("to_epoch_string(3)", a.to_epoch_string(3), "J2000.000")) n++;
  if(!test.equals("to_epoch_string(-1)", a.to_epoch_string(-1), "J2000")) n++;
  if(!test.equals("to_epoch_string(13)", a.to_epoch_string(13), a.to_epoch_string(12))) n++;
  if(!test.equals("to_calendar_date()", a.to_calendar_date(NOVAS_TT).jd(), a.jd(), 1e-8)) n++;

  char str[40] = {'\0'};
  novas_iso_timestamp(a._novas_timespec(), str, sizeof(str));
  if(!test.equals("to_iso_string()", a.to_iso_string(), std::string(str))) n++;

  Time a1(a.to_string(NOVAS_UTC), eop, NOVAS_UTC);
  if(!test.check("to_string() -> Time(string)", a1.equals(a, 1e-5))) n++;

  Time a2 = a1 + Interval(1.0);
  if(!test.equals("operator+()", novas_diff_time(a2._novas_timespec(), a1._novas_timespec()), 1.0, 1e-12)) n++;
  if(!test.check("operator!=()", a2 != a1)) n++;
  if(!test.check("operator>()", a2 > a1)) n++;
  if(!test.check("operator<()", a1 < a2)) n++;
  if(!test.check("operator>=() >", a2 >= a1)) n++;
  if(!test.check("operator<=() <", a1 <= a2)) n++;
  if(!test.check("operator>=() !", !(a1 >= a2))) n++;
  if(!test.check("operator<=() !", !(a2 <= a1))) n++;
  if(!test.equals("operator-(Time)", (a2 - a1).seconds(), 1.0, 1e-12)) n++;
  if(!test.check("operator-(Interval)", (a2 - Interval(1.0)) == a1)) n++;
  if(!test.check("offset_from(invalid timescale)", !a2.offset_from(a1, (enum novas_timescale) -1).is_valid())) n++;
  if(!test.check("shifted(Interval)", a1.shifted(Interval(1.0)) == a2)) n++;

  struct timespec tu = {}, tu1 = {};
  tu.tv_sec = novas_get_unix_time(a._novas_timespec(), &tu.tv_nsec);
  tu1.tv_sec = a.unix_time(&tu1.tv_nsec);
  if(!test.equals("unix_time() seconds", tu1.tv_sec, tu.tv_sec)) n++;
  if(!test.equals("unix_time() nanos", tu1.tv_nsec, tu.tv_nsec)) n++;

  Time a3(&tu, eop);
  if(!test.check("Time(struct timespec *)", a3 == a)) n++;

  novas_timespec ts = {};
  novas_set_split_time(NOVAS_UTC, (long) NOVAS_JD_J2000, 0.0, 32, 0.1, &ts);
  if(!test.equals("leap_seconds(novas_timespec *)", Time::leap_seconds(&ts), 32)) n++;
  if(!test.equals("leap_seconds(novas_timespec = NULL)", Time::leap_seconds(NULL), -1)) n++;

  Time b(&ts);
  if(!test.check("operator==()", b.equals(a, Interval(Unit::us)))) n++;
  if(!test.check("operator==()", b == a)) n++;
  if(!test.check("operator<=() ==", b <= a)) n++;
  if(!test.check("operator>=() ==", b >= a)) n++;
  if(!test.check("operator!=() !", !(b != a))) n++;
  if(!test.check("operator>() !", !(b > a))) n++;
  if(!test.check("operator<() !", !(b < a))) n++;


  if(!test.equals("mjd(UTC)", a.mjd(NOVAS_UTC), NOVAS_JD_J2000 - NOVAS_JD_MJD0, 1e-8)) n++;
  Time c((long) NOVAS_JD_J2000, 0.0, eop, NOVAS_TT);
  if(!test.equals("jd()", c.jd(), NOVAS_JD_J2000, 1e-8)) n++;
  if(!test.equals("mjd()", c.mjd(), NOVAS_JD_J2000 - NOVAS_JD_MJD0, 1e-8)) n++;
  if(!test.check("operator!=()", c != a)) n++;
  if(!test.equals("epoch()", c.epoch(), 2000.0, 1e-12)) n++;
  if(!test.equals("era()", c.era().deg(), era(NOVAS_JD_J2000, -ts.ut1_to_tt), 1-14)) n++;
  if(!test.equals("gmst()", c.gmst().hours(), novas_gmst(NOVAS_JD_J2000, -ts.ut1_to_tt), 1-14)) n++;
  if(!test.equals("gst()", c.gst().hours(), novas_gast(NOVAS_JD_J2000 - ts.ut1_to_tt, ts.ut1_to_tt, NOVAS_FULL_ACCURACY), 1-14)) n++;

  Site site(33.0 * Unit::deg, -21.0 * Unit::deg, 3000.0 * Unit::m);
  if(!test.equals("gst()", c.lst(site).hours(), novas_time_lst(c._novas_timespec(), site.longitude().deg(), NOVAS_FULL_ACCURACY), 1-14)) n++;

  Time d = Time::from_mjd(51544.5, eop, NOVAS_UTC);
  if(!test.check("from_mjd()", d == a)) n++;

  novas_timespec ts1 = ts; ts1.fjd_tt = NAN;
  if(!test.check("invalid timespec.fjd = NULL", !Time(&ts1).is_valid())) n++;
  ts1 = ts; ts1.ut1_to_tt = NAN;
  if(!test.check("invalid timespec.ut1_to_tt = NAN", !Time(&ts1).is_valid())) n++;
  ts1 = ts; ts1.tt2tdb = NAN;
  if(!test.check("invalid timespec.ut1_to_tt = NAN", !Time(&ts1).is_valid())) n++;

  timespec_get(&tu, TIME_UTC);
  Time e = Time::now(eop);
  tu1.tv_sec = e.unix_time(&tu1.tv_nsec);
  if(!test.equals("now()", (tu1.tv_sec + 1e-9 * tu1.tv_nsec), (tu.tv_sec + 1e-9 * tu.tv_nsec), 0.1));

  std::cout << "Time.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
