/**
 * @file
 *
 * @date Created  on Oct 10, 2025
 * @author Attila Kovacs
 */

#include <cstring>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

#define UNIX_UTC_J2000    946728000L    ///< 12:00, 1 Jan 2000 (UTC timescale)

using namespace novas;

namespace supernovas {

Calendar::Calendar(enum novas::novas_calendar_type type) : _type(type) {
  _valid = true;
}

enum novas_calendar_type Calendar::type() const {
  return _type;
}

Calendar Calendar::gregorian() {
  return Calendar(NOVAS_GREGORIAN_CALENDAR);
}

Calendar Calendar::roman() {
  return Calendar(NOVAS_ROMAN_CALENDAR);
}

Calendar Calendar::astronomical() {
  return Calendar(NOVAS_ASTRONOMICAL_CALENDAR);
}

CalendarDate Calendar::date(int year, int month, int day, const TimeAngle& time) const {
  return CalendarDate(*this, year, month, day, time);
}

CalendarDate Calendar::date(double jd) const {
  return CalendarDate(*this, jd);
}

CalendarDate Calendar::date(time_t t, long nanos) const {
  return CalendarDate(*this, NOVAS_JD_J2000 + ((t - UNIX_UTC_J2000) + nanos * Unit::ns) / Unit::day);
}

CalendarDate Calendar::date(const struct timespec *ts) const {
  if(!ts) {
    novas_error(0, EINVAL, "Calendar::date", "input Julian Date is NAN");
    return date(NAN);
  }
  return date(ts->tv_sec, ts->tv_nsec);
}

std::optional<CalendarDate> Calendar::parse_date(const std::string& str, enum novas_date_format fmt) const {
  double jd = novas_parse_date_format(_type, fmt, str.c_str(), NULL);
  if(isnan(jd)) {
    novas_trace_invalid("Calendar::parse_date");
    return std::nullopt;
  }
  return date(jd);
}

std::string Calendar::to_string() const {
  switch(_type) {
    case NOVAS_GREGORIAN_CALENDAR:
      return "Gregorian calendar";
    case NOVAS_ROMAN_CALENDAR:
      return "Roman calendar";
    default:
      return "astronomical calendar";
  }
}


CalendarDate::CalendarDate(const Calendar& calendar, int year, int month, int day, const TimeAngle& time)
: _calendar(calendar), _year(year), _month(month), _mday(day), _time_of_day(time), _jd(NAN) {
  _jd = novas_jd_from_date(calendar.type(), year, month, day, time.hours());
  if(isnan(_jd))
    novas_trace_invalid("CalendarDate()");
  else
    _valid = true;
}

CalendarDate::CalendarDate(const Calendar& calendar, double jd)
: _calendar(calendar), _year(-1), _month(-1), _mday(-1), _time_of_day(NAN), _jd(jd) {
  if(isnan(jd))
    novas_error(0, EINVAL, "CalendarDate()", "input Julian Date is NAN");
  else
    _valid = true;

  if(_valid) {
    double hours = NAN;
    novas_jd_to_date(jd, calendar.type(), &_year, &_month, &_mday, &hours);
    _time_of_day = TimeAngle(hours * Unit::hour);
  }
}

CalendarDate CalendarDate::operator+(const Interval& r) const {
  return CalendarDate(calendar(), jd() + r.days());
}

CalendarDate CalendarDate::operator-(const Interval& r) const {
  return CalendarDate(calendar(), jd() - r.days());
}

Interval CalendarDate::operator-(const CalendarDate& r) const {
  return Interval((jd() - r.jd()) * Unit::day);
}

bool CalendarDate::operator<(const CalendarDate& date) const {
  return jd() < date.jd();
}

bool CalendarDate::operator>(const CalendarDate& date) const {
  return jd() > date.jd();
}

bool CalendarDate::operator<=(const CalendarDate& date) const {
  return (jd() - date.jd()) * Unit::day < Unit::ms;
}

bool CalendarDate::operator>=(const CalendarDate& date) const {
  return (date.jd() - jd()) * Unit::day < Unit::ms;
}

bool CalendarDate::equals(const CalendarDate& date, double seconds) const {
  return fabs(jd() - date.jd()) * Unit::day < fabs(seconds);
}

double CalendarDate::jd() const {
  return _jd;
}

double CalendarDate::mjd() const {
  return _jd - NOVAS_JD_MJD0;
}

int CalendarDate::year() const {
  return _year;
}

int CalendarDate::month() const {
  return _month;
}

int CalendarDate::day_of_year() const {
  return novas_day_of_year(_jd, _calendar.type(), NULL);
}

int CalendarDate::day_of_month() const {
  return _mday;
}

int CalendarDate::day_of_week() const {
  return novas_day_of_week(_jd);
}

const TimeAngle& CalendarDate::time_of_day() const {
  return _time_of_day;
}

const std::string& CalendarDate::month_name() const {
  static const std::string names[] = {
          "invalid",
          "January", "February", "March", "April", "May", "June", "July", "August"
          "September", "October", "November", "December"
  };

  if(_month > 0 && _month <= 12)
    return names[_month];

  return names[0];
}

const std::string& CalendarDate::short_month_name() const {
  static const std::string names[] = {
          "inv", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  if(_month > 0 && _month <= 12)
    return names[_month];

  return names[0];
}


const std::string& CalendarDate::day_name() const {
  static const std::string names[] = { "invalid", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
  return names[day_of_week()];
}

const std::string& CalendarDate::short_day_name() const {
  static const std::string names[] = { "inv", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
  return names[day_of_week()];
}

int CalendarDate::break_down(struct tm *tm) const {
  if(!tm)
    return novas_error(-1, EINVAL, "Calendar::break_down", "output tm structure is NULL");

  // Start from a clear slate
  memset(tm, 0, sizeof(*tm));

  tm->tm_year = (_year - 1900);
  tm->tm_mon = (_month - 1);
  tm->tm_mday = _mday;
  tm->tm_hour = (int) _time_of_day.hours();
  tm->tm_min = (int) _time_of_day.minutes() - 60 * tm->tm_hour;
  tm->tm_sec = (int) _time_of_day.seconds() - 3600 * tm->tm_hour - 60 * tm->tm_min;
  tm->tm_yday = day_of_year() - 1;
  tm->tm_wday = day_of_week() % 7;
  tm->tm_isdst = 0;

  return _valid ? 0 : -1;
}

time_t CalendarDate::unix_time() const {
  return  UNIX_UTC_J2000 + (time_t) round((_jd - NOVAS_JD_J2000) * Unit::day);
}

CalendarDate CalendarDate::in_calendar(const Calendar& calendar) const {
  return calendar.date(_jd);
}

Time CalendarDate::to_time(int leap_seconds, double dut1, novas::novas_timescale timescale) const {
  return Time(jd(), leap_seconds, dut1, timescale);
}

Time CalendarDate::to_time(const EOP& eop, novas::novas_timescale timescale) const {
  return Time(jd(), eop, timescale);
}



std::string CalendarDate::to_long_date_string() const {
  return short_day_name() + " " + std::to_string(_mday) + " " + short_month_name() + " " +
          (_year > 0 ? std::to_string(_year) : std::to_string(1 - _year) + " BCE");
}

std::string CalendarDate::to_date_string(enum novas_date_format fmt) const {
  char m[20] = {'\0'}, d[20] = {'\0'};
  std::string y = _year > 0 ? std::to_string(_year) : std::to_string(1 - _year) + "BCE";

  snprintf(m, sizeof(m), "%02d", _month);
  snprintf(d, sizeof(d), "%02d", _mday);

  switch(fmt) {
    case NOVAS_YMD:
      return y + "-" + std::string(m) + "-" + std::string(d);
    case NOVAS_DMY:
      return std::string(d) + "." + std::string(m) + "." + y;
    case NOVAS_MDY:
      return y + "/" + std::to_string(_month) + "-" + std::to_string(_mday);
    default:
      novas_error(0, EINVAL, "CalendarDate::date_string", "invalid format: %d", fmt);
      return "<invalid date format>";
  }
}

std::string CalendarDate::to_string(enum novas_date_format fmt, int decimals) const {
  return to_date_string(fmt) + " " + _time_of_day.to_string(NOVAS_SEP_COLONS, decimals);
}

std::string CalendarDate::to_string(int decimals) const {
  return to_string(NOVAS_YMD, decimals);
}


} // namespace supernovas


