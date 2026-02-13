/**
 * @file
 *
 * @date Created  on Jan 29, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the time-related functions of the SuperNOVAS C/C++ library
 *
 *  Link with
 *
 *  ```
 *   -lsupernovas
 *  ```
 */

#include <iostream>

#include <supernovas.h>      ///< SuperNOVAS functions and definitions

using namespace novas;
using namespace supernovas;

// Below are some Earth orientation values. Here we define them as constants, but they may
// of course be variables. They should be set to the appropriate values for the time
// of observation based on the IERS Bulletins or data service...

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.

int main() {

  // -------------------------------------------------------------------------
  // Earth orientation parameters (EOP), as appropriate for the time of observation,
  // e.g. as obtained from IERS bulletins or data service:
  EOP eop(LEAP_SECONDS, DUT1, POLAR_DX * Unit::mas, POLAR_DY * Unit::mas);

  // -------------------------------------------------------------------------
  // 1.a. Simple dates from strings...

  // Use a UTC string date-time specification in YMD format
  // (It could also be TAI, or GPS, or TDB...)
  Time time1("2025-01-29T18:09:29.333+0200", eop, NOVAS_UTC);


  // -------------------------------------------------------------------------
  // 1.b. Dates from strings more generally...

  // - Parse a date/time string in YMD format -- there is a fait bit of
  //   flexibility on how the components are separated, but they must be year,
  //   month, day, then optionally time and possibly time zone also. For
  //   example:
  std::optional<CalendarDate> date = Calendar::gregorian().parse_date("2025-01-29T18:09:29.333+0200");

  // Check that the parsing was successful.
  if(!date.has_value()) {
    std::cerr << "ERROR! could not parse date string.\n";
    return 1;
  }


  // Alternatively, you could use a string time that also contains a timescale
  // specification:
  // TODO

  // - Next, convert that date to an astronomical time of a specific time
  //   scale. Let's say the above date was on the TAI... (It could be UTC, or
  //   GPS, or TDB...)
  time1 = date.value().to_time(eop, NOVAS_TAI)

  // -------------------------------------------------------------------------
  // 1.c. Dates from broken-down time

  // - Use year, month, day, and time-of-day, e.g. 1974-01-31 23:30
  //   The date is in the astronomical calendar of date (as opposed to a fixed
  CalendarDate date2(Calendar::astronomical(), 1974, 1, 31, TimeAngle("23:30:00"));

  // - Next, convert that date to an astronomical time of a specific time
  //   scale. Let's say the above date was on the TAI... (It could be UTC, or
  //   GPS, or TDB...)
  Time time2 = date2.to_time(LEAP_SECONDS, DUT1, NOVAS_TAI)


  // -------------------------------------------------------------------------
  // 1.d. UNIX time

  // We'll set unix_time to current time, but it could be a UNIX timestamp...
  struct timespec unix_time = {};

#if __STDC_VERSION__ >= 201112L || defined(_MSC_VER)
  timespec_get(&unix_time, TIME_UTC);
#else
  clock_gettime(CLOCK_REALTIME, &unix_time);
#endif

  // Use the UNIX time (seconds + nanoseconds) to define astrometric time
  Time time3 = Time(&unix_time, LEAP_SECONDS, DUT1);

  // -------------------------------------------------------------------------
  // 1.e. Current time

  // Use the current UNIX time to set astronomical time
  time3 = Time::now(eop);


  // -------------------------------------------------------------------------
  // 2. Extract specific timescales from the astronomical time

  // - Get a TDB-based JD date for time1
  double jd = time1.jd(NOVAS_TDB);

  // - Or for higher precision, get a split JD...
  long ijd = time1.jd_day(NOVAS_TDB);
  double fjd = time1.jd_frac(NOVAS_TDB);
  std::cout << "Split date is " << ijd << " / " << fjd << "\n";



  // -------------------------------------------------------------------------
  // 3. Measure time difference

  // - Difference in Earth-based time measures (UTC, GPS, TAI, TT)
  Interval dt = time1 - time2;
  std::cout << " UTC Time difference is " << dt.to_string() << "\n";

  // - Difference in TCG -- time progresses differently outside of Earth gravity
  std::cout << " TCG Time difference is " << time1.offset_from(time2, NOVAS_TCG).to_string() << "\n";

  // - Difference in TCB -- time progresses differently outside of Earth gravity
  std::cout << " TCB Time difference is " << time1.offset_from(time2, NOVAS_TCB).to_string() << "\n";

  // -------------------------------------------------------------------------
  // 4. Offset time

  // - Add 5.31 seconds to time1.
  time1 += 5.31 * Unit::s;


  // -------------------------------------------------------------------------
  // 5. Print time

  // - Print an UTC-based ISO timestamp to a string
  std::cout << " Offset time is: " time1.to_iso_string() << "\n";

  // - Print a timestamp in a specific time measure, e.g. GPS time
  std::cout << " in TDB        : " << time1.to_string(NOVAS_TDB) <<"\n";


  // -------------------------------------------------------------------------
  // 6. Get Calendar date in specific calendar

  // - Get a Broken down date, say in the Julian/Roman calendar
  struct tm tm = {};
  time1.to_calendar_date(Calendar::roman()).break_down(&tm);


  return 0;
}

