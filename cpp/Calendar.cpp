/**
 * @file
 *
 * @date Created  on Oct 10, 2025
 * @author Attila Kovacs
 *
 *  Calendars and calendar dates.
 */

#include <cstring>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

#define UNIX_UTC_J2000    946728000L    ///< 12:00, 1 Jan 2000 (UTC timescale)

using namespace novas;

namespace supernovas {

/**
 * Instantiates a new calendar of the specified type.
 *
 * @param type      The type of calendar, e.g. novas::NOVAS_GREGORIAN_CALENDAR,
 *                  novas::NOVAS_ROMAN_CALENDAR, or novas::NOVAS_ASTRONOMICAL calendar. The
 *                  astronomical calendar is the same as the Gregorian calendar after the
 *                  Gregorian calendar reform of 1582, or else the same as the Roman / Julian
 *                  calendar before the reform. The Gregorian calendar can be used also for dates
 *                  that precede the calendar reform, for so called proleptic Gregorian dates.
 *                  Accordingly, ISO 8601 timestamps are always expressed in the Gregorian
 *                  calendar, withour exception.
 */
Calendar::Calendar(enum novas::novas_calendar_type type) : _type(type) {
  _valid = true;
}

/**
 * Returns the type of this calendar, that is whether it is a Gregorian, Roman / Julian,
 * or astronomical calendar. The astronomical calendat is the conventional calendar of date.
 *
 * @return          The type of calendar, e.g. novas::NOVAS_GREGORIAN_CALENDAR,
 *                  novas::NOVAS_ROMAN_CALENDAR, or novas::NOVAS_ASTRONOMICAL calendar.
 */
enum novas_calendar_type Calendar::type() const {
  return _type;
}

/**
 * Returns a new Gregorian calendar instance.
 *
 * @return    a Gregorian calendar instance
 *
 * @sa roman(), astronomical()
 */
Calendar Calendar::gregorian() {
  return Calendar(NOVAS_GREGORIAN_CALENDAR);
}

/**
 * Returns a new Roman / Julian calendar instance.
 *
 * @return    a Roman / Julian calendar instance
 *
 * @sa gregorian(), astronomical()
 */
Calendar Calendar::roman() {
  return Calendar(NOVAS_ROMAN_CALENDAR);
}

/**
 * Returns a new astronomical calendar instance. The astronomical calendat is the conventional
 * calendar of date, that is the Roman / Julian calendar prior to the Gregorian calendar reform
 * of 1582; and the Gregorian calendar after the date of the reform.
 *
 * @return    an astronomical calendar / conventional calendar of date instance.
 *
 * @sa Calendar::roman()
 * @sa Calendar::gregorian()
 */
Calendar Calendar::astronomical() {
  return Calendar(NOVAS_ASTRONOMICAL_CALENDAR);
}

/**
 * Returns a new calendar date instance for the date and time specified in this calendar.
 *
 * @param year    [yr] calendar year
 * @param month   [month] calendar month [1:12]
 * @param day     [day] calendar day-of-month [1:31]
 * @param time    time of day
 * @return        a date instance for the specified date in this calendar.
 *
 * @sa CalendarDate::CalendarDate(), date(), parse_date()
 */
CalendarDate Calendar::date(int year, int month, int day, const TimeAngle& time) const {
  return CalendarDate(*this, year, month, day, time);
}

/**
 * Returns a new calendar date instance, in this calendar, for the specified Julian date.
 *
 * @param jd    [day] Julian Day
 * @return      a new calendar date, in this calendar, corresponding to the specified Julian date.
 *
 * @sa CalendarDate::CalendarDate(), date(), parse_date()
 */
CalendarDate Calendar::date(double jd) const {
  return CalendarDate(*this, jd);
}

/**
 * Returns a new calendar date instance, in this calendar, for the specified UNIX time.
 *
 * @param t       [s] UNIX time (seconds since the Gregorian date of January 1, 1970).
 * @param nanos   [ns] sub-second time component
 * @return        a new calendar date, in this calendar, corresponding to the specified UNIX time.
 *
 * @sa date(), date(double)
 */
CalendarDate Calendar::date(time_t t, long nanos) const {
  return CalendarDate(*this, NOVAS_JD_J2000 + ((t - UNIX_UTC_J2000) + nanos * Unit::ns) / Unit::day);
}

/**
 * Returns a new calendar date instance, in this calendar, for the given astronomical time specification.
 *
 * @param ts    Pointer to the astronomical time specification
 * @return      a new calendar date, in this calendar, corresponding to the specified astronomical
 *              time.
 *
 * @sa date(), parse_date()
 */
CalendarDate Calendar::date(const struct timespec *ts) const {
  if(!ts) {
    novas_set_errno(EINVAL, "Calendar::date", "input Julian Date is NAN");
    return date(NAN);
  }
  return date(ts->tv_sec, ts->tv_nsec);
}

/**
 * Returns a new calendar date instance, in this calendar, for the specified string date / time
 * representation, if possible, or else std::nullopt.
 *
 * @param str   The string date / time specification. There is a lot of flexibility on what
 *              dates/times may be parsed. See novas::novas_parse_date_format() for details.
 * @param fmt   The date representation, such as novas::NOVAS_YMD, novas::NOVAS_DMY, or
 *              novas::NOVAS_MDY, specifying the order in which the year (Y), month (M), and day
 *              (D) components are expected in the input string representation.
 * @return      A new calendar date, in this calendar, or else std::nullopt if the string date
 *              could not be parsed.
 *
 * @sa CalendarDate::to_string(), date()
 */
std::optional<CalendarDate> Calendar::parse_date(const std::string& str, enum novas_date_format fmt) const {
  double jd = novas_parse_date_format(_type, fmt, str.c_str(), NULL);
  if(isnan(jd)) {
    novas_trace_invalid("Calendar::parse_date");
    return std::nullopt;
  }
  return date(jd);
}

/**
 * Returns a string representation of this type of calendar
 *
 * @return    A string representation of the calendar, such as "Gregorian calendar".
 */
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

/**
 * Instantiates a date / time in the specified calendar, using the specified date components.
 *
 * @param calendar    calendar, in which the date is specified.
 * @param year        [yr] calendar year
 * @param month       [month] calendar month [1:12]
 * @param day         [day] calendar day-of-month [1:31]
 * @param time        time of day component.
 *
 * @sa Calendar::date()
 */
CalendarDate::CalendarDate(const Calendar& calendar, int year, int month, int day, const TimeAngle& time)
: _calendar(calendar), _year(year), _month(month), _mday(day), _time_of_day(time), _jd(NAN) {
  _jd = novas_jd_from_date(calendar.type(), year, month, day, time.hours());
  if(isnan(_jd))
    novas_trace_invalid("CalendarDate()");
  else
    _valid = true;
}

/**
 * Instantiates a date / time in the specified calendar, using the specified Julian Date.
 *
 * @param calendar     calendar, in which the date is specified.
 * @param jd           [day] Julian date (in the timescale of preference).
 *
 * @sa equals(), operator!=()
 */
CalendarDate::CalendarDate(const Calendar& calendar, double jd)
: _calendar(calendar), _year(-1), _month(-1), _mday(-1), _time_of_day(NAN), _jd(jd) {
  if(isnan(jd))
    novas_set_errno(EINVAL, "CalendarDate()", "input Julian Date is NAN");
  else
    _valid = true;

  if(_valid) {
    double hours = NAN;
    novas_jd_to_date(jd, calendar.type(), &_year, &_month, &_mday, &hours);
    _time_of_day = TimeAngle(hours * Unit::hour);
  }
}

/**
 * Returns a new calendar date that is offset from this calendar date by the specified time
 * interval.
 *
 * @param r   the offset interval, on the righ-hand-side of the + operator.
 * @return    a new calendar date, at the specified time offset from this one.
 */
CalendarDate CalendarDate::operator+(const Interval& r) const {
  return CalendarDate(calendar(), jd() + r.days());
}

/**
 * Returns a new calendar date that is offset, back in time, from this calendar date by the
 * specified time interval.
 *
 * @param r   the offset interval, on the righ-hand-side of the - operator.
 * @return    a new calendar date, at the specified time offset, backwards in time, from this one.
 */
CalendarDate CalendarDate::operator-(const Interval& r) const {
  return CalendarDate(calendar(), jd() - r.days());
}

/**
 * Returns the difference in time between this calendar date and the specified other calendar
 * date.
 *
 * @param r   the other calendar date, on the right-hand-side of the - operator.
 * @return    the time difference (interval) between this calendar date and the argument.
 */
Interval CalendarDate::operator-(const CalendarDate& r) const {
  return Interval((jd() - r.jd()) * Unit::day);
}

/**
 * Checks if this calendar date is before the specified other date.
 *
 * @param date    the other date to compare to this one.
 * @return        true if this date precedes the other calendar date, or else false.
 */
bool CalendarDate::operator<(const CalendarDate& date) const {
  return jd() < date.jd();
}

/**
 * Checks if this calendar date is after the specified other date.
 *
 * @param date    the other date to compare to this one.
 * @return        true if this date is after the other calendar date, or else false.
 */
bool CalendarDate::operator>(const CalendarDate& date) const {
  return jd() > date.jd();
}

/**
 * Checks if this calendar date is the same or before the specified other date.
 *
 * @param date    the other date to compare to this one.
 * @return        true if this date is the same or before the other calendar date, or else false.
 */
bool CalendarDate::operator<=(const CalendarDate& date) const {
  return (jd() - date.jd()) * Unit::day < Unit::ms;
}

/**
 * Checks if this calendar date is the same or after the specified other date.
 *
 * @param date    the other date to compare to this one.
 * @return        true if this date is the same or after the other calendar date, or else false.
 */
bool CalendarDate::operator>=(const CalendarDate& date) const {
  return (date.jd() - jd()) * Unit::day < Unit::ms;
}

/**
 * Checks if this calendar date is the same as the specified other date, within the specified
 * tolerance.
 *
 * @param date      the other date to compare to this one.
 * @param seconds   [s] tolerance for equality check.
 * @return          `true` if this date is the same as the other calendar date, within the
 *                  specified tolerance, or else `false`.
 *
 * @sa equals(), operator==()
 */
bool CalendarDate::equals(const CalendarDate& date, double seconds) const {
  return fabs(jd() - date.jd()) * Unit::day < fabs(seconds);
}

/**
 * Checks if this calendar date is the same as the specified other date, within the specified
 * tolerance.
 *
 * @param date      the other date to compare to this one.
 * @param precision tolerance for equality check.
 * @return          `true` if this date is the same as the other calendar date, within the
 *                  specified tolerance, or else `false`.
 *
 * @sa equals(), operator==()
 */
bool CalendarDate::equals(const CalendarDate& date, const Interval& precision) const {
  return equals(date, precision.seconds());
}

/**
 * Checks if this calendar date is the same as the specified other date, within 1 &mu;s
 *
 * @param date      the other date to compare to this one.
 * @return          `true` if this date is the same as the other calendar date, within 1 &mu;s, or
 *                  else `false`.
 *
 * @sa equals(), operator!=()
 */
bool CalendarDate::operator==(const CalendarDate& date) const {
  return equals(date);
}

/**
 * Checks if this calendar date differs from the specified other date, by more than 1 &mu;s
 *
 * @param date      the other date to compare to this one.
 * @return          `true` if this date differs from the other calendar date, by more than 1
 *                  &mu;s, or else `false`.
 *
 * @sa equals(), operator!=()
 */
bool CalendarDate::operator!=(const CalendarDate& date) const {
  return !equals(date);
}

/**
 * Returns the Julian Day corresponding to this calendar date.
 *
 * @return    [day] the Julian Date corresponding to this calendar date. Note, that Julian dates
 *            start at noon, not midnight.
 *
 * @sa mjd()
 */
double CalendarDate::jd() const {
  return _jd;
}

/**
 * Returns the Modified Julian Day corresponding to this calendar date.
 *
 * @return    [day] the Modified Julian Day (MJD) corresponding to this calendar date. Modified
 *            Julian days start at midnight.
 *
 * @sa mjd()
 */
double CalendarDate::mjd() const {
  return _jd - NOVAS_JD_MJD0;
}

/**
 * Returns the calendar year component of this date.
 *
 * @return    [yr] the calendar year component. For X BCE, it returns 1 - X. I.e., 45 BCE is
 *            returned as -44.
 *
 * @sa month(), day_of_year()
 */
int CalendarDate::year() const {
  return _year;
}

/**
 * Returns the calendar month component [1:12] of this date
 *
 * @return      [month] calendar month [1:12]
 *
 * @sa month_name(), short_month_name(), year(), day_of_month()
 */
int CalendarDate::month() const {
  return _month;
}

/**
 * Returns the day of year number [1:366] of this date.
 *
 * @return    the day of year number, starting from 1.
 *
 * @sa year(), day_of_month(), day_of_week()
 */
int CalendarDate::day_of_year() const {
  return novas_day_of_year(_jd, _calendar.type(), NULL);
}

/**
 * Returns the day of month component [1:32] of this date
 *
 * @return    [day] day of month component, starting from 1
 *
 * @sa month(), day_of_year(), day_of_week()
 */
int CalendarDate::day_of_month() const {
  return _mday;
}

/**
 * Returns the day of week number [1:7] of this date, starting from 1 (Monday) to 7 (Sunday).
 *
 * @return    [day] the day of the week stating from 1 (Monday) to 7 (Sunday)
 *
 * @sa day_name(), short_day_name(), day_of_month(), day_of_year()
 */
int CalendarDate::day_of_week() const {
  return novas_day_of_week(_jd);
}

/**
 * Returns the time-of-dat component of this calendar date.
 *
 * @return    reference to the the time of day component
 */
const TimeAngle& CalendarDate::time_of_day() const {
  return _time_of_day;
}

/**
 * Returns the full (English) name of the month for this date.
 *
 * @return    the full name of the month, e.g. "January".
 *
 * @sa short_month_name(), month()
 */
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

/**
 * Returns the short 3-letter (English) abbreviation of the month name for this date.
 *
 * @return    the full name of the month, e.g. "Jan".
 *
 * @sa month_name(), month()
 */
const std::string& CalendarDate::short_month_name() const {
  static const std::string names[] = {
          "inv", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  if(_month > 0 && _month <= 12)
    return names[_month];

  return names[0];
}

/**
 * Returns the full (English) name of the day of the week for this date.
 *
 * @return    the full name of the day of the week, e.g. "Wednesday".
 *
 * @sa short_day_name(), day_of_week()
 */
const std::string& CalendarDate::day_name() const {
  static const std::string names[] = { "invalid", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
  int i = day_of_week();
  if(i < 0) i = 0;
  return names[i];
}

/**
 * Returns the short 3-letter (English) abbreviation of the day of the week for this date.
 *
 * @return    the full name of the day of the week, e.g. "Wed".
 *
 * @sa day_name(), day_of_week()
 */
const std::string& CalendarDate::short_day_name() const {
  static const std::string names[] = { "inv", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
  int i = day_of_week();
  if(i < 0) i = 0;
  return names[i];
}

/**
 * Returns the standard POSIX broken-down time representation for this date.
 *
 * @param[out] tm     Pointer to the broken-down time structure in which to return the result.
 * @return            0 if successful, or else -1 if the argument is NULL (errno will be
 *                    set to EINVAL).
 *
 * @sa year(), month(), day_of_year(), day_of_month(), day_of_week(), time_of_day()
 */
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

/**
 * Returns the UNIX time for this calendar date
 *
 * @return    [s] UNIX time, that is seconds since 1 Jan 1970.
 */
time_t CalendarDate::unix_time() const {
  return  UNIX_UTC_J2000 + (time_t) round((_jd - NOVAS_JD_J2000) * Unit::day);
}

/**
 * Returns this date in the specified other calendar.
 *
 * @param calendar    a calendar in which to express this date
 * @return            this date, expressed in the specified other calendar.
 */
CalendarDate CalendarDate::in_calendar(const Calendar& calendar) const {
  return calendar.date(_jd);
}

/**
 * Converts this calendar date to ans astronomical time representation, given the specified
 * Earth Orientation Parameters, and astronomical timescale. Not, that calendar dates have
 * accuracy at the 0.1 ms level only, hence the resulting astronomical time will be limited
 * to the same level of precision also.
 *
 * @param leap_seconds    [s] the leap seconds of date, that is the TAI - UTC time difference.
 * @param dut1            [s] the UT1 - UTC time difference for the date
 * @param timescale       the astronomical timescale in which this calendar date is defined.
 * @return                an astronomical time instance for this date and input parameters.
 */
Time CalendarDate::to_time(int leap_seconds, double dut1, novas::novas_timescale timescale) const {
  return Time(jd(), leap_seconds, dut1, timescale);
}

/**
 * Converts this calendar date to ans astronomical time representation, given the specified
 * Earth Orientation Parameters, and astronomical timescale. Not, that calendar dates have
 * accuracy at the 0.1 ms level only, hence the resulting astronomical time will be limited
 * to the same level of precision also.
 *
 * @param eop             Earth Orientation Parameters (EOP) for this date.
 * @param timescale       the astronomical timescale in which this calendar date is defined.
 * @return                an astronomical time instance for this date and input parameters.
 */
Time CalendarDate::to_time(const EOP& eop, novas::novas_timescale timescale) const {
  return Time(jd(), eop, timescale);
}

/**
 * Returns the date component in long-form string representation. E.g. "11 November 2026",
 * or "12 September 490 BCE".
 *
 * @return      the string representation of the date (only) in long form.
 *
 * @sa to_date_string(), to_string()
 */
std::string CalendarDate::to_long_date_string() const {
  return short_day_name() + " " + std::to_string(_mday) + " " + short_month_name() + " " +
          (_year > 0 ? std::to_string(_year) : std::to_string(1 - _year) + " BCE");
}

/**
 * Returns the numerical date only representation, in the specified component order, e.g.
 * as "2025-01-31" (for novas::NOVAS_YMD), "31.01.2025" (for novas::NOVAS_DMY), or as
 * "1/31/2025" for novas::NOVAS_MDY).
 *
 * @param fmt     The format / order of year (Y), month (M), and day (D) components.
 *
 * @return      the string representation of the date (only) in the specified format.
 *
 * @sa to_long_date_string(), to_string()
 */
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
      return std::to_string(_month) + "/" + std::to_string(_mday) + "/" + y;
    default:
      novas_set_errno(EINVAL, "CalendarDate::date_string", "invalid format: %d", fmt);
      return "<invalid date format>";
  }
}

/**
 * Returns a string representation of this calendar date, given the requested date format and
 * decimal places to print for the seconds component.
 *
 * @param fmt       (optional) The order of the date components (default: `NOVAS_YMD`)
 * @param decimals  (optional) Number of decimal places to print for the seconds (default: 3)
 * @return          The string representation of the date.
 *
 * @sa to_string(int), to_date_string(), to_long_date_string()
 */
std::string CalendarDate::to_string(enum novas_date_format fmt, int decimals) const {
  return to_date_string(fmt) + " " + _time_of_day.to_string(NOVAS_SEP_COLONS, decimals);
}

/**
 * Returns a string representation of this calendar date, using the YMD format and the specified
 * decimal places to print for the seconds component.
 *
 * @param decimals  Number of decimal places to print for the seconds.
 * @return          The string representation of the date.
 *
 * @sa to_date_string(), to_long_date_string()
 */
std::string CalendarDate::to_string(int decimals) const {
  return to_string(NOVAS_YMD, decimals);
}


} // namespace supernovas


