/**
 * @file
 *
 *  Various functions to parse string specifications of angles, time, dates, and coordinate epochs.
 *
 * @date Created  on Mar 2, 2025
 * @author Attila Kovacs
 *
 * @sa target.c, observer.c, timescale.c
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#ifndef _MSC_VER
#  include <strings.h>            // strcasecmp() / strncasecmp() -- POSIX.1-2001 / 4.4BSD
#endif

#include "novas.h"

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif

/// \cond PRIVATE
#define DATE_SEP_CHARS  "-_./ \t\r\n\f"             ///< characters that may separate date components
#define DATE_SEP        "%*[" DATE_SEP_CHARS "]"    ///< Parse pattern for ignored date separators

/// Parse pattern for month specification, either as a 1-2 digit integer or as a month name or abbreviation.
#define MONTH_SPEC      "%9[^" DATE_SEP_CHARS "]"
/// \endcond

#if __Lynx__ && __powerpc__
// strcasecmp() / strncasecmp() are not defined on PowerPC / LynxOS 3.1
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
#elif defined(_MSC_VER)
#  define strcasecmp _stricmp                       /// MSVC equivalent
#  define strncasecmp _strnicmp                     /// MSVC equivalent
#endif

/**
 * Returns the Julian day corresponding to an astronomical coordinate epoch.
 *
 * @param system        Coordinate system, e.g. "ICRS", "B1950.0", "J2000.0", "FK4", "FK5",
 *                      "1950", "2000", or "HIP". In general, any Besselian or Julian year epoch
 *                      can be used by year (e.g. "B1933.193" or "J2022.033"), or else the fixed
 *                      values listed. If 'B' or 'J' is ommitted in front of the epoch year, then
 *                      Besselian epochs are assumed prior to 1984.0, and Julian epochs after.
 * @return              [day] The Julian day corresponding to the given coordinate epoch, or else
 *                      NAN if the input string is NULL or the input is not recognised as a
 *                      coordinate epoch specification (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa make_cat_object_sys(), make_redshifted_object_sys(), transform_cat(), precession()
 * @sa NOVAS_SYSTEM_ICRS, NOVAS_SYSTEM_B1950, NOVAS_SYSTEM_J2000, NOVAS_SYSTEM_HIP
 */
double novas_epoch(const char *restrict system) {
  static const char *fn = "novas_epoch";

  double year;
  char type = 0, *tail = NULL;

  if(!system) {
    novas_set_errno(EINVAL, fn, "epoch is NULL");
    return NAN;
  }

  if(!system[0]) {
    novas_set_errno(EINVAL, fn, "epoch is empty");
    return NAN;
  }

  if(strcasecmp(&system[1], "CRS") == 0)
    return NOVAS_JD_J2000;

  if(strcasecmp(system, NOVAS_SYSTEM_FK6) == 0)
    return NOVAS_JD_J2000;

  if(strcasecmp(system, NOVAS_SYSTEM_FK5) == 0)
    return NOVAS_JD_J2000;

  if(strcasecmp(system, NOVAS_SYSTEM_FK4) == 0)
    return NOVAS_JD_B1950;

  if(strcasecmp(system, NOVAS_SYSTEM_HIP) == 0)
    return NOVAS_JD_HIP;

  if(toupper(system[0]) == 'B') {
    type = 'B';
    system++;
  }
  else if(toupper(system[0]) == 'J') {
    type = 'J';
    system++;
  }

  errno = 0;

  year = strtod(system, &tail);
  if(tail <= system) {
    novas_set_errno(EINVAL, fn, "Invalid epoch: %s", system);
    return NAN;
  }

  if(!type)
    type = year < 1984.0 ? 'B' : 'J';

  if(type == 'J')
    return NOVAS_JD_J2000 + (year - 2000.0) * NOVAS_JULIAN_YEAR_DAYS;

  return NOVAS_JD_B1950 + (year - 1950.0) * NOVAS_BESSELIAN_YEAR_DAYS;
}

/**
 * Parses the decimal hours for a HMS string specification. The hour, minute, and second
 * components may be separated by spaces, tabs, colons `:`, underscore `_`, or a combination
 * thereof. Additionally, the hours and minutes may be separated by the letter `h`, and the
 * minutes and seconds may be separated by `m` or a single quote `'`. The seconds may be followed
 * by 's' or double quote `"`.
 *
 * There is no enforcement on the range of hours that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range of 0-24h.
 *
 * For example, all of the lines below are valid specifications:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999
 *  23h59'59.999
 *  23 59 59.999
 *  23 59
 * </pre>
 *
 * At least the leading two components (hours and minutes) are required. If the seconds are
 * ommitted, they will be assumed zero, i.e. `23:59` is the same as `23:59:00.000`.
 *
 * @param hms         String specifying hours, minutes, and seconds, which correspond to
 *                    a time between 0 and 24 h. Time in any range is permitted, but the minutes
 *                    and seconds must be &gt;=0 and &lt;60.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed time.
 * @return        [hours] Corresponding decimal time value, or else NAN if there was an
 *                error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_hms_hours(), novas_parse_hours(), novas_print_hms(), novas_parse_dms()
 */
double novas_parse_hms(const char *restrict hms, char **restrict tail) {
  static const char *fn = "novas_hms_hours";

  int h = 0, m = 0, n = 0, k = 0;
  double s = 0.0;
  char next;

  if(tail)
    *tail = (char *) hms;

  if(!hms) {
    novas_set_errno(EINVAL, fn, "input string is NULL");
    return NAN;
  }
  if(!hms[0]) {
    novas_set_errno(EINVAL, fn, "input string is empty");
    return NAN;
  }

  if(sscanf(hms, "%d%*[:hH _\t]%d%n%*[:mM'’ _\t]%lf%n", &h, &m, &n, &s, &n) < 2) {
    novas_set_errno(EINVAL, fn, "not in HMS format: '%s'", hms);
    return NAN;
  }

  if(m < 0 || m >= 60) {
    novas_set_errno(EINVAL, fn, "invalid minutes: got %d, expected 0-59", m);
    return NAN;
  }

  if(s < 0.0 || s >= 60.0) {
    novas_set_errno(EINVAL, fn, "invalid seconds: got %f, expected [0.0:60.0)", s);
    return NAN;
  }

  // Trailing seconds marker (if any)
  sscanf(&hms[n], "%*[ _\t]%*[s\"”]%n", &k);

  // The trailing markers must be standalone (end of string or followed by white space)
  next = hms[n + k];
  if(next == '\0' || next == '_' || isspace(next) || ispunct(next))
    n += k;

  if(tail)
    *tail += n;

  s = abs(h) + (m / 60.0) + (s / 3600.0);
  return h < 0 ? -s : s;
}

/**
 * Returns the decimal hours for a HMS string specification. The hour, minute, and second
 * components may be separated by spaces, tabs, colons `:`, or a combination thereof.
 * Additionally, the hours and minutes may be separated by the letter `h`, and the minutes
 * and seconds may be separated by `m` or a single quote `'`. The seconds may be followed by 's'
 * or double quote `"`.
 *
 * There is no enforcement on the range of hours that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range of 0-24h.
 *
 * For example, all of the lines below specify the same time:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999s
 *  23h59'59.999
 *  23 59 59.999
 *  23 59
 *  23h
 * </pre>
 *
 * At least the leading two components (hours and minutes) are required. If the seconds are
 * ommitted, they will be assumed zero, i.e. `23:59` is the same as `23:59:00.000`.
 *
 * NOTES:
 * <ol>
 * <li> To see if the string was fully parsed when returning a valid (non-NAN) value, you can
 * check `errno`: it should be zero (0) if all non-whitespace characters have been parsed from
 * the input string, or else `EINVAL` if the parsed value used only the leading part of the
 * string.</li>
 * </ol>
 *
 * @param hms     String specifying hours, minutes, and seconds, which correspond to
 *                a time between 0 and 24 h. Time in any range is permitted, but the minutes and
 *                seconds must be &gt;=0 and &lt;60.
 * @return        [hours] Corresponding decimal time value, or else NAN if there was an
 *                error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_hours(), novas_parse_hms(), novas_dms_degrees()
 */
double novas_hms_hours(const char *restrict hms) {
  char *tail = (char *) hms;
  double h = novas_parse_hms(hms, &tail);
  if(isnan(h))
    return novas_trace_nan("novas_hms_hours");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return h;
}

static int parse_compass(const char *restrict str, int *n) {
  char compass[7] = {'\0'};
  int from = 0;

  *n = 0;

  // Skip underscores and white spaces
  while(str[from] && (str[from] == '_' || isspace(str[from]) || ispunct(str[from]))) from++;

  // Compass direction (if any)
  if(sscanf(&str[from], "%6s", compass) > 0) {
    int i;

    for(i = 0; compass[i]; i++)
      if(compass[i] == '_' || ispunct(compass[i])) {
        compass[i] = '\0';
        break;
      }

    if(strcmp(compass, "N") == 0 || strcmp(compass, "E") == 0 ||
            strcasecmp(compass, "north") == 0 || strcasecmp(compass, "east") == 0) {
      *n = from + i;
      return 1;
    }
    else if (strcmp(compass, "S") == 0 || strcmp(compass, "W") == 0 ||
            strcasecmp(compass, "south") == 0 || strcasecmp(compass, "west") == 0) {
      *n = from + i;
      return -1;
    }
  }

  return 0;
}

/**
 * Parses the decimal degrees for a DMS string specification. The degree, (arc)minute, and
 * (arc)second components may be separated by spaces, tabs, colons `:`, underscore `_`, or a
 * combination thereof. Additionally, the degree and minutes may be separated by the letter `d`,
 * and the minutes and seconds may be separated by `m` or a single quote `'`. The seconds may be
 * followed by `s` or a double quote `"`. Finally, the leading or trailing component may
 * additionally be a standalone upper-case letter 'N', 'E', 'S', or 'W' or the
 * words 'North', 'East', 'South', or 'West' (case insensitive), signifying a compass
 * direction.
 *
 * There is no enforcement on the range of angles that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range, such as +- 90
 * degrees N/S.
 *
 * For example, all of the lines below are valid specifications:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  -179 59' 59.999
 *  179:59:59.999S
 *  179:59:59.999 W
 *  179:59:59.999 West
 *  179_59_59.999__S
 *  179 59 S
 *  W 179 59 59
 *  North 179d 59m
 * </pre>
 *
 * At least the leading two components (degrees and arcminutes) are required. If the arcseconds
 * are ommitted, they will be assumed zero, i.e. `179:59` is the same as `179:59:00.000`.
 *
 * @param dms         String specifying degrees, minutes, and seconds, which correspond to
 *                    an angle. Angles in any range are permitted, but the minutes and
 *                    seconds must be &gt;=0 and &lt;60.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed time.
 * @return            [deg] Corresponding decimal angle value, or else NAN if there was
 *                    an error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 *
 * @sa novas_dms_degrees(), novas_parse_degrees(), novas_print_dms(), novas_parse_hms()
 */
double novas_parse_dms(const char *restrict dms, char **restrict tail) {
  static const char *fn = "novas_dms_degrees";

  int sign = 0, d = 0, m = 0, nv = 0, nu = 0, nc = 0;
  double s = 0.0;
  char *str, ss[40] = {'\0'};

  if(tail)
    *tail = (char *) dms;

  if(!dms) {
    novas_set_errno(EINVAL, fn, "input string is NULL");
    return NAN;
  }
  if(!dms[0]) {
    novas_set_errno(EINVAL, fn, "input string is empty");
    return NAN;
  }

  sign = parse_compass(dms, &nc);
  str = (char *) dms + nc;

  if(sscanf(str, "%d%*[:d _\t]%d%n%*[:m' _\t]%n%39[-+0-9.]", &d, &m, &nv, &nv, ss) < 2) {
    novas_set_errno(EINVAL, fn, "not in DMS format: '%s'", dms);
    return NAN;
  }

  if(m < 0 || m >= 60) {
    novas_set_errno(EINVAL, fn, "invalid minutes: got %d, expected 0-59", m);
    return NAN;
  }

  if(ss[0]) {
    char *end = ss;
    s = strtod(ss, &end);
    nv += (int) (end - ss);
  }

  if(s < 0.0 || s >= 60.0) {
    novas_set_errno(EINVAL, fn, "invalid seconds: got %f, expected [0.0:60.0)", s);
    return NAN;
  }

  s = abs(d) + (m / 60.0) + (s / 3600.0);

  if (d < 0)
    s = -s;

  if(sign < 0) {
    s = -s;
  }

  // Trailing seconds marker (if any)
  sscanf(&str[nv], "%*[ _\t]%*[s\"]%n", &nu);

  // Compass direction (if any)
  if(nc == 0 && parse_compass(&str[nv + nu], &nc) < 0)
    s = -s;

  if(tail)
    *tail = (char *) dms + nv + nu + nc;

  return s;
}

/**
 * Returns the decimal degrees for a DMS string specification. The degree, (arc)minute, and
 * (arc)second components may be separated by spaces, tabs, colons `:`, or a combination thereof.
 * Additionally, the degree and minutes may be separated by the letter `d`, and the minutes and
 * seconds may be separated by `m` or a single quote `'`. The seconds may be followed by 's' or
 * double quote `"`. Finally, the leading or trailing component may additionally be a
 * standalone upper-case letter 'N', 'E', 'S', or 'W' or the words 'North', 'East', 'South', or
 * 'West' (case insensitive), signifying a compass direction.
 *
 * There is no enforcement on the range of angles that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range, such as +- 90
 * degrees N/S.
 *
 * For example, all of the lines below are valid specifications:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  -179 59' 59.999
 *  179:59:59.999S
 *  179 59 59.999 W
 *  179 59 59.999 west
 *  179_59_59.999__S
 *  W 179 59 59
 *  North 179d 59m
 * </pre>
 *
 * At least the leading two components (degrees and arcminutes) are required. If the arcseconds
 * are ommitted, they will be assumed zero, i.e. `179:59` is the same as `179:59:00.000`.
 *
 * NOTES:
 * <ol>
 * <li> To see if the string was fully parsed when returning a valid (non-NAN) value, you can
 * check `errno`: it should be zero (0) if all non-whitespace characters have been parsed from the
 * input string, or else `EINVAL` if the parsed value used only the leading part of the
 * string.</li>
 * </ol>
 *
 * @param dms     String specifying degrees, minutes, and seconds, which correspond to
 *                an angle. Angles in any range are permitted, but the minutes and
 *                seconds must be &gt;=0 and &lt;60.
 * @return        [deg] Corresponding decimal angle value, or else NAN if there was
 *                an error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_degrees(), novas_parse_dms(), novas_hms_hours()
 */
double novas_dms_degrees(const char *restrict dms) {
  char *tail = (char *) dms;
  double deg = novas_parse_dms(dms, &tail);
  if(isnan(deg))
    return novas_trace_nan("novas_dms_degrees");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return deg;
}

/**
 * Parses an angle in degrees from a string that contains either a decimal degrees or else a
 * broken-down DMS representation.
 *
 * The decimal representation may be followed by a unit designator: "d", "dg", "deg", "degree",
 * or "degrees", which will be parsed case-insensitively also, if present.
 *
 * Both DMS and decimal values may start or end with a compass direction: such as an upper-case
 * letter `N`, `E`, `S`, or `W`, or else the case-insensitive words 'North', 'East', 'South' or
 * 'West'.
 *
 * There is no enforcement on the range of angles that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range, such as +- 90
 * degrees N/S.
 *
 * A few examples of angles that may be parsed:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  179 59' 59.999" S
 *  179 59 S
 *  -179.99999d
 *  -179.99999
 *  179.99999W
 *  179.99999 West
 *  179.99999 deg S
 *  W 179.99999d
 *  North 179d 59m
 *  east 179.99 degrees
 * </pre>
 *
 * @param str         The input string that specified an angle either as decimal degrees
 *                    or as a broken down DMS speficication. The decimal value may be
 *                    followed by the letter `d` immediately. And both the decimal and DMS
 *                    representation may be ended with a compass direction marker,
 *                    `N`, `E`, `S`, or `W`. See more in `novas_parse_dms()` on acceptable DMS
 *                    specifications.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed angle.
 * @return            [deg] The angle represented by the string, or else NAN if the
 *                    string could not be parsed into an angle value (errno will indicate
 *                    the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_degrees(), novas_parse_dms(), novas_parse_hours()
 */
double novas_parse_degrees(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_degrees";

  double deg;
  enum novas_debug_mode debug = novas_get_debug_mode();
  int sign, nc = 0;
  char *next, num[80] = {'\0'};

  if(tail)
    *tail = (char *) str;

  if(!str) {
    novas_set_errno(EINVAL, fn, "input string is NULL");
    return NAN;
  }

  novas_debug(NOVAS_DEBUG_OFF);
  deg = novas_parse_dms(str, tail);
  novas_debug(debug);

  if(!isnan(deg))
    return deg;

  sign = parse_compass(str, &nc);
  next = (char *) str + nc;

  while(*next && isspace(*next)) next++;

  if(sscanf(next, "%79[-+0-9.]", num) > 0) {
    char *end = num;
    int n;

    deg = strtod(num, &end);
    n = end - num;

    if(n > 0) {
      char unit[9] = {'\0'};
      int n1, nu = 0;

      // Check if exponential notation.
      if(toupper(next[n]) == 'E' && next[n+1] && !isspace(next[n+1]) && next[n+1] != '_') {
        int exp = strtol(&next[n+1], &end, 10);
        if(end > &next[n+1]) {
          deg *= pow(10.0, exp);
          n = end - next;
        }
      }

      // Skip underscores and white spaces
      for(n1 = n; next[n1] && (next[n1] == '_' || isspace(next[n1]));) n1++;

      // Skip over unit specification
      if(sscanf(&next[n1], "%8s%n", unit, &nu) > 0) {
        static const char *units[] = { "d", "dg", "deg", "degree", "degrees" , NULL};
        int i;

        // Terminate unit at punctuation
        for(i = 0; unit[i]; i++) if(unit[i] == '_' || ispunct(unit[i])) {
          unit[i] = '\0';
          nu = i;
          break;
        }

        // Check for match against recognised units.
        for(i = 0; units[i]; i++) if(strcasecmp(units[i], unit) == 0) {
          n = n1 + nu;
          break;
        }
      }

      if(nc == 0) {
        sign = parse_compass(&next[n], &nc);
        n += nc;
      }

      if(sign < 0)
        deg = -deg;

      if(tail)
        *tail = next + n;

      return deg;
    }
  }

  novas_set_errno(EINVAL, fn, "invalid angle specification: '%s'", str);
  return NAN;
}

/**
 * Parses a time or time-like angle from a string that contains either a decimal hours or else a
 * broken-down HMS representation.
 *
 * The decimal representation may be followed by a unit designator: "h", "hr", "hrs", "hour", or
 * "hours", which will be parsed case-insensitively also, if present.
 *
 * There is no enforcement on the range of hours that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range of 0-24h.
 *
 * A few examples of angles that may be parsed:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999s
 *  23h59'59.999
 *  23 59 59.999
 *  23.999999h
 *  23.999999 hours
 *  23.999999
 * </pre>
 *
 *
 * @param str         The input string that specified an angle either as decimal hours
 *                    or as a broken down HMS speficication. The decimal value may be immediately
 *                    followed by a letter 'h'. See more in `novas_parse_hms()` on acceptable HMS
 *                    input specifications.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed angle.
 * @return            [h] The time-like value represented by the string, or else NAN if the
 *                    string could not be parsed into a time-like value (errno will indicate
 *                    the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_hours(), novas_parse_hms(), novas_parse_degrees()
 */
double novas_parse_hours(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_hours";

  double h;
  enum novas_debug_mode debug = novas_get_debug_mode();
  int n = 0;

  if(tail)
    *tail = (char *) str;

  if(!str) {
    novas_set_errno(EINVAL, fn, "input string is NULL");
    return NAN;
  }

  novas_debug(NOVAS_DEBUG_OFF);
  h = novas_parse_hms(str, tail);
  novas_debug(debug);

  if(!isnan(h))
    return h;

  if(sscanf(str, "%lf%n", &h, &n) > 0) {
    char unit[7] = {'\0'};
    int n1, n2 = 0;

    // Skip underscores and white spaces
    for(n1 = n; str[n1] && (str[n1] == '_' || isspace(str[n1])); n1++);

    // Skip over unit specification
    if(sscanf(&str[n1], "%6s%n", unit, &n2) > 0) {
      static const char *units[] = { "h", "hr", "hrs", "hour", "hours" , NULL};
      int i;

      // Terminate unit at punctuation
      for(i = 0; unit[i]; i++) if(unit[i] == '_' || ispunct(unit[i])) {
        unit[i] = '\0';
        n2 = i;
        break;
      }

      // Check for match against recognised units.
      for(i = 0; units[i]; i++) if(strcasecmp(units[i], unit) == 0) {
        n = n1 + n2;
        break;
      }
    }

    if(tail)
      *tail += n;
  }
  else {
    novas_set_errno(EINVAL, fn, "invalid time specification: '%s'", str);
    return NAN;
  }


  return h;
}

/**
 * Returns an angle parsed from a string that contains either a decimal degrees or else a
 * broken-down DMS representation. See `novas_parse_degrees()` to see what string
 * representations may be used.
 *
 * To see if the string was fully parsed when returning a valid (non-NAN) value, you can check
 * `errno`: it should be zero (0) if all non-whitespace and punctuation characters have been
 * parsed from the input string, or else `EINVAL` if the parsed value used only the leading part
 * of the string.
 *
 * @param str     The input string that specified an angle either as decimal degrees
 *                or as a broken down DMS speficication. The decimal value may be immediately
 *                followed by a letter 'd'. See more in `novas_parse_degrees()` on acceptable
 *                input specifications.
 * @return        [deg] The angle represented by the string, or else NAN if the
 *                string could not be parsed into an angle value (errno will indicate
 *                the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_degrees(), novas_parse_dms(), novas_print_dms(), novas_str_hours()
 */
double novas_str_degrees(const char *restrict str) {
  char *tail = (char *) str;
  double deg = novas_parse_degrees(str, &tail);
  if(isnan(deg))
    return novas_trace_nan("novas_str_degrees");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return deg;
}

/**
 * Returns a time or time-like angleparsed  from a string that contains either a decimal hours
 * or else a broken-down HMS representation. See `novas_parse_hours()` to see what string
 * representations may be used.
 *
 * To check if the string was fully parsed when returning a valid (non-NAN) value you can check
 * `errno`: it should be zero (0) if all non-whitespace and punctuation characters have been
 * parsed from the input string, or else `EINVAL` if the parsed value used only the leading part
 * of the string.
 *
 * @param str     The input string that specified an angle either as decimal hours
 *                or as a broken down HMS speficication. The decimal value may be
 *                immediately followed by a letter 'h'. See more in `novas_parse_hours()`
 *                on acceptable input specifications.
 * @return        [h] The time-like value represented by the string, or else NAN if the
 *                string could not be parsed into a time-like value (errno will indicate
 *                the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_hours(), novas_parse_hms(), novas_print_hms(), novas_str_degrees()
 */
double novas_str_hours(const char *restrict str) {
  char *tail = (char *) str;
  double h = novas_parse_hours(str, &tail);
  if(isnan(h))
    return novas_trace_nan("novas_str_hours");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return h;
}

static int skip_white(const char *str, char **tail) {
  char *next = (char *) str;

  // Consume trailing 'white' spaces / punctuation
  for(; *next; next++)
    if(!isspace(*next) && *next != '_')
      break;

  *tail = next;
  return 0;
}

static int parse_zone(const char *str, char **tail) {
  char *next = (char *) str;

  *tail = next;

  if(*str == '+' || *str == '-') {
    static const char *fn = "parse_zone";

    // zone in {+|-}HH[:[MM]] format...
    int H = 0, M = 0;
    int sign = *(next++) == '-' ? -1 : 1;
    int colon = 0;

    if(isdigit(next[0]) && isdigit(next[1])) {
      H = 10 * (next[0] - '0') + (next[1] - '0');
      if(H >= 24)
        return novas_error(-1, EINVAL, fn, "invalid zone hours: %d, expected [0-23]", H);
      next += 2;
    }
    else
      return novas_error(-1, EINVAL, fn, "invalid time zone specification");

    if(*next == ':') {
      next++;
      colon = 1;
    }

    if(isdigit(next[0])) {
      if(!isdigit(next[1]))
        return novas_error(-1, EINVAL, fn, "invalid time zone specification");

      M = 10 * (next[0] - '0') + (next[1] - '0');
      if(M >= 60)
        return novas_error(-1, EINVAL, fn, "invalid zone minutes: %d, expected [0-60]", M);
      next += 2;
    }
    else if(colon)
      next--;

    *tail = next;
    return sign * (H * 3600 + M * 60); // zone time to UTC...
  }

  if(*str == 'Z' || *str == 'z')
    *tail = (char *) str + 1;

  return 0;
}

/**
 * Parses a calendar date/time string, expressed in the specified type of calendar, into a Julian
 * day (JD). The date must be composed of a full year (e.g. 2025), a month (numerical or name or
 * 3-letter abbreviation, e.g. "01", "1", "January", or "Jan"), and a day (e.g. "08" or "8"). The
 * components may be separated by dash `-`, underscore `_`, dot `.`,  slash '/', or spaces/tabs,
 * or any combination thereof. The components will be parsed in the specified order.
 *
 * The date may be followed by a time specification in HMS format, separated from the date by the
 * letter `T` or `t`, or spaces, comma `,`, or semicolon `;` or underscore '_', or a combination
 * thereof. Finally, the time may be followed by the letter `Z`, or `z` (for UTC) or else by a
 * {+/-}HH[:[MM]] time zone specification.
 *
 * For example, for `format` NOVAS_YMD, all of the following strings may specify the date:
 *
 * <pre>
 *  2025-01-26
 *  2025 January 26
 *  2025_Jan_26
 *  2025-01-26T19:33:08Z
 *  2025.01.26T19:33:08
 *  2025 1 26 19h33m28.113
 *  2025/1/26 19:33:28+02
 *  2025-01-26T19:33:28-0600
 *  2025 Jan 26 19:33:28+05:30
 * </pre>
 *
 * are all valid dates that can be parsed.
 *
 * If your date format cannot be parsed with this function, you may parse it with your own
 * function into year, month, day, and decimal hour-of-day components, and use julian_date() with
 * those.
 *
 * NOTES:
 * <ol>
 *  <li>B.C. dates are indicated with years &lt;=0 according to the astronomical
 * and ISO 8601 convention, i.e., X B.C. as (1-X), so 45 B.C. as -44.</li>
 * </oL>
 *
 * @param calendar    The type of calendar to use: NOVAS_ASTRONOMICAL_CALENDAR,
 *                    NOVAS_GREGORIAN_CALENDAR, or NOVAS_ROMAN_CALENDAR.
 * @param format      Expected order of date components: NOVAS_YMD, NOVAS_DMY, or NOVAS_MDY.
 * @param date        The date specification, possibly including time and timezone, in the
 *                    specified standard format.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed time.
 *
 * @return            [day] The Julian Day corresponding to the string date/time specification or
 *                    NAN if the string is NULL or if it does not specify a date/time in the
 *                    expected format.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_date(), novas_parse_iso_date(), novas_timescale_for_string(), novas_iso_timestamp()
 *     novas_jd_from_date()
 */
double novas_parse_date_format(enum novas_calendar_type calendar, enum novas_date_format format, const char *restrict date,
        char **restrict tail) {
  static const char *fn = "novas_parse_date";
  static const char md[13] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  int y = 0, m = 0, d = 0, n = 0, N = 0;
  double h = 0.0;
  char month[10] = {'\0'}, *next = (char *) date;

  if(tail)
    *tail = (char *) date;

  if(!date) {
    novas_set_errno(EINVAL, fn, "input string is NULL");
    return NAN;
  }
  if(!date[0]) {
    novas_set_errno(EINVAL, fn, "input string is empty");
    return NAN;
  }

  switch(format) {
    case NOVAS_YMD:
      N = sscanf(date, "%d" DATE_SEP MONTH_SPEC DATE_SEP "%d%n", &y, month, &d, &n);
      break;
    case NOVAS_DMY:
      N = sscanf(date, "%d" DATE_SEP MONTH_SPEC DATE_SEP "%d%n", &d, month, &y, &n);
      break;
    case NOVAS_MDY:
      N = sscanf(date, MONTH_SPEC DATE_SEP "%d" DATE_SEP "%d%n", month, &d, &y, &n);
      break;
    default:
      novas_set_errno(EINVAL, fn, "invalid date format: %d", format);
      return NAN;
  }

  if(N < 3) {
    novas_set_errno(EINVAL, fn, "invalid date: '%s'", date);
    return NAN;
  }

  if(sscanf(month, "%d", &m) == 1) {
    // Month as integer, check if in expected range
    if(m < 1 || m > 12) {
      novas_set_errno(EINVAL, fn, "invalid month: got %d, expected 1-12", m);
      return NAN;
    }
  }
  else {
    // Perhaps month as string...
    for(m = 1; m <= 12; m++) {
      static const char *monNames[13] = { NULL, "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

      if(strcasecmp(monNames[m], month) == 0)
        break;      // match full month name
      if(strncasecmp(monNames[m], month, 3) == 0)
        break;      // match abbreviated month name
    }
    if(m > 12) {
      // No match to month names...
      novas_set_errno(EINVAL, fn, "invalid month: %s", month);
      return NAN;
    }
  }

  // Check that day is valid in principle (for leap years)
  if(d < 1 || d > md[m]) {
    novas_set_errno(EINVAL, fn, "invalid day-of-month: got %d, expected 1-%d", d, md[m]);
    return NAN;
  }

  if(tail)
    *tail += n;

  skip_white(&date[n], &next);

  if(*next) {
    char *from = next;
    enum novas_debug_mode saved = novas_get_debug_mode();

    // Check if 'T' is used to separate time component, as in ISO timestamps.
    if(*next == 'T' || *next == 't')
      next++;

    // suppress debug messages while we parse time...
    novas_debug(NOVAS_DEBUG_OFF);

    // Try parse time
    h = novas_parse_hms(next, &next);

    // Restore prior debug state...
    errno = 0;
    novas_debug(saved);

    if(!isnan(h)) {
      int ds = parse_zone(next, &next);
      if(errno)
        return novas_trace_nan(fn);
      h -= ds / 3600.0;
    }
    else if(tail) {
      h = 0.0;
      next = from; // Time parsing unsuccessful, no extra characters consumed.
    }

    if(tail)
      *tail = next;
  }

  return novas_jd_from_date(calendar, y, m, d, h);
}

/**
 * Parses an astronomical date/time string into a Julian date specification.
 *
 * The date must be YMD-type with full year, followed the month (numerical or name or 3-letter
 * abbreviation), and the day. The components may be separated by dash `-`, underscore `_`, dot
 * `.`,  slash '/', or spaces/tabs, or any combination thereof. The date may be followed by a time
 * specification in HMS format, separated from the date by the letter `T` or `t`, or spaces, comma
 * `,`, or semicolon `;`, or underscore `_` or a combination thereof. Finally, the time may be
 * followed by the letter `Z`, or `z` (for UTC) or else {+/-}HH[:[MM]] time zone specification.
 *
 * For example:
 *
 * <pre>
 *  2025-01-26
 *  2025 January 26
 *  2025_Jan_26
 *  2025-01-26T19:33:08Z
 *  2025.01.26T19:33:08
 *  2025 1 26 19h33m28.113
 *  2025/1/26 19:33:28+02
 *  2025-01-26T19:33:28-0600
 *  2025 Jan 26 19:33:28+05:30
 * </pre>
 *
 * are all valid dates that can be parsed.
 *
 * NOTES:
 * <ol>
 * <li>This function assumes Gregorian dates after their introduction on 1582 October 15, and
 * Julian/Roman dates before that, as was the convention of the time. I.e., the day before of the
 * introduction of the Gregorian calendar reform is 1582 October 4. I.e., you should not use
 * this function with ISO 8601 timestamps containing dates prior to 1582 October 15 (for such
 * date you may use `novas_parse_iso_date()` instead).</li>
 *
 * <li>B.C. dates are indicated with years &lt;=0 according to the astronomical
 * and ISO 8601 convention, i.e., X B.C. as (1-X), so 45 B.C. as -44.</li>
 * </oL>
 *
 * @param date        The astronomical date specification, possibly including time and timezone,
 *                    in a standard format. The date is assumed to be in the astronomical calendar
 *                    of date, which differs from ISO 8601 timestamps for dates prior to the
 *                    Gregorian calendar reform of 1582 October 15 (otherwise, the two are
 *                    identical).
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed time.
 *
 * @return            [day] The Julian Day corresponding to the string date/time specification or
 *                    NAN if the string is NULL or if it does not specify a date/time in the
 *                    expected format.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_iso_date(), novas_parse_date_format(), novas_date(), novas_date_scale(),
 *     novas_timescale_for_string(), novas_iso_timestamp(), novas_timestamp()
 */
double novas_parse_date(const char *restrict date, char **restrict tail) {
  double jd = novas_parse_date_format(NOVAS_ASTRONOMICAL_CALENDAR, NOVAS_YMD, date, tail);
  if(isnan(jd))
    return novas_trace_nan("novas_parse_date");
  return jd;
}

/**
 * Parses an ISO 8601 timestamp, converting it to a Julian day. It is equivalent to
 * `novas_parse_date()` for dates after the Gregorian calendar reform of 1582. For earlier dates,
 * ISO timestamps continue to assume the Gregorian calendar (i.e. proleptic Gregorian dates),
 * whereas `novas_parse_timestamp()` will assume Roman/Julian dates, which were conventionally
 * used before the calendar reform.
 *
 * NOTES:
 * <ol>
 * <li>B.C. dates are indicated with years &lt;=0 according to the astronomical
 * and ISO 8601 convention, i.e., X B.C. as (1-X), so 45 B.C. as -44.</li>
 * </oL>
 *
 * @param date        The ISO 8601 date specification, possibly including time and timezone, in a
 *                    standard format.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed time.
 *
 * @return            [day] The Julian Day corresponding to the string date/time specification or
 *                    NAN if the string is NULL or if it does not specify a date/time in the
 *                    expected format.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_iso_timestamp(), novas_parse_date()
 */
double novas_parse_iso_date(const char *restrict date, char **restrict tail) {
  double jd = novas_parse_date_format(NOVAS_GREGORIAN_CALENDAR, NOVAS_YMD, date, tail);
  if(isnan(jd))
    return novas_trace_nan("novas_parse_iso_date");
  return jd;
}


/**
 * Parses the timescale from a string containing a standard abbreviation (case insensitive), and
 * returns the updated parse position after the timescale specification (if any). The following
 * timescale values are recognised: "UTC", "UT", "UT0", "UT1", "GMT", "TAI", "GPS", "TT", "ET",
 * "TCG", "TCB", "TDB".
 *
 * @param str         String specifying an astronomical timescale. Leading white spaces will be
                      skipped over.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed timescale specification.
 *
 * @return            The SuperNOVAS timescale constant (&lt;=0), or else -1 if the string was
 *                    NULL, empty, or could not be matched to a timescale value (errno will be set
 *                    to EINVAL also).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_timescale_for_string(), novas_set_str_time(), novas_print_timescale()
 */
enum novas_timescale novas_parse_timescale(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_timescale";

  enum novas_timescale scale = NOVAS_UTC;
  char s[4] = {'\0'};
  int n = 0;

  if(tail)
    *tail = (char *) str;

  if(!str)
    return (enum novas_timescale) novas_error(-1, EINVAL, fn, "input string is NULL");

  if(sscanf(str, "%3s%n", s, &n) == 1) {
    scale = novas_timescale_for_string(s);
    if(scale < 0)
      return (enum novas_timescale) novas_trace(fn, scale, 0);
  }

  if(tail)
    *tail += n;

  return scale;
}

/**
 * Returns the timescale constant for a string that denotes the timescale in with a standard
 * abbreviation (case insensitive). The following values are recognised: "UTC", "UT", "UT0",
 * "UT1", "GMT", "TAI", "GPS", "TT", "ET", "TCG", "TCB", and "TDB".
 *
 * @param str     String specifying an astronomical timescale
 * @return        The SuperNOVAS timescale constant (&lt;=0), or else -1 if the string was NULL,
 *                empty, or could not be matched to a timescale value (errno will be set to EINVAL
 *                also).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_timescale(), novas_set_str_time(), novas_print_timescale()
 */
enum novas_timescale novas_timescale_for_string(const char *restrict str) {
  static const char *fn = "novas_str_timescale";

  if(!str)
    return (enum novas_timescale) novas_error(-1, EINVAL, fn, "input string is NULL");

  if(!str[0])
    return (enum novas_timescale) novas_error(-1, EINVAL, fn, "input string is empty");

  if(strcasecmp("UTC", str) == 0 || strcasecmp("UT", str) == 0 || strcasecmp("UT0", str) == 0 || strcasecmp("GMT", str) == 0)
    return NOVAS_UTC;

  if(strcasecmp("UT1", str) == 0)
    return NOVAS_UT1;

  if(strcasecmp("TAI", str) == 0)
    return NOVAS_TAI;

  if(strcasecmp("GPS", str) == 0)
    return NOVAS_GPS;

  if(strcasecmp("TT", str) == 0 || strcasecmp("ET", str) == 0)
    return NOVAS_TT;

  if(strcasecmp("TCG", str) == 0)
    return NOVAS_TCG;

  if(strcasecmp("TCB", str) == 0)
    return NOVAS_TCB;

  if(strcasecmp("TDB", str) == 0)
    return NOVAS_TDB;

  return (enum novas_timescale) novas_error(-1, EINVAL, fn, "unknown timescale: %s", str);
}


#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif
