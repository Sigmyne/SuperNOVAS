/**
 * @file
 *
 * @date Created  on Apr 18, 2026
 * @author Attila Kovacs
 */

#include <errno.h>
#include <curl/curl.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

#define LEAP_FILENAME           "leap-seconds.list"
#define LEAP_URL                "https://hpiers.obspm.fr/iers/bul/bulc/ntp/" LEAP_FILENAME

#define UNIX_SECONDS_0UTC_1JAN2000  946684800L   ///< [s] UNIX time at J2000.0

/**
 * Earth Orienation Parameters (EOP), such as obtained from IERS.
 *
 * @since 1.7
 * @author Attila Kovacs
 */
typedef struct {
  double jd;          ///< [day] Julian day (in any time measure)
  int leap;           ///< [s] Leap seconds, that is the TAI - UTC time difference.
  double dut1;        ///< [s] UT1 - UTC time difference
  double xp;          ///< [arcsec] Polar offset in ITRF _x_ direction.
  double yp;          ///< [arcsec] Polar offset in ITRF _y_ direction.
} novas_eop;


typedef struct {
  char *url;
  int head_bytes;
  int line_len;
  double jd_start;
  double jd_step;
} iers_data_file;

typedef struct iers_leap_entry {
  int unix_start;
  int unix_end;
  int leap;
  struct iers_leap_entry *next;
} iers_leap_entry;


static iers_leap_entry *leaps;
static time_t leap_expiration;

// 1973.01.02 to +365 days, no head
static iers_data_file rapid = {
        "https://datacenter.iers.org/data/latestVersion/finals.all.iau2000.txt",
        0, 188, 0.0, 1.0
};

// 1962 -- now
static iers_data_file medium = {
        "https://datacenter.iers.org/data/latestVersion/EOP_20u24_C04_one_file_1962-now.txt",
        729, 219, NOVAS_JD_MJD0 + 37665, 1.0
};

// 1890 -- now (0.05 year)
static iers_data_file old = {
        "https://datacenter.iers.org/data/latestVersion/EOP_C01_IAU2000_1846-now.txt",
        1975, 312, NOVAS_JD_MJD0 + 11367.380, 0.05 * NOVAS_TROPICAL_YEAR_DAYS
};


// 1846 -- 1890 (0.1 year)
static iers_data_file very_old = {
        "https://datacenter.iers.org/data/latestVersion/EOP_C01_IAU2000_1846-now.txt",
        1975, 312, NOVAS_JD_MJD0 - 4703.268, 0.1 * NOVAS_TROPICAL_YEAR_DAYS
};

typedef struct {
  char *buf;
  size_t capacity;
  size_t size;
} download_buffer;


static size_t write_to_buffer(char *ptr, size_t size, size_t nmemb, void *userdata) {
  download_buffer *data = (download_buffer *) userdata;

  size_t n = size * nmemb;
  if(data->size + n >= data->capacity)
    n = data->capacity - data->size - 1;

  memcpy(data->buf, ptr, n);
  data->size += n;
  data->buf[data->size] = '\0';

  return n;
}

static void destroy_leap_list(iers_leap_entry *list) {
  while(list) {
    iers_leap_entry *e = list;
    list = e->next;
    free(e);
  }
}

// TODO use mutex
static iers_leap_entry *load_leaps_async(long long *exp) {
  static const char *fn = "load_leaps_async()";

  CURL *curl = NULL;
  CURLcode res;
  char str[32768];
  download_buffer data = { str, sizeof(str), 0 };
  size_t offset = 0;
  iers_leap_entry *list = NULL;

  curl = curl_easy_init();
  if (!curl) {
    dprintf("error: download() curl init failed.");
    return -1;
  }

  curl_easy_setopt(curl, CURLOPT_URL, LEAP_URL);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

  res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if(res)
    return novas_error(-1, EAGAIN, fn, "curl error %d", res);

  // Parse leap-seconds.list data
  while(offset < data.size) {
    char line[80] = {'\0'};
    size_t from = offset;
    int i;

    // Read one line (up to 80-chars)
    for(i = 0; i < 80; i++) {
      char c = str[offset++];
      if(c == '\0' || c == '\n')
        break;
    }

    // Process expiration timestamp
    if(str[from] == '#') {
      if(str[from + 1] == '@') {
        if(sscanf(str[from + 1], "%ld", exp) < 1)
          return novas_error(-1, errno, fn, "could not parse leap-seconds.list expiration time.");
      }
    }
    else {
      // Add leap entry to list
      iers_leap_entry *e = (iers_leap_entry *) calloc(1, sizeof(iers_leap_entry));
      long long start = 0LL;

      if(!e) {
        destroy_leap_list(list);
        novas_set_errno(errno, fn, "leap entry alloc error: %s", strerror(errno));
        return NULL;
      }

      // Parse start time and leap seconds value
      if(sscanf(str[from], "%lld %d", &start, &e->leap) < 2) {
        destroy_leap_list(list);
        novas_set_errno(errno, fn, "invalid leap-seconds.list entry");
        return NULL;
      }

      // Prepend to head of list.
      e->unix_start = (time_t) start;
      e->unix_end = (time_t) *exp;
      e->next = list;
      if(e->next)
        e->next->unix_end = e->unix_start;
      list = e;
    }
  }

  return list;
}

static int novas_lookup_leap(time_t t) {
  static const char *fn = "novas_lookup_leap";

  iers_leap_entry *e;
  struct tm tm = {};
  char str[40] = {'\0'};

  if(!leaps) {
    leaps = load_leaps_async();
  }
  else if(time(NULL) > leap_expiration) {
    iers_leap_entry *update = load_leaps_async();
    iers_leap_entry *old;

    if(!update)
      return novas_error(-1, errno, fn, "Could not update leap seconds list");

    old = leaps;
    leaps = update;
    destroy_leap_list(old);
  }

  if(!leaps)
    return novas_error(-1, errno, fn, "Could not obtain current leap seconds list");

  if(t > leaps->unix_end) {
    gmtime_r(t, &tm);
    strftime(str, sizeof(str), "%c", &tm);
    return novas_error(-1, ERANGE, fn, "Time %s is beyond the leap seconds coverage range", str);
  }

  for(e = leaps; e != NULL; e = e->next)
    if(t >= e->unix_start && t < e->unix_end)
      return e->leap;

  gmtime_r(t, &tm);
  strftime(str, sizeof(str), "%c", &tm);
  return novas_error(-1, ERANGE, fn, "Time %s is before leap second data", str);
}

static int novas_fetch_eop_chunk(const char *restrict url, long offset, int len, char *restrict out) {
  static const char *fn = "novas_fetch_eop_chunk";

  CURL *curl = NULL;
  CURLcode res;
  download_buffer data = { out, len, 0 };
  char range[40] = {'\0'};

  curl = curl_easy_init();
  if (!curl)
    return novas_error(-1, EAGAIN, fn, "curl error %d", res);

  snprintf(range, sizeof(range), "%ld-%ld", offset, (offset + len - 1));

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_RANGE, range);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

  res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if(res)
    return novas_error(-1, EAGAIN, fn, "curl error %d", res);

  return res;
}

static int eop_parse_line(iers_data_file *restrict file, int line, char *str, novas_eop *restrict eop) {
  char *from = str[line * file->line_len];
  errno = 0;

  eop->xp = eop->yp = eop->dut1 = NAN;

  if(file == rapid) {
    eop->xp = strtod(from + 18);
    eop->yp = strtod(from + 37);
    eop->dut1 = strtod(from + 58);
  }
  else if(file == medium) {
    eop->xp = strtod(from + 26);
    eop->yp = strtod(from + 38);
    eop->dut1 = strtod(from + 50);
  }
  else if(file == old) {
    eop->xp = strtod(from + 12);
    eop->yp = strtod(from + 22);
    eop->dut1 = strtod(from + 32); // UT1 - TAI
    if(eop->dut1 == 99.99)
      eop->dut1 = NAN;
    else
      eop->dut1 += eop->leap;
  }

  if(errno)
    return novas_error(-1, errno, "eop_parse_line", "parse error: %s", strerror(errno));

  return 0;
}

static int novas_fetch_from_file(iers_data_file *restrict file, double jd, novas_eop restrict *eop, int n) {
  static const char *fn = "novas_fetch_from_file";

  long offset;
  char lines[1024] = {'\0'};
  int i;

  offset = file->head_bytes + file->line_len * (jd - file->jd_start) / file->jd_step;
  prop_error(fn, novas_fetch_eop_chunk(file, offset, 2 * file->line_len, lines), 0);

  for(i = 0; i < n; i++) {
    eop[i].leap =  novas_lookup_leap(jd + i * file->jd_step);;
    prop_error(fn, eop_parse_line(file, i, lines, &eop[i]), 0);
    eop->jd = file->jd_start + offset * file->jd_step;
  }

  return 0;
}

static int novas_fetch_eop_array(double jd, novas_eop *restrict eop, int n) {
  static const char *fn = "novas_fetch_eop_array";

  novas_timespec ts = {};
  long mjd = (long) floor(jd - NOVAS_JD_MJD0);
  long mjd_now;
  int m = n >> 2;

  novas_set_current_time(0, 0.0, &ts);
  mjd_now = (long) floor(novas_get_time(&ts, NOVAS_TAI) - NOVAS_JD_MJD0);

  if(mjd >= mjd_now - 30 - n  && mjd <= mjd_now + 365 - n) {
    // Within last month, and up to a year ahead...
    prop_error(fn, novas_fetch_from_file(rapid, jd - m * rapid.jd_step, eop, n), 0);
  }
  else if(jd >= medium.jd_start + m * medium.jd_step) {
    prop_error(fn, novas_fetch_from_file(medium, jd - m * medium.jd_step, eop, n), 0);
  }
  else if(jd >= old.jd_start + m * old.jd_step) {
    prop_error(fn, novas_fetch_from_file(old, jd - m * old.jd_step, eop, n), 0);
  }
  else if(jd >= very_old.jd_start + m * very_old.jd_step) {
    prop_error(fn, novas_fetch_from_file(very_old, jd - m * very_old.jd_step, eop, n), 0);
  }
  else {
    memset(eop, 0, sizeof(novas_eop));
    return novas_error(-1, ERANGE, fn, "JD %.3g is outside of IERS EOP data range", jd);
  }

  return 0;
}

static double spline_coeff(double dx) {
  dx = fabs(dx);
  return dx > 1.0 ? ((-0.5 * dx + 2.5) * dx - 4.0) * dx + 2.0 : (1.5 * dx - 2.5) * dx * dx + 1.0;
}

static int novas_eop_spline_interp(double jd, const novas_eop *restrict array, novas_eop *restrict eop) {
  double dx = (jd - array[1].jd) / (array[2].jd - array[1].jd);
  time_t t = UNIX_SECONDS_0UTC_1JAN2000 + (jd - NOVAS_JD_J2000) * 86400;

  int i = 0;

  errno = 0;

  memset(eop, 0, sizeof(novas_eop));

  eop->leap = array[1].leap;
  eop->jd = jd;

  for(i = 0; i < 4; i++) {
    const novas_eop *e = &array[i];
    double w = spline_coeff(i - dx);
    eop->xp += w * e->xp;
    eop->yp += w * e->yp;
    eop->dut1 += w * (e->dut1 - e->leap + eop->leap); // (spline UT1 - TAI) -> dUT1
  }

  if(errno)
    return novas_trace("novas_eop_spline_interp", -1, 0);

  return 0;
}

/**
 * Obtains interpolated Earth Orientation Parameter data from IERS. For dates within the past
 * month or prediction for the next year, the rapid service data is used (`finals.all`), otherwise
 * for dates after 1962 the C04 series is used, or for dates all the way back to 1846, the C01
 * series is used. All of the mentioned data are retrieved relative to the IAU2000
 * precession-nutation model.
 *
 * NOTES:
 *
 *  1. You will typically need an internet connection and the IERS server must be online and
 *     accessible (at least for the initial call). You should always check the return status
 *     of this call.
 *
 *  2. Obtaining values from IERS can have arbitrary latencies, and can impact performance
 *     severely. For performance critical applications, you should consider specifying the EOP
 *     values more directly, e.g. from a local file instead.
 *
 *  3. This function caches EOP data from the last online query. As such repeated calls within the
 *     same data bracket (typically the same MJD day) will return fast and will reuse the last
 *     data obtained from the IERS.
 *
 *  4. This function assumes that the files served from IERS remain accessible and their format
 *     does not change over time.
 *
 *  5. This function uses cubic spline interpolation of the published data points.
 *
 *  6. The returned EOP values are assumed to be ITRF 2020 values, which is appropriate
 *     at the time this __SuperNOVAS__ module was written or last updated. If IERS later publishes
 *     data in some other future ITRF realization, this module may need to be updated, accordingly.
 *     However, the ITRF realization is unlikely to matter significantly.
 *
 * @param jd          Julian Date (in any timescale, with a preference for UTC)
 * @param[out] eop    Output EOP data structure to populate
 * @return            0 if successful or else -1 (errno will indicate the type of error).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_make_frame(), novas_set_time(), @ref earth
 */
int novas_fetch_eop(double jd, novas_eop *eop) {
  static const char *fn = "novas_fetch_eop";

  THREAD_LOCAL static novas_eop array[4];
  THREAD_LOCAL static double jd_from, jd_to = -1.0;

  if(!eop)
    return novas_error(-1, EINVAL, fn, "output eop is NULL");

  if(jd < jd_from || jd > jd_to) {
    prop_error(fn, novas_fetch_eop_array(jd, array, 4), 0);
    jd_from = array[1].jd;
    jd_to = array[2].jd;
  }

  return novas_eop_spline_interp(jd, array, eop);
}
