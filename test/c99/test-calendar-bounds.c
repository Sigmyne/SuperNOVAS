/**
 * @file
 * Tests calendar range checks and integer arithmetic at the input limits.
 */

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "novas.h"

static int check(const char *name, int is_valid)
{
    if(!is_valid) {
        fprintf(stderr, "FAIL: %s\n", name);
        return 1;
    }
    return 0;
}

/* Use floor division for negative years, not C integer truncation. */
static long long floor_div(long long value, long long divisor)
{
    return value / divisor - (value % divisor < 0);
}

/* Count days from 1 January, year 1. This does not use the library algorithm. */
static double reference_jd(enum novas_calendar_type calendar, int year, int month, int day, double hour)
{
    static const int days_before_month[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    long long previous_year = (long long) year - 1;
    long long days = 365 * previous_year + floor_div(previous_year, 4);
    int is_gregorian = calendar == NOVAS_GREGORIAN_CALENDAR;
    int is_leap;

    if(calendar == NOVAS_ASTRONOMICAL_CALENDAR) {
        is_gregorian = year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15)));
    }
    if(is_gregorian) {
        days += -floor_div(previous_year, 100) + floor_div(previous_year, 400);
    }
    is_leap = year % 4 == 0 && (!is_gregorian || year % 100 != 0 || year % 400 == 0);
    days += days_before_month[month - 1] + day - 1;
    if(month > 2 && is_leap) {
        days++;
    }
    return (double) (days + (is_gregorian ? 1721426LL : 1721424LL)) + (hour - 12.0) / 24.0;
}

static int test_reference(void)
{
    int n = 0;

    n += check("reference Gregorian year 1", reference_jd(NOVAS_GREGORIAN_CALENDAR, 1, 1, 1, 12.0) == 1721426.0);
    n += check("reference Julian year 1", reference_jd(NOVAS_ROMAN_CALENDAR, 1, 1, 1, 12.0) == 1721424.0);
    n += check("reference Gregorian year 0", reference_jd(NOVAS_GREGORIAN_CALENDAR, 0, 1, 1, 12.0) == 1721060.0);
    n += check("reference J2000", reference_jd(NOVAS_GREGORIAN_CALENDAR, 2000, 1, 1, 12.0) == NOVAS_JD_J2000);
    n += check("reference Gregorian leap day", reference_jd(NOVAS_GREGORIAN_CALENDAR, 2000, 3, 1, 12.0) == 2451605.0);
    n += check("reference Julian leap day", reference_jd(NOVAS_ROMAN_CALENDAR, 2000, 3, 1, 12.0) == 2451618.0);
    n += check("reference before reform", reference_jd(NOVAS_ASTRONOMICAL_CALENDAR, 1582, 10, 4, 0.0) == 2299159.5);
    n += check("reference after reform", reference_jd(NOVAS_ASTRONOMICAL_CALENDAR, 1582, 10, 15, 0.0) == 2299160.5);
    return n;
}

static int check_accepted(double jd, enum novas_calendar_type calendar)
{
    static const int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int year = 0;
    int month = 0;
    int day = 0;
    double hour = NAN;
    int n = 0;
    int status = novas_jd_to_date(jd, calendar, &year, &month, &day, &hour);
    int is_gregorian = calendar == NOVAS_GREGORIAN_CALENDAR;
    int max_day;

    if(status != 0) {
        fprintf(stderr, "FAIL: accepted JD %.17g, calendar %d: status %d\n", jd, calendar, status);
        return 1;
    }
    if(calendar == NOVAS_ASTRONOMICAL_CALENDAR) {
        is_gregorian = jd >= NOVAS_JD_START_GREGORIAN;
    }
    if(month < 1 || month > 12) {
        fprintf(stderr, "FAIL: JD %.17g, calendar %d: date %d/%d/%d\n", jd, calendar, year, month, day);
        return 1;
    }
    max_day = month_days[month - 1];
    if(month == 2 && year % 4 == 0 && (!is_gregorian || year % 100 != 0 || year % 400 == 0)) {
        max_day++;
    }
    n += check("accepted day is valid for its month", day >= 1 && day <= max_day);
    n += check("accepted hour is in [0, 24)", hour >= 0.0 && hour < 24.0);
    if(n == 0) {
        /* One representable step allows rounding at large Julian dates. */
        double tolerance = nextafter(jd, DBL_MAX) - jd;
        double expected = reference_jd(calendar, year, month, day, hour);
        double actual = novas_jd_from_date(calendar, year, month, day, hour);
        n += check("accepted date matches independent day count", fabs(expected - jd) <= tolerance);
        n += check("accepted date round trip", fabs(actual - jd) <= tolerance);
    }
    if(n != 0) {
        fprintf(stderr, "  JD %.17g, calendar %d: %d/%d/%d, hour %.17g\n", jd, calendar, year, month, day, hour);
    }
    return n;
}

static int check_rejected(double jd, enum novas_calendar_type calendar, int expected_errno)
{
    int year = 7;
    int month = 7;
    int day = 7;
    double hour = 7.0;
    int status;
    int saved_errno;
    int n = 0;

    errno = 0;
    status = novas_jd_to_date(jd, calendar, &year, &month, &day, &hour);
    saved_errno = errno;
    n += check("rejected date returns -1", status == -1);
    n += check("rejected date sets errno", saved_errno == expected_errno);
    if(n != 0) {
        fprintf(stderr, "  JD %.17g, calendar %d: status %d, errno %d (expected %d)\n",
                jd, calendar, status, saved_errno, expected_errno);
    }
    return n;
}

static int test_bounds(void)
{
    /* These are the proposed arithmetic limits, not a reference date algorithm. */
    long long min_day = 1794108LL + (INT_MIN - 200LL) * 146097LL / 400;
    long long max_day = 1794108LL + (INT_MAX - 200LL + 1) * 146097LL / 400 - 1;
    double lower = (double) min_day - 0.5;
    double upper = (double) max_day + 0.5;
    enum novas_calendar_type calendars[] = {
        NOVAS_ROMAN_CALENDAR, NOVAS_ASTRONOMICAL_CALENDAR, NOVAS_GREGORIAN_CALENDAR
    };
    size_t i;
    int n = 0;

    for(i = 0; i < sizeof(calendars) / sizeof(calendars[0]); i++) {
        enum novas_calendar_type calendar = calendars[i];
        n += check_accepted((double) min_day, calendar);
        n += check_accepted((double) max_day, calendar);
        n += check_accepted(lower, calendar);
        n += check_accepted(nextafter(lower, DBL_MAX), calendar);
        n += check_accepted(nextafter(upper, -DBL_MAX), calendar);
        n += check_rejected(nextafter(lower, -DBL_MAX), calendar, ERANGE);
        n += check_rejected(upper, calendar, ERANGE);
        n += check_rejected(nextafter(upper, DBL_MAX), calendar, ERANGE);
        n += check_rejected(NAN, calendar, ERANGE);
        n += check_rejected(DBL_MAX, calendar, ERANGE);
        n += check_rejected(-DBL_MAX, calendar, ERANGE);
#ifdef INFINITY
        n += check_rejected(INFINITY, calendar, ERANGE);
        n += check_rejected(-INFINITY, calendar, ERANGE);
#endif
    }
    n += check_rejected(NOVAS_JD_J2000, (enum novas_calendar_type) -2, EINVAL);
    n += check_rejected(NOVAS_JD_J2000, (enum novas_calendar_type) 2, EINVAL);
    return n;
}

static int test_extreme_years(void)
{
    int years[] = {INT_MIN, INT_MAX};
    int dates[][2] = {{1, 1}, {3, 1}, {12, 31}};
    enum novas_calendar_type calendars[] = {
        NOVAS_ROMAN_CALENDAR, NOVAS_ASTRONOMICAL_CALENDAR, NOVAS_GREGORIAN_CALENDAR
    };
    size_t i;
    size_t j;
    size_t k;
    int n = 0;

    for(i = 0; i < sizeof(calendars) / sizeof(calendars[0]); i++) {
        for(j = 0; j < sizeof(years) / sizeof(years[0]); j++) {
            for(k = 0; k < sizeof(dates) / sizeof(dates[0]); k++) {
                double expected = reference_jd(calendars[i], years[j], dates[k][0], dates[k][1], 12.0);
                double actual = novas_jd_from_date(calendars[i], years[j], dates[k][0], dates[k][1], 12.0);
                if(actual != expected) {
                    fprintf(stderr, "FAIL: calendar %d, %d/%d/%d: JD %.17g, expected %.17g\n",
                            calendars[i], years[j], dates[k][0], dates[k][1], actual, expected);
                    n++;
                }
            }
        }
    }
    return n;
}

static int test_invalid_month(void)
{
    /* Run this case separately so a sanitizer abort does not hide other failures. */
    volatile int month = INT_MIN;
    double jd;
    int saved_errno;
    int n = 0;

    errno = 0;
    jd = novas_jd_from_date(NOVAS_GREGORIAN_CALENDAR, 2000, month, 1, 12.0);
    saved_errno = errno;
    n += check("INT_MIN month returns NaN", isnan(jd));
    n += check("INT_MIN month sets EINVAL", saved_errno == EINVAL);
    return n;
}

int main(int argc, char **argv)
{
    int n;

    if(argc == 2 && strcmp(argv[1], "invalid-month") == 0) {
        n = test_invalid_month();
    } else {
        n = test_reference();
        if(n == 0) {
            n += test_bounds();
            n += test_extreme_years();
        }
    }
    return n == 0 ? 0 : 1;
}
