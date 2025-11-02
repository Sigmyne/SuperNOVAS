/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

#define UNIX_J2000    946728000L     ///< 12:00, 1 Jan 2000 (UTC timescale)

using namespace novas;

int main() {
  TestUtil test = TestUtil("Calendar");

  int n = 0;

  Calendar a = Calendar::gregorian();
  if(!test.equals("gregorian()", a.type(), NOVAS_GREGORIAN_CALENDAR)) n++;

  Calendar b = Calendar::roman();
  if(!test.equals("roman()", b.type(), NOVAS_ROMAN_CALENDAR)) n++;

  Calendar c = Calendar::astronomical();
  if(!test.equals("astronomical()", c.type(), NOVAS_ASTRONOMICAL_CALENDAR)) n++;

  if(!test.equals("to_stirng(Gregorian)", a.to_string(), "Gregorian calendar")) n++;
  if(!test.equals("to_string(Roman)", b.to_string(), "Roman calendar")) n++;
  if(!test.equals("to_string(astronomical)", c.to_string(), "astronomical calendar")) n++;

  if(!test.check("parse_date(blah)", !a.parse_date("blah").has_value())) n++;
  if(!test.check("date(time=NAN)", !a.date(2000, 1, 1, TimeAngle(NAN)).is_valid())) n++;

  CalendarDate dx = a.date(NAN);
  if(!test.check("date(jd=NAN)", !dx.is_valid())) n++;
  if(!test.equals("year() invalid", dx.year(), -1)) n++;
  if(!test.equals("month() invalid", dx.month(), -1)) n++;
  if(!test.equals("day_of_month() invalid", dx.day_of_month(), -1)) n++;
  if(!test.check("time_of_day() invalid", !dx.time_of_day().is_valid())) n++;
  if(!test.equals("month_name() invalid", dx.month_name(), "invalid")) n++;
  if(!test.equals("short_month_name() invalid", dx.short_month_name(), "inv")) n++;
  if(!test.equals("day_name() invalid", dx.day_name(), "invalid")) n++;
  if(!test.equals("short_day_name() invalid", dx.short_day_name(), "inv")) n++;
  if(!test.check("parse_date(J2000)", a.parse_date("2000-01-01 12:00:00").has_value())) n++;

  CalendarDate da = a.date(2000, 1, 1, TimeAngle::noon());
  if(!test.equals("J2000", da.jd(), NOVAS_JD_J2000)) n++;
  if(!test.equals("J2000", da.mjd(), NOVAS_JD_J2000 - NOVAS_JD_MJD0, 1e-8)) n++;
  if(!test.equals("year()", da.year(), 2000)) n++;
  if(!test.equals("month()", da.month(), 1)) n++;
  if(!test.equals("day_of_month()", da.day_of_month(), 1)) n++;
  if(!test.equals("time_of_day()", da.time_of_day().hours(), 12.0, 1e-14)) n++;
  if(!test.equals("day_of_year()", da.day_of_year(), 1)) n++;
  if(!test.equals("day_of_week()", da.day_of_week(), novas_day_of_week(NOVAS_JD_J2000))) n++;
  if(!test.equals("month_name()", da.month_name(), "January")) n++;
  if(!test.equals("short_month_name()", da.short_month_name(), "Jan")) n++;
  if(!test.equals("day_name()", da.day_name(), "Saturday")) n++;
  if(!test.equals("short_day_name()", da.short_day_name(), "Sat")) n++;
  if(!test.equals("to_time(EOP)", da.to_time(EOP(32, 0.0, 0.0, 0.0), NOVAS_TT).jd(), da.jd(), 1e-8)) n++;
  if(!test.equals("to_time()", da.to_time(32, 0.0, NOVAS_TT).jd(), da.jd(), 1e-8)) n++;

  if(!test.equals("break_down(NULL)", da.break_down(NULL), -1)) n++;

  struct tm tm = {};
  if(!test.equals("break_down()", da.break_down(&tm), 0)) n++;
  if(!test.equals("break_down() -> year", tm.tm_year, 100)) n++;
  if(!test.equals("break_down() -> month", tm.tm_mon, 0)) n++;
  if(!test.equals("break_down() -> mday", tm.tm_mday, 1)) n++;
  if(!test.equals("break_down() -> yday", tm.tm_yday, 0)) n++;
  if(!test.equals("break_down() -> wday", tm.tm_wday, 6)) n++;
  if(!test.equals("break_down() -> hour", tm.tm_hour, 12)) n++;
  if(!test.equals("break_down() -> min", tm.tm_min, 0)) n++;
  if(!test.equals("break_down() -> sec", tm.tm_sec, 0)) n++;
  if(!test.equals("break_down() -> isdst", tm.tm_isdst, 0)) n++;

  if(!test.equals("unix_time()", da.unix_time(), UNIX_J2000)) n++;

  CalendarDate db = b.parse_date("2000-01-01 12:00:00").value();

  CalendarDate dc = c.date(NOVAS_JD_J2000);
  if(!test.equals("J2000 (gregorian vs astronomical)", da.jd(), dc.jd())) n++;

  CalendarDate db1 = (db - Interval(13 * Unit::day)).in_calendar(a);
  if(!test.equals("J2000 (gregorian vs roman)", da.jd(), db1.jd())) n++;

  if(!test.check("operator ==", da == dc)) n++;
  if(!test.check("operator !=", da != db)) n++;

  if(!test.check("operator >=", da >= dc)) n++;
  if(!test.check("operator <=", da <= dc)) n++;

  if(!test.check("operator >= (>)", db >= da)) n++;
  if(!test.check("operator <= (<)", da <= db)) n++;

  if(!test.check("operator >", db > da)) n++;
  if(!test.check("operator <", da < db)) n++;

  CalendarDate dd = a.date(2000, 1, 1, TimeAngle::hours(13.0));
  if(!test.check("operator +(Interval)", dd == da + Interval(1.0 * Unit::hour))) n++;
  if(!test.check("operator -(Interval)", da == dd - Interval(1.0 * Unit::hour))) n++;
  if(!test.check("operator -(CalendarDate)", (dd - da).equals(Interval(1.0 * Unit::hour), Unit::ms))) n++;

  struct timespec ts;
  ts.tv_sec = UNIX_J2000;
  ts.tv_nsec = 0L;

  CalendarDate de = c.date(&ts);
  if(!test.equals("date(struct timespec)", da.jd(), de.jd(), 1e-8)) n++;
  if(!test.check("date(NULL)", !c.date((struct timespec *) NULL).is_valid())) n++;

  if(!test.equals("to_string()", da.to_string(), "2000-01-01 12:00:00")) n++;
  if(!test.equals("to_string(3)", da.to_string(3), "2000-01-01 12:00:00.000")) n++;

  if(!test.equals("to_date_string(YMD)", da.to_date_string(novas::NOVAS_YMD), "2000-01-01")) n++;
  if(!test.equals("to_date_string(DMY)", da.to_date_string(novas::NOVAS_DMY), "01.01.2000")) n++;
  if(!test.equals("to_date_string(MDY)", da.to_date_string(novas::NOVAS_MDY), "1/1/2000")) n++;
  if(!test.equals("to_date_string(-1)", da.to_date_string((enum novas_date_format) -1), "<invalid date format>")) n++;

  if(!test.equals("to_long_date_string()", da.to_long_date_string(), "Sat 1 Jan 2000")) n++;


  std::cout << "Calendar.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
