/**
 * @file
 *
 * @date Created  on Apr 18, 2026
 * @author Attila Kovacs
 *
 *  Functions to obtain and manage Earth Orientation data from the International Earth Rotation
 *  and Reference Systems Service (IERS) via HTTPS.
 */

#include <stdlib.h>   // atexit()
#include <string.h>
#include <time.h>
#include <errno.h>
#include <curl/curl.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

/// \cond PRIVATE
#define LEAP_FILENAME           "leap-seconds.list"
#define LEAP_URL                "https://hpiers.obspm.fr/iers/bul/bulc/ntp/" LEAP_FILENAME

#define UNIX_SECONDS_0UTC_1JAN2000  946684800L    ///< [s] UNIX time at J2000.0
#define NTP_UNIX_0                  2208988800LL  ///< [s] NTP timestamp of UNIX epoch (1970 Jan 1)
#define IERS_CONNECT_TIMEOUT_MS     3000L         ///< [ms] connection timeout for IERS servers

/**
 * IERS data file structural description.
 */
typedef struct {
  CURL *curl;       ///< CURL handle (reused)
  char *url;        ///< URL of EOP data file at IERS
  int head_bytes;   ///< [bytes] Header bytes before regular table row data begins
  int line_len;     ///< [byres] Length of data rows, including line feed
  double jd_start;  ///< [day] Julian date of first entry
  double jd_step;   ///< [day] interval between consecutive entries
  short ijd;        ///< column index from which to parse Julian Date
  short ixp;        ///< column index from which to parse _x_<sub>p</sub>
  short ixpe;       ///< column index from which to parse _x_<sub>p</sub> error
  short iyp;        ///< column index from which to parse _y_<sub>p</sub>
  short iype;       ///< column index from which to parse _y_<sub>p</sub> error
  short idut;       ///< column index from which to parse UT1 time difference
  short idute;      ///< column index from which to parse UT1 time error
  short ilod;       ///< column index from which to parse length of day (LOD) differential.
  short ilode;      ///< column index from which to parse LOD error
} iers_data_file;

/**
 * A individual leap seconds entry in a linked list of leap seconds
 *
 */
typedef struct iers_leap_entry {
  int unix_start;   ///< [day] Julian date leap was introduced
  int unix_end;     ///< [day] Julian date to which leap is valid
  int leap;         ///< [s] Leap seconds (TAI - UTC time difference)
  struct iers_leap_entry *next;   ///< Link to the next leap entry in list.
} iers_leap_entry;

/**
 * A simple data buffer for downloading data chunks from IERS
 */
typedef struct {
  char *buf;          ///< Allocated byte buffer
  size_t capacity;    ///< Capacity of byte buffer
  size_t size;        ///< Number of bytes filled
} download_buffer;

static iers_leap_entry *leaps;    ///< Leap seconds list
static time_t leap_expiration;    ///< UNIX time at which leap seconds list expires.

// 1973.01.02 to +365 days, no head
static iers_data_file finals = { NULL,
        "https://datacenter.iers.org/data/latestVersion/finals.all.iau2000.txt",
        0, 188, NOVAS_JD_MJD0 + 41684.0, 1.0,
        6, 17, 27, 36, 46, 58, 68, 78, 86
};

// 1962 -- now
static iers_data_file medium = { NULL,
        "https://datacenter.iers.org/data/latestVersion/EOP_20u24_C04_one_file_1962-now.txt",
        729, 219, NOVAS_JD_MJD0 + 37665, 1.0,
        16, 26, 122, 38, 134, 50, 146, 111, 206
};

// 1890 -- now (0.05 year)
static iers_data_file old = { NULL,
        "https://datacenter.iers.org/data/latestVersion/EOP_C01_IAU2000_1846-now.txt",
        139255, 312, NOVAS_JD_MJD0 + 11367.380, 0.05 * NOVAS_TROPICAL_YEAR_DAYS,
        0, 12, 71, 22, 83, 32, 93, 226, 281
};


// 1846 -- 1890 (0.1 year)
static iers_data_file very_old = { NULL,
        "https://datacenter.iers.org/data/latestVersion/EOP_C01_IAU2000_1846-now.txt",
        1975, 312, NOVAS_JD_MJD0 - 4703.268, 0.1 * NOVAS_TROPICAL_YEAR_DAYS
};

/// \endcond


static size_t write_to_buffer(char *ptr, size_t size, size_t nmemb, void *userdata) {
  download_buffer *data = (download_buffer *) userdata;

  size_t n = size * nmemb;
  if(data->size + n >= data->capacity) {
    novas_set_errno(ERANGE, "write_to_buffer", "truncating buffer %lld -> %lld", (long long) (data->size + n), (long long) data->capacity - 1);
    n = data->capacity - data->size - 1;
  }

  memcpy(&data->buf[data->size], ptr, n);
  data->size += n;
  data->buf[data->size] = '\0'; // always string terminate

  return n;
}

static CURL *init_curl() {
  CURL *curl = curl_easy_init();
  if (!curl) {
    novas_set_errno(errno, "init_curl", "curl init failed.");
    return NULL;
  }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);

  // Latency optimization
  curl_easy_setopt(curl, CURLOPT_TCP_FASTOPEN, 1L);
  curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, IERS_CONNECT_TIMEOUT_MS);

  return curl;
}

static void destroy_leap_list(iers_leap_entry *list) {
  while(list) {
    iers_leap_entry *e = list;
    list = e->next;
    free(e);
  }
}


// TODO use mutex
static iers_leap_entry *load_leaps_async(long long *expiration) {
  static const char *fn = "load_leaps_async()";

  CURL *curl = NULL;
  CURLcode res;
  char str[32768];
  download_buffer data = { str, sizeof(str), 0 };
  size_t offset = 0;
  iers_leap_entry *list = NULL;

  curl = init_curl();
  if (!curl) {
    novas_trace_invalid(fn);
    return NULL;
  }

  curl_easy_setopt(curl, CURLOPT_URL, LEAP_URL);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

  res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if(res) {
    novas_set_errno(EAGAIN, fn, "curl error %d", res);
    return NULL;
  }

  // Parse leap-seconds.list data
  while(offset < data.size) {
    size_t from = offset;

    // Read one line (up to 80-chars)
    for(;;) {
      char c = str[offset++];
      if(c == '\0' || c == '\n')
        break;
    }

    // Process expiration timestamp
    if(str[from] == '#') {
      if(str[from + 1] == '@') {
        if(sscanf(&str[from + 2], "%lld", expiration) < 1) {
          novas_set_errno(errno, fn, "could not parse leap-seconds.list expiration time.");
          return NULL;
        }
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
      if(sscanf(&str[from], "%lld %d", &start, &e->leap) < 2) {
        char line[80] = {'\0'};
        memcpy(line, &str[from], sizeof(line) - 1);
        destroy_leap_list(list);
        novas_set_errno(errno, fn, "invalid leap-seconds.list entry: %s", line);
        return NULL;
      }

      // Prepend to head of list.
      e->unix_start = (time_t) start;
      e->unix_end = (time_t) (*expiration - NTP_UNIX_0);
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
  long long expiration = 0LL;

  if(!leaps) {
    leaps = load_leaps_async(&expiration);
    if(!leaps)
      return novas_error(-1, errno, fn, "Could not obtain leap seconds list");
    leap_expiration = (time_t) expiration;
  }
  else if(time(NULL) > leap_expiration) {
    iers_leap_entry *update = load_leaps_async(&expiration);
    iers_leap_entry *old;

    if(!update)
      return novas_error(-1, errno, fn, "Could not update leap seconds list");

    old = leaps;
    leaps = update;
    leap_expiration = (time_t) expiration;

    destroy_leap_list(old);
  }

  if(!leaps)
    return novas_error(-1, errno, fn, "Could not obtain current leap seconds list");

  if(t > leaps->unix_end) {
    gmtime_r(&t, &tm);
    strftime(str, sizeof(str), "%c", &tm);
    return novas_error(-1, ERANGE, fn, "Time %s is beyond the leap seconds coverage range", str);
  }

  for(e = leaps; e != NULL; e = e->next)
    if(t >= e->unix_start && t < e->unix_end)
      return e->leap;

  gmtime_r(&t, &tm);
  strftime(str, sizeof(str), "%c", &tm);
  return 0;
}

static int novas_fetch_eop_chunk(CURL **restrict pCurl, const char *restrict url, long offset, int len, download_buffer *data) {
  static const char *fn = "novas_fetch_eop_chunk";

  CURL *curl;
  CURLcode res;
  char range[40] = {'\0'};

  if(!*pCurl)
    *pCurl = init_curl();


  curl = *pCurl;
  if (!curl)
    return novas_trace(fn, -1, 0);

  snprintf(range, sizeof(range), "%ld-%ld", offset, (offset + len - 1));

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
  curl_easy_setopt(curl, CURLOPT_RANGE, range);

  res = curl_easy_perform(curl);
  //curl_easy_cleanup(curl);

  if(res)
    return novas_error(-1, EAGAIN, fn, "curl error %d", res);

  return res;
}

static float read_float(const char *str) {
  char *tail = NULL;
  float f = (float) strtod(str, &tail);
  return (tail == str) ? NAN : f;
}

static int eop_parse_line(iers_data_file *restrict file, int line, char *str, novas_eop *restrict eop) {
  char *from = &str[line * file->line_len];
  char end;
  errno = 0;

  eop->xp = eop->yp = eop->dut1 = NAN;

  end = from[file->line_len-1];
  if(end != '\n' && end != '\0')
    return novas_error(-1, EBADMSG, "eop_parse_line", "Corrupted entry or unexpected format");

  eop->jd = NOVAS_JD_MJD0 + strtod(from + file->ijd, NULL);

  eop->xp = read_float(from + file->ixp);
  eop->yp = read_float(from + file->iyp);
  eop->dut1 = read_float(from + file->idut);
  eop->lod = read_float(from + file->ilod);

  eop->xp_err = read_float(from + file->ixpe);
  eop->yp_err = read_float(from + file->iype);
  eop->dut1_err = read_float(from + file->idute);
  eop->lod_err = read_float(from + file->ilode);

  if(file == &finals) {
    eop->lod *= 1e-3;       ///< finals LOD is milliseconds.
    eop->lod_err *= 1e-3;
  }
  else if(file == &old || file == &very_old) {
    if(fabs(eop->dut1 - 99.99) < 1e-5)
      eop->dut1 = NAN;
    else
      eop->dut1 += eop->leap;

    if(fabs(eop->dut1_err - 99.99) < 1e-5)
          eop->dut1_err = NAN;

    if(fabs(eop->lod - 99.99) < 1e-5)
      eop->lod = NAN;

    if(fabs(eop->lod_err - 99.99) < 1e-5)
      eop->lod_err = NAN;
  }

  if(errno)
    return novas_error(-1, errno, "eop_parse_line", "parse error: %s", strerror(errno));

  return 0;
}

static int novas_fetch_from_file(iers_data_file *restrict file, double jd, novas_eop *restrict eop, int n) {
  static const char *fn = "novas_fetch_from_file";

  long offset;
  char lines[2048] = {'\0'};
  download_buffer data = { lines, sizeof(lines), 0 };
  int i;

  offset = file->head_bytes + file->line_len * floor((jd - file->jd_start) / file->jd_step);
  prop_error(fn, novas_fetch_eop_chunk(&file->curl, file->url, offset, n * file->line_len, &data), 0);

  for(i = 0; i < n; i++) {
    time_t t = (jd - NOVAS_JD_J2000 + i * file->jd_step) * 86400L + UNIX_SECONDS_0UTC_1JAN2000;
    eop[i].leap =  novas_lookup_leap(t);
    prop_error(fn, eop_parse_line(file, i, lines, &eop[i]), 0);
  }

  //printf("%s\n", lines);

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

  if(jd >= (finals.jd_start + m * finals.jd_step) && mjd <= mjd_now + 365 - n) {
    // up to a year ahead...
    prop_error(fn, novas_fetch_from_file(&finals, jd - m * finals.jd_step, eop, n), 0);
  }
  else if(jd >= medium.jd_start + m * medium.jd_step) {
    prop_error(fn, novas_fetch_from_file(&medium, jd - m * medium.jd_step, eop, n), 0);
  }
  else if(jd >= old.jd_start + m * old.jd_step) {
    prop_error(fn, novas_fetch_from_file(&old, jd - m * old.jd_step, eop, n), 0);
  }
  else if(jd >= very_old.jd_start + m * very_old.jd_step) {
    prop_error(fn, novas_fetch_from_file(&very_old, jd - m * very_old.jd_step, eop, n), 0);
  }
  else {
    memset(eop, 0, sizeof(novas_eop));
    return novas_error(-1, ERANGE, fn, "JD %.3f is outside of IERS EOP data range", jd);
  }

  return 0;
}

static double spline_coeff(double dx) {
  dx = fabs(dx);
  return dx > 1.0 ? ((-0.5 * dx + 2.5) * dx - 4.0) * dx + 2.0 : (1.5 * dx - 2.5) * dx * dx + 1.0;
}

static int novas_eop_spline_interp(double jd, const novas_eop *restrict array, novas_eop *restrict eop) {
  double dx = (jd - array[1].jd) / (array[2].jd - array[1].jd);

  int i = 0;

  errno = 0;

  memset(eop, 0, sizeof(novas_eop));

  eop->leap = array[1].leap;

  for(i = 0; i < 4; i++) {
    const novas_eop *e = &array[i];
    float w = (float) spline_coeff(i - 1 - dx);

    eop->xp += w * e->xp;
    eop->yp += w * e->yp;
    eop->lod += w * e->lod;
    eop->dut1 += w * (e->dut1 - e->leap + eop->leap); // (spline UT1 - TAI) -> dUT1

    eop->xp_err += w * e->xp_err;
    eop->yp_err += w * e->yp_err;
    eop->dut1_err += w * e->dut1_err;
    eop->lod_err += w * e->lod_err;
  }

  if(errno)
    return novas_trace("novas_eop_spline_interp", -1, 0);

  return 0;
}

static void cleanup_file(iers_data_file *file) {
  CURL *curl = file->curl;

  if(!curl)
    return;

  file->curl = NULL;

  curl_easy_cleanup(curl);
  //free(curl);
}

/**
 * Releases resources used by URL handles used for obtaining Earth Orientation Parameter (EOP)
 * data from the International Earth Rotation and Reference Systems Service (IERS). This function
 * is automatically called at normal program exit, but users may call it explicitly to clean
 * up the tiny bit of resources used at any time.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_fetch_eop()
 */
void novas_cleanup_eop() {
  cleanup_file(&finals);
  cleanup_file(&medium);
  cleanup_file(&old);
  cleanup_file(&very_old);
}

/**
 * Obtains interpolated Earth Orientation Parameter data from the International Earth Rotation and
 * Reference Systems Service (IERS). For dates since 1972 or for prediction for up to a year ahead,
 * the rapid service data is used (`finals.all`), otherwise for dates after 1962 the C04 series is
 * used, and for dates all the way back to 1846, the C01 series is used. All of the mentioned data
 * are retrieved relative to the IAU2000 precession-nutation model.
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
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa novas_make_frame(), novas_set_time(), @ref earth
 */
int novas_fetch_eop(double jd, novas_eop *eop) {
  static const char *fn = "novas_fetch_eop";
  static int initialized;

  THREAD_LOCAL static novas_eop array[4];
  THREAD_LOCAL static double jd_from, jd_to = -1.0;

  if(!eop)
    return novas_error(-1, EINVAL, fn, "output eop is NULL");

  if(!initialized) {
    atexit(novas_cleanup_eop);
    initialized = 1;
  }


  if(jd < jd_from || jd > jd_to) {
    prop_error(fn, novas_fetch_eop_array(jd, array, 4), 0);
    jd_from = array[1].jd;
    jd_to = array[2].jd;
  }

  return novas_eop_spline_interp(jd, array, eop);
}
