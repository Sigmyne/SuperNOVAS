/**
 * @file
 *
 * @date Created  on Apr 18, 2026
 * @author Attila Kovacs and Kiran Shila
 *
 *  Functions to obtain and manage Earth Orientation data from the International Earth Rotation
 *  and Reference Systems Service (IERS) via HTTPS, or using other (remote or local) URLs.
 *
 *  The functions in this module are generally thread-safe. That is, you may call all functions
 *  from concurrent threads without data corruption. However, note that the EOP resources (URLs
 *  and/or local files), and the setting that enables/disables automatic fetching, are global
 *  settings. Thus, changing these in one thread will affect subsequent EOP fetch calls in other
 *  threads also.
 *
 *  @sa \ref earth
 */

#if !defined(_MSC_VER) && !defined(WITHOUT_LIBC)
/// \cond PRIVATE
#  define _GNU_SOURCE         ///< fmemopen() (before glibc 2.10)
 /// \endcond
#endif

#include <errno.h>
#include <string.h>
#include <time.h>

#ifdef WITHOUT_LIBC
#  ifndef WITHOUT_CURL
#    define WITHOUT_CURL      ///< Without libc, we also cannot have cURL...
#  endif
#else
#  include <stdio.h>
#  include <stdlib.h>         // atexit(), calloc(), free()
#endif

#ifndef WITHOUT_CURL
#  include <curl/curl.h>
#endif

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

#ifndef WITHOUT_LIBC
#  include "novas-mutex.h"
#endif

#if SKIP_IERS_DNS_LOOKUP
#  define IERS_DATACENTER         "141.74.67.212"       ///< datacenter.iers.org
#  define IERS_LEAP_SERVER        "145.238.80.89"       ///< hpiers.obspm.fr
#else
#  define IERS_DATACENTER         "datacenter.iers.org" ///< IERS EOP server name
#  define IERS_LEAP_SERVER        "hpiers.obspm.fr"     ///< IERS leap-seconds.list server name
#endif

/// \cond PRIVATE
#  define LEAP_FILENAME               "leap-seconds.list"
#  define LEAP_LIST_MAX_LEN           32768
#  define DEFAULT_LEAP_URL            "https://" IERS_LEAP_SERVER "/iers/bul/bulc/ntp/" LEAP_FILENAME
#  define NTP_UNIX_EPOCH              2208988800LL      ///< [s] NTP timestamp of UNIX epoch (1970 Jan 1)

#ifdef _MSC_VER
#  define gmtime_r        gmtime_s                      ///< MSC equivalent
#  define strtok_r        strtok_s                      ///< MSVC equivalent
#endif

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
/// \endcond

// ===========================================================================
#ifndef WITHOUT_LIBC

static iers_leap_entry *leaps;      ///< Leap seconds list
static time_t leap_expiration;      ///< UNIX time at which leap seconds list expires.
static lock_type leap_mutex;        ///< mutex for leap seconds data
static int leap_mutex_initialized;  ///< whether the leap mutex was initialized;

#endif // WITH_LIBC
// ===========================================================================


// ---------------------------------------------------------------------------
#ifndef WITHOUT_CURL
/// \cond PRIVATE

#  define IERS_LATEST_URL_PREFIX      "https://" IERS_DATACENTER "/data/latestVersion/"

#  define RAPID_JD_START              ( NOVAS_JD_MJD0 + 41684.0 )   ///< [day] Julian date of first entry in rapid service file
#  define C04_JD_START                ( NOVAS_JD_MJD0 + 37665.0 )   ///< [day] Julian date of first entry in C04 series file
#  define C01_JD_START                ( NOVAS_JD_MJD0 - 4703.268 )  ///< [day] Julian date of first entry in C01 series file
#  define C01_SPARSE_LINES            440           ///< Number of lines in C01 series with sparser sampling

/**
 * IERS data file structural description.
 */
typedef struct {
  CURL *curl;       ///< CURL handle (reused)
  enum novas_eop_series series; /// IERS EOP data series identifier
  long head_bytes;  ///< [bytes] Header bytes before regular table row data begins
  int line_len;     ///< [bytes] Length of data rows, including line feed
  int start_line;   ///< Line index at which to start using data
  double jd_check;  ///< [day] Julian date of first entry in file
  double jd_start;  ///< [day] Julian date of first entry to use
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

/// The currently configured URLs for each data series
static const char *urls[NOVAS_NUM_EOP_SERIES];

/// Serial number for the URL configuration;
static unsigned long url_version;

static int itrf_years[NOVAS_NUM_EOP_SERIES] = { -1, 2020, 2020, 2020 };

/// The default URLs for each data series.
static const char *default_urls[NOVAS_NUM_EOP_SERIES] = {
        DEFAULT_LEAP_URL,
        IERS_LATEST_URL_PREFIX "finals.all.iau2000.txt",
        IERS_LATEST_URL_PREFIX "EOP_20u24_C04_one_file_1962-now.txt",
        IERS_LATEST_URL_PREFIX "EOP_C01_IAU2000_1846-now.txt"
};

static const int default_itrf_years[NOVAS_NUM_EOP_SERIES] = { -1, 2020, 2020, 2020 };

/**
 * A simple data buffer for downloading data chunks from IERS
 */
typedef struct {
  char *buf;          ///< Allocated byte buffer
  size_t capacity;    ///< Capacity of byte buffer
  size_t size;        ///< Number of bytes filled
} download_buffer;

// 1973.01.02 to +365 days, no head
static iers_data_file rapid = { NULL, EOP_RAPID_IAU2000,
        0, 188, 0, RAPID_JD_START, RAPID_JD_START, 1.0,
        6, 17, 27, 36, 46, 58, 68, 78, 86
};

// 1962 -- now
static iers_data_file c04 = { NULL, EOP_C04_IAU2000_0UTC,
        -1, 219, 0, C04_JD_START, C04_JD_START, 1.0,
        16, 26, 122, 38, 134, 50, 146, 111, 206
};

// 1846 -- 1890 (0.1 year)
static iers_data_file c01_sparse = { NULL, EOP_C01_IAU2000,
        -1, 312, 0, C01_JD_START, C01_JD_START, 0.1 * NOVAS_TROPICAL_YEAR_DAYS,
        0, 12, 71, 22, 83, 32, 93, 226, 281
};

// 1890 -- now (0.05 year)
static iers_data_file c01 = { NULL, EOP_C01_IAU2000,
        -1, 312, C01_SPARSE_LINES, C01_JD_START, NOVAS_JD_MJD0 + 11367.380, 0.05 * NOVAS_TROPICAL_YEAR_DAYS,
        0, 12, 71, 22, 83, 32, 93, 226, 281
};

static int auto_fetch_eop = 1;    ///< Enable fetching EOP from IERS as needed by default
static lock_type eop_mutex;       ///< Mutex for EOP data (excl. leap) access
static int eop_mutex_initialized;  ///< Whether EOP mutex was initialized

/// \endcond
#endif /* WITH_CURL */
// ---------------------------------------------------------------------------

// ===========================================================================
#ifndef WITHOUT_LIBC

static void lock_leap() {
  if(!leap_mutex_initialized) {
    novas_init_lock(&leap_mutex);
    leap_mutex_initialized = 1;
  }
  novas_lock(&leap_mutex);
}

static void unlock_leap() {
  novas_unlock(&leap_mutex);
}

static void destroy_leap_list(iers_leap_entry *list) {
  while(list) {
    iers_leap_entry *e = list;
    list = e->next;
    free(e);
  }
}

static void cleanup_leaps_async() {
  destroy_leap_list(leaps);
}

static void set_leap_list_async(iers_leap_entry *list, long long expiration) {
  static int initialized;

  if(!initialized) {
    atexit(cleanup_leaps_async);
    initialized = 1;
  }

  iers_leap_entry *obsolete = leaps;
  leaps = list;
  leap_expiration = expiration;

  destroy_leap_list(obsolete);
}

static iers_leap_entry *parse_leaps(char *buf, long long *expiration) {
  static const char *fn = "parse_leap_file";

  iers_leap_entry *list = NULL;
  char *ptr = buf, *line, *context = NULL;

  // Parse leap-seconds.list data
  while((line = strtok_r(ptr, "\r\n", &context)) != NULL) if(line[0]) {
    ptr = NULL; // Keep parsing from the same input

    // Process expiration timestamp
    if(line[0] == '#') {
      if(line[1] == '@') {
        if(sscanf(&line[2], "%lld", expiration) < 1) {
          novas_set_errno(errno, fn, "could not parse leap-seconds.list expiration time.");
          return NULL;
        }
        *expiration -= NTP_UNIX_EPOCH;
      }
    }
    else {
      // Add leap entry to list
      iers_leap_entry *e = (iers_leap_entry *) calloc(1, sizeof(iers_leap_entry));
      long long start = 0LL;

      // -- It's hard to produce a malloc fail in a test environment, so we'll skip coverage tracking on it.
      // LCOV_EXCL_START
      if(!e) {
        destroy_leap_list(list);
        novas_set_errno(errno, fn, "leap entry alloc error: %s", strerror(errno));
        return NULL;
      }
      // LCOV_EXCL_STOP

      // Parse start time and leap seconds value
      if(sscanf(line, "%lld %d", &start, &e->leap) < 2) {
        destroy_leap_list(list);
        novas_set_errno(errno, fn, "invalid leap-seconds.list entry: %s", line);
        return NULL;
      }

      // Prepend to head of list.
      e->unix_start = (time_t) (start - NTP_UNIX_EPOCH);
      e->unix_end = (time_t) *expiration;
      e->next = list;
      if(e->next)
        e->next->unix_end = e->unix_start;
      list = e;
    }
  }

  return list;
}

static iers_leap_entry *parse_leap_file(FILE *fp, long long *expiration) {
  static const char *fn = "novas_parse_leap_file";

  long len;
  char *buf = NULL;
  iers_leap_entry *list;

  // LCOV_EXCL_START
  //
  //  The error conditions in this functions should not normally occur, and
  //  would be quite impossible to test for in a controlled way. Thus it makes
  //  no sense to include these bits in the code coverage stats.

  if(fseek(fp, 0, SEEK_END) < 0)
    goto error; // @suppress("Goto statement used")

  len = ftell(fp);
  if(len < 0)
    goto error; // @suppress("Goto statement used")

  if(len > LEAP_LIST_MAX_LEN) {
    errno = EFBIG;
    goto error; // @suppress("Goto statement used")
  }

  if(fseek(fp, 0, SEEK_SET) < 0)
    goto error; // @suppress("Goto statement used")

  buf = (char *) malloc(len + 1);
  if(!buf)
    goto error; // @suppress("Goto statement used")

  if(fread(buf, 1, len, fp) != (size_t) len)
    goto error; // @suppress("Goto statement used")

  buf[len] = '\0';

  list = parse_leaps(buf, expiration);
  free(buf);
  if(!list)
    novas_trace_invalid(fn);

  return list;

  // -------------------------------------------------------------------------
  error:

  if(buf)
    free(buf);

  novas_set_errno(errno, fn, strerror(errno));
  return NULL;
  // LCOV_EXCL_STOP
}

#endif /* WITH_LIBC */
// ===========================================================================

// ---------------------------------------------------------------------------
#ifndef WITHOUT_CURL

static void lock_eop() {
  if(!eop_mutex_initialized) {
    novas_init_lock(&eop_mutex);
    eop_mutex_initialized = 1;
  }
  novas_lock(&eop_mutex);
}

static void unlock_eop() {
  novas_unlock(&eop_mutex);
}

static size_t write_to_buffer(const char *ptr, size_t size, size_t nmemb, void *userdata) {
  download_buffer *data = (download_buffer *) userdata;

  size_t n = size * nmemb;
  if(data->size + n >= data->capacity) {
    novas_set_errno(ERANGE, "write_to_buffer", "buffer overflow");
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

  // Latency/performance optimization
  curl_easy_setopt(curl, CURLOPT_TCP_FASTOPEN, 1L);
  curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

  return curl;
}

static iers_leap_entry *fetch_leaps_async(long long *expiration) {
  static const char *fn = "fetch_leaps_async()";

  CURL *curl = NULL;
  CURLcode res;
  char str[LEAP_LIST_MAX_LEN];
  download_buffer data = { str, sizeof(str), 0 };
  iers_leap_entry *list;
  const char *url;

  curl = init_curl();
  if (!curl) {
    novas_trace_invalid(fn);
    return NULL;
  }

  url = novas_get_eop_url(EOP_LEAP_LIST);
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

  res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if(res) {
    novas_set_errno(EAGAIN, fn, "curl error %d for %s: %s", res, url, curl_easy_strerror(res));
    return NULL;
  }

  list = parse_leaps(str, expiration);
  if(!list)
    novas_trace_invalid(fn);

  return list;
}

static void cleanup_handle_async(iers_data_file *file) {
  CURL *curl = file->curl;

  if(!curl)
    return;

  file->curl = NULL;
  if(file->head_bytes)
    file->head_bytes = -1;

  curl_easy_cleanup(curl);
}

static void cleanup_eop_handles_async() {
  cleanup_handle_async(&rapid);
  cleanup_handle_async(&c04);
  cleanup_handle_async(&c01);
  cleanup_handle_async(&c01_sparse);
}

static int novas_fetch_eop_chunk(CURL **restrict pCurl, const char *restrict url, long offset, int len, download_buffer *restrict data,
        long timeout_millis) {
  static const char *fn = "novas_fetch_eop_chunk";
  static int initialized;

  CURL *curl;
  CURLcode res;
  char range[40] = {'\0'};

  if(!*pCurl)
    *pCurl = init_curl();

  curl = *pCurl;
  if (!curl)
    return novas_trace(fn, -1, 0);

  if(!initialized) {
    atexit(cleanup_eop_handles_async);
    initialized = 1;
  }

  novas_snprintf(range, sizeof(range), "%ld-%ld", offset, (offset + len - 1));

  curl_easy_setopt(curl, CURLOPT_URL, url);
  if(timeout_millis > 0L)
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout_millis);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
  curl_easy_setopt(curl, CURLOPT_RANGE, range);

  res = curl_easy_perform(curl);
  //curl_easy_cleanup(curl);

  if(res)
    return novas_error(-1, EAGAIN, fn, "curl error %d for %s: %s", res, url, curl_easy_strerror(res));

  return 0;
}

static float read_float(const char *str, int required) {
  float f = NAN;
  int n = sscanf(str, "%f", &f);
  if(required && n < 1) {
    errno = EBADMSG;
    return NAN;
  }
  return f;
}

static int eop_parse_line(const iers_data_file *restrict file, int line, char *str, novas_eop *restrict eop) {
  const char *from = &str[line * file->line_len];
  char end;
  errno = 0;

  eop->xp = eop->yp = eop->dut1 = NAN;

  end = from[file->line_len-1];
  if(end != '\n' && end != '\0')
    return novas_error(-1, EBADMSG, "eop_parse_line", "Corrupted entry or unexpected format");

  eop->series = file->series;
  eop->jd = NOVAS_JD_MJD0 + strtod(from + file->ijd, NULL);

  eop->xp = read_float(from + file->ixp, 1);
  eop->yp = read_float(from + file->iyp, 1);
  eop->dut1 = read_float(from + file->idut, 1);
  eop->lod = read_float(from + file->ilod, 0);

  eop->xp_err = read_float(from + file->ixpe, 0);
  eop->yp_err = read_float(from + file->iype, 0);
  eop->dut1_err = read_float(from + file->idute, 0);
  eop->lod_err = read_float(from + file->ilode, 0);

  if(file == &rapid) {
    eop->lod *= 1e-3;       ///< finals LOD is milliseconds.
    eop->lod_err *= 1e-3;
  }
  else if(file == &c01 || file == &c01_sparse) {
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

static int checkout_eop_file_async(iers_data_file *restrict file, long timeout_millis) {
  static const char *fn = "set_eop_file_struct";

  char buf[4096] = {'\0'};
  download_buffer head = { buf, sizeof(buf), 0 };
  char *next = buf;
  novas_eop eop = {};

  prop_error(fn, novas_fetch_eop_chunk(&file->curl, novas_get_eop_url(file->series), 0, head.capacity - 1, &head, timeout_millis), 0);

  // Skip empty and commented lines.
  for(; *next == '#' || *next == '\n'; next++)
    while(*(++next) && *next != '\n');  // parse to end of the line.

  prop_error(fn, eop_parse_line(file, 0, next, &eop), 0);

  if(fabs(eop.jd - file->jd_check) > 1e-2)
    return novas_error(-1, ERANGE, fn, "Mismatched JD in first entry: expected %.3f, got %.3f", file->jd_check, eop.jd);

  // Set the head
  file->head_bytes = next - buf + (long) file->start_line * file->line_len;

  return 0;
}

static int novas_fetch_eop_from_file(iers_data_file *restrict file, double jd, novas_eop *restrict eop, int n, long timeout_millis) {
  static const char *fn = "novas_fetch_eop_from_file";

  long offset;
  char lines[2048] = {'\0'};
  download_buffer data = { lines, sizeof(lines), 0 };
  int i;

  lock_eop();
  if(file->head_bytes < 0) {
    int stat = checkout_eop_file_async(file, timeout_millis);
    if(stat) {
      unlock_eop();
      return novas_trace(fn, -1, 0);
    }
  }

  offset = file->head_bytes + file->line_len * floor((jd - file->jd_start) / file->jd_step);
  unlock_eop();

  prop_error(fn, novas_fetch_eop_chunk(&file->curl, novas_get_eop_url(file->series), offset, n * file->line_len, &data, timeout_millis), 0);

  for(i = 0; i < n; i++) {
    time_t t = (jd - NOVAS_JD_J2000 + i * file->jd_step) * 86400L + UNIX_SECONDS_0UTC_1JAN2000;

    eop[i].leap = novas_lookup_leap(t);
    if(eop[i].leap == NOVAS_INVALID_LEAP)
      return novas_trace(fn, -1, 0);

    prop_error(fn, eop_parse_line(file, i, lines, &eop[i]), 0);
  }

  return 0;
}

static int novas_fetch_eop_array(double jd, long timeout_millis, novas_eop *restrict eop, int n) {
  static const char *fn = "novas_fetch_eop_array";

  novas_timespec ts = {};
  long mjd = (long) floor(jd - NOVAS_JD_MJD0);
  long mjd_now;
  int m = n >> 2;

  novas_set_current_time(0, 0.0, &ts);
  mjd_now = (long) floor(novas_get_time(&ts, NOVAS_TAI) - NOVAS_JD_MJD0);

  if(jd >= (rapid.jd_start + m * rapid.jd_step) && mjd <= mjd_now + 365 - n) {
    // up to a year ahead...
    prop_error(fn, novas_fetch_eop_from_file(&rapid, jd - m * rapid.jd_step, eop, n, timeout_millis), 0);
  }
  else if(jd >= c04.jd_start + m * c04.jd_step) {
    prop_error(fn, novas_fetch_eop_from_file(&c04, jd - m * c04.jd_step, eop, n, timeout_millis), 0);
  }
  else if(jd >= c01.jd_start + m * c01.jd_step) {
    int stat = novas_fetch_eop_from_file(&c01, jd - m * c01.jd_step, eop, n, timeout_millis);
    c01_sparse.line_len = c01.line_len;
    c01_sparse.head_bytes = c01.head_bytes - (long) c01.start_line * c01.line_len;
    prop_error(fn, stat, 0);
  }
  else if(jd >= c01_sparse.jd_start + m * c01_sparse.jd_step) {
    int stat = novas_fetch_eop_from_file(&c01_sparse, jd - m * c01_sparse.jd_step, eop, n, timeout_millis);
    c01.line_len = c01_sparse.line_len;
    c01.head_bytes = c01_sparse.head_bytes + (long) c01.start_line * c01_sparse.line_len;
    prop_error(fn, stat, 0);
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

  memset(eop, 0, sizeof(novas_eop));

  eop->series = array[1].series;
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

  return 0;
}

static void cleanup_eop_urls_async() {
  int i;
  for(i = 0; i < NOVAS_NUM_EOP_SERIES; i++)
    if(urls[i]) {
      char *url = (char *) urls[i];
      urls[i] = NULL;
      free(url);
    }
}

#endif /* WITH_CURL */
// ---------------------------------------------------------------------------

/**
 * Specifies a local file containing the official leap seconds list, i.e. `leap-seconds.list`, to
 * use, at least until its expiry. (If the local leap file expires, a new one will be fetched from
 * IERS as needed).
 *
 * It is similar to `novas_set_eop_url()` for the `EOP_LEAP_LIST` series, with a `file://` type
 * URL, except that this one does not need cURL support, and can work with relative paths also.
 *
 * NOTES:
 *
 *  - If __SuperNOVAS__ was built without `libc` support (`WITHOUT_LIBC` build configuration option),
 *    then this function will always return an error (`errno` set to `ENOSYS`).
 *
 *  - This call is not thread-safe. You should avoid calling it from concurrent threads if you
 *    want it to have predictable behavior.
 *
 * @param filename      Path to a local `leap-seconds.list` file (as obtained from IERS or a
 *                      mirror). It is typically included in the `tzdata` package on Linux, where
 *                      it may be found as `/usr/share/zoneinfo/leap-seconds.list` typically. Or,
 *                      NULL to clean up any prior leap seconds resources.
 * @return              0 if successful, or else -1 if there was an error (errno will indicate the
 *                      type of error).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa https://hpiers.obspm.fr/iers/bul/bulc/ntp/leap-seconds.list
 * @sa novas_set_eop_url(), novas_lookup_leap(), novas_fetch_eop(), novas_reset_eop()
 */
int novas_set_leap_list(const char *filename) {
  static const char *fn = "novas_set_leap_list";

#ifdef WITHOUT_LIBC
  (void) filename;
  return novas_error(-1, ENOSYS, fn, "SuperNOVAS was built without libc (WITHOUT_LIBC): file I/O unavailable");
#else
  FILE *fp;
  iers_leap_entry *list;
  long long expiration = 0LL;

  if(!filename) {
    set_leap_list_async(NULL, 0LL);
    return 0;
  }

  if(!filename[0])
    return novas_error(-1, EINVAL, fn, "leap seconds file name is empty");

  fp = fopen(filename, "r");
  if(!fp)
    return novas_error(-1, errno, fn, "open %s: %s", filename, strerror(errno));

  list = parse_leap_file(fp, &expiration);
  fclose(fp);

  if(!list)
    return novas_trace(fn, -1, 0);

  lock_leap();
  set_leap_list_async(list, expiration);
  unlock_leap();

  return 0;
#endif /* !WITHOUT_LIBC */
}

/**
 * Returns the leap seconds for the given UNIX timestamp, based either on a locally supplied leap
 * seconds list (see `novas_set_leap_list()`), or else from data obtained as needed from IERS
 * (provided that fetching it is allowed). In case of errors -1 is returned, with errno set as
 * appropriate.
 *
 * Leap seconds were first introduced on 1 Jan 1972. Thus for date preceding the introduction,
 * 0 is returned. Leap seconds prognosis into the future is available only up to the expiration
 * date of the `leap-seconds.list` file (as supplied or updated from IERS).
 *
 * NOTES:
 *
 *  - If __SuperNOVAS__ was built without `libc` support (`WITHOUT_LIBC` build configuration option),
 *    then this function will always return an error (`errno` set to `ENOSYS`).
 *
 * @param t     [s] UNIX time (seconds since 0 UTC, 1 Jan 1970)
 * @return      The leap seconds for the given time, or NOVAS_INVALID_LEAP (-999) if there was an
 *              error (errno will indicate the type of error).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_set_leap_list(), novas_set_eop_url(), novas_fetch_eop(), novas_is_auto_fetch_eop(),
 *     NOVAS_INVALID_LEAP
 */
int novas_lookup_leap(time_t t) {
  static const char *fn = "novas_lookup_leap";

#ifdef WITHOUT_LIBC
  (void) t;
  return novas_error(NOVAS_INVALID_LEAP, ENOSYS, fn, "SuperNOVAS was built without libc (WITHOUT_LIBC): specify leap seconds explicitly instead.");
#else
  const iers_leap_entry *e;

  lock_leap();
  if(!leaps || (t >= leap_expiration && time(NULL) >= leap_expiration)) {

#  ifdef WITHOUT_CURL
    unlock_leap();
    return novas_error(NOVAS_INVALID_LEAP, ERANGE, fn, "no leap data available for time %lld (WITHOUT_CURL)", (long long) t);
#  else
    if(novas_is_auto_fetch_eop()) {
      long long expiration = 0LL;
      iers_leap_entry *update = fetch_leaps_async(&expiration);
      if(!update) {
        unlock_leap();
        return novas_trace(fn, 1, NOVAS_INVALID_LEAP - 1); // trick work-around propagating negative (not -1) error code.
      }
      set_leap_list_async(update, expiration);
    }
    else {
      unlock_leap();
      return novas_error(NOVAS_INVALID_LEAP, EAGAIN, fn, "automatic EOP fetching is disabled.");
    }
#  endif
  }

  if(t > leaps->unix_end) {
    struct tm tm = {};
    char str[40] = {'\0'};
    unlock_leap();
    gmtime_r(&t, &tm);
    strftime(str, sizeof(str), "%c", &tm);
    return novas_error(NOVAS_INVALID_LEAP, ERANGE, fn, "Time %s is beyond the leap seconds coverage range", str);
  }

  for(e = leaps; e != NULL; e = e->next)
    if(t >= e->unix_start && t < e->unix_end) {
      unlock_leap();
      return e->leap;
    }

  unlock_leap();

  return 0;
#endif /* !WITHOUT_LIBC */
}


/**
 * Specify a URL to use for a given IERS Earth Orientation Parameter (EOP) series. By default,
 * `novas_fetch_eop()` will use the standard IERS data center URLs to obtain EOP, but users may
 * want to use a local file, or a local mirror instead for faster, more reliable source(s) for
 * the IERS data. This function allows you to do just that.
 *
 * This call will check out the specified URL, to ensure it points to a suitable file for the
 * series, and will return an error if the file is not appropriate. As such, the call may be
 * delayed for a long time, while cURL fetches a few kilobytes from the file, needed for
 * validation.
 *
 * NOTES:
 *
 *  - Requires __SuperNOVAS__ to be compiled with cURL support enabled, otherwise -1 is returned
 *    with `errno` set to `ENOSYS`.
 *
 *  - This call is not thread-safe. You should avoid calling it from concurrent threads if you
 *    want it to have predictable behavior.
 *
 * @param series      The EOP series identifier constant.
 * @param itrf_year   [yr] ITRF realization year. Needed only for precision at the few &mu;as
 *                    level, otherwise, you can set it to something recent, like 2020. It is
 *                    unused if the URL is NULL, or if the series if the leap seconds list. For
 *                    years prior to the first ITRF realization (in 1988), 1988 will be used.
 * @param url         The new URL to use for the given data series, or NULL to use the default
 *                    IERS data center URL.
 * @return            0 if successful, or else -1 if there was an error (`errno` will be set to
 *                    `ERANGE` if the series is invalid, `EINVAL` if the URL is an empty string,
 *                    or `ENOSYS` if __SuperNOVAS__ was built without cURL support).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_get_eop_url(), novas_fetch_eop(), novas_set_leap_list(), novas_get_eop_itrf_year()
 */
int novas_set_eop_url(enum novas_eop_series series, int itrf_year, const char *url) {
  static const char *fn = "novas_set_eop_url";

#ifdef WITHOUT_CURL
  return novas_error(-1, ENOSYS, fn, "SuperNOVAS was built without cURL support (WITHOUT_CURL)");
#else
  static int initialized;
  char *discard;
  int status = 0;

  if(url && !url[0])
    return novas_error(-1, EINVAL, fn, "empty URL for series %d", (int) series);

  if(!initialized) {
    atexit(cleanup_eop_urls_async);
    initialized = 1;
  }

  if(series != EOP_LEAP_LIST)
    lock_eop();

  // Close existing handles...
  switch(series) {
    case EOP_LEAP_LIST:
      novas_set_leap_list(NULL);
      break;
    case EOP_RAPID_IAU2000:
      cleanup_handle_async(&rapid);
      break;
    case EOP_C04_IAU2000_0UTC:
      cleanup_handle_async(&c04);
      break;
    case EOP_C01_IAU2000:
      cleanup_handle_async(&c01);
      cleanup_handle_async(&c01_sparse);
      break;
    default:
      unlock_eop();
      return novas_error(-1, ERANGE, fn, "invalid EOP series %d", (int) series);
  }

  if(itrf_year < 1988)
    itrf_year = 1988;

  discard = (char *) urls[series];
  urls[series] = url ? strdup(url) : NULL;
  url_version++;

  if(series != EOP_LEAP_LIST)
    itrf_years[series] = url ? itrf_year : default_itrf_years[series];

  if(discard)
    free(discard);

  switch(series) {
    case EOP_LEAP_LIST:
      if(novas_lookup_leap(0L) != 0)
        return novas_trace(fn, -1, 0);
      return 0;
    case EOP_RAPID_IAU2000:
      status = checkout_eop_file_async(&rapid, 0);
      break;
    case EOP_C04_IAU2000_0UTC:
      status = checkout_eop_file_async(&c04, 0);
      break;
    case EOP_C01_IAU2000:
      status = checkout_eop_file_async(&c01, 0);
      break;
  }

  unlock_eop();

  prop_error(fn, status, 0);
  return 0;
#endif // WITH_CURL
}

/**
 * Returns the URL currently configured for a given IERS Earth Orientation Parameter (EOP) series.
 *
 * NOTES:
 *
 *  - If __SuperNOVAS__ was built without cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *    configuration options), then this function will return an error (`errno` set to `ENOSYS`).
 *
 * @param series    The EOP series identifier constant.
 * @return          The currently configured URL for the given series, or else NULL if the series
 *                  is invalid (`errno` set to `ERANGE`), or if __SuperNOVAS__ was built without
 *                  cURL support (`errno` set to `ENOSYS`).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_set_eop_url(), novas_fetch_eop(), novas_get_eop_itrf_year()
 */
const char *novas_get_eop_url(enum novas_eop_series series) {
#ifdef WITHOUT_CURL
  novas_set_errno(ENOSYS, "novas_get_eop_url", "SuperNOVAS was built without cURL support (WITHOUT_CURL)");
  return NULL;
#else
  const char *url;

  if(series < 0 || series >= NOVAS_NUM_EOP_SERIES) {
    novas_set_errno(ERANGE, "novas_get_eop_url", "invalid EOP series %d", (int) series);
    return NULL;
  }

  url = urls[series];
  return url ? url : default_urls[series];
#endif
}

/**
 * Returns the ITRF realization year for a given IERS Earth Orientation Parameter (EOP) series.
 *
 * NOTES:
 *
 *  - If __SuperNOVAS__ was built without cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *    configuration options), then this function will return an error (`errno` set to `ENOSYS`).
 *
 * @param series    The EOP series identifier constant.
 * @return          [yr] ITRF realization year, e.g. as set by `novas_set_eop_url()`.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_set_eop_url(), novas_fetch_eop()
 */
int novas_get_eop_itrf_year(enum novas_eop_series series) {
#ifdef WITHOUT_CURL
  return novas_error(-1, ENOSYS, "novas_get_eop_itrf_year", "SuperNOVAS was built without cURL support (WITHOUT_CURL)");
#else
  if((unsigned int) series >= NOVAS_NUM_EOP_SERIES)
    return novas_error(-1, EINVAL, "novas_get_eop_itrf_year", "invalid series: %d", (int) series);
  return itrf_years[series];
#endif
}

/**
 * Releases resources used by URL handles used for obtaining Earth Orientation Parameter (EOP)
 * data from the International Earth Rotation and Reference Systems Service (IERS), including
 * the leap seconds list supplied earlier or obtained from IERS. It also discards any custom
 * URLs that may have been set previously, and restores the default IERS URLs for obtaining leap
 * seconds and EOP from IERS. The cleanup performed by this function is automatic at normal
 * program exit, but users may call it explicitly restore the initial state at any point.
 *
 * NOTES:
 *
 *  - This call does not affect or destroy any thread-local data currently cached. As such,
 *    `novas_fetch_eop()` may continue to return EOP using the previously cached values, if the
 *    requested date falls within the same data bracket as the last call.
 *
 *  - This call is not thread-safe. You should avoid calling it from concurrent threads if you
 *    want it to have predictable behavior.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_set_leap_list(), novas_fetch_eop()
 */
void novas_reset_eop() {
#ifndef WITHOUT_LIBC
 lock_leap();
 set_leap_list_async(NULL, 0LL);
 unlock_leap();
#endif

#ifndef WITHOUT_CURL
 lock_eop();
 cleanup_eop_urls_async();
 cleanup_eop_handles_async();
 unlock_eop();
#endif
}

/**
 * Obtains interpolated Earth Orientation Parameter data from the International Earth Rotation and
 * Reference Systems Service (IERS). For dates since 1972 or for prediction for up to a year ahead,
 * the rapid service data is used (`finals.all`), otherwise for dates after 1962 the C04 series is
 * used, and for dates all the way back to 1846, the C01 series is used. All of the mentioned data
 * are retrieved relative to the IAU2000 precession-nutation model.
 *
 * You should always check the return status when using this function, since the fetching of EOP
 * values may fail for a host of different reasons.
 *
 * NOTES:
 *
 *  1. You must have built __SuperNOVAS__ with cURL support. Otherwise, this function will return
 *     -1, with `errno` set to `ENOSYS`.
 *
 *  2. You will need an internet connection and the IERS server must be online and accessible,
 *     unless you have changed the necessary URLs to use local copies instead (see
 *     `novas_set_eop_url()`).
 *
 *  3. Obtaining values from IERS can have arbitrary latencies, and can impact the performance
 *     severely. For performance critical applications, you should consider specifying the EOP
 *     values more directly, or using local files instead (see also `novas_set_eop_url()`).
 *
 *  4. This function assumes that the files served from IERS remain accessible and their format
 *     does not change over time. As such what works today, may not work the same in the future.
 *
 *  5. This function caches EOP data from the last query. As such repeated calls within the same
 *     date bracket (typically the same MJD day) will return fast and will interpolate from the
 *     data last obtained from the IERS.
 *
 *  6. This function uses cubic spline interpolation around the published data points. As such,
 *     it needs at least two data points at the same time or before, and two points after, the
 *     date for which interpolated values are requested.
 *
 *  7. Prior to 1 Jan 1956, the UT1-UTC time difference, and length-of-day (LOD) are not provided
 *     by IERS, as they were not measured prior to the age of atomic clocks. Hence, for dates
 *     prior to 1956 the returned EOP will have `dut1` and `lod` (and their uncertainties) set to
 *     NAN. (However, the polar the offsets _x_<sub>p</sub> and _y_<sub>p</sub> can be provided
 *     all the way back to 1846, with some precision.)
 *
 *  8. If __SuperNOVAS__ was built without cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *     configuration options), then this function will return an error (`errno` set to `ENOSYS`).
 *
 * @param jd              Julian Date (in any timescale, with a preference for UTC)
 * @param timeout_millis  [ms] HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @param[out] eop        Output EOP data structure to populate
 * @return                0 if successful or else -1 (errno will indicate the type of error).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa novas_fetch_current_eop(), novas_set_eop_url(), novas_reset_eop()
 * @sa novas_make_frame(), novas_set_time(), novas_set_auto_fetch_eop(), @ref earth
 */
int novas_fetch_eop(double jd, long timeout_millis, novas_eop *eop) {
  static const char *fn = "novas_fetch_eop";

#ifdef WITHOUT_CURL
  return novas_error(-1, ENOSYS, fn, "SuperNOVAS was built without cURL support (WITHOUT_CURL).");
#else
  static THREAD_LOCAL unsigned long version = -1;
  static THREAD_LOCAL novas_eop array[4];
  static THREAD_LOCAL double jd_from, jd_to = -1.0;

  if(!eop)
    return novas_error(-1, EINVAL, fn, "output eop is NULL");

  if(version != url_version || jd_to < jd_from || jd < jd_from || jd > jd_to) {
    prop_error(fn, novas_fetch_eop_array(jd, timeout_millis, array, 4), 0);

    if(!(jd >= array[1].jd && jd <= array[2].jd))
      return novas_error(-1, EBADMSG, fn, "Misaligned dates in file");

    jd_from = array[1].jd;
    jd_to = array[2].jd;
    version = url_version;
  }

  return novas_eop_spline_interp(jd, array, eop);
#endif
}

/**
 * Obtains interpolated Earth Orientation Parameter data from the International Earth Rotation and
 * Reference Systems Service (IERS) around the current system time, using the rapid service data
 *  (`finals.all`).
 *
 * You should always check the return status when using this function, since the fetching of EOP
 * values may fail for a host of different reasons.
 *
 * NOTES:
 *
 *  1. You must have access to the `curl` library and built __SuperNOVAS__ with CURL support.
 *     Otherwise this function will return -1, with `errno` set to `ENOSYS`.
 *
 *  2. You will need an internet connection and the IERS server must be online and accessible (at
 *     least for the initial call).
 *
 *  3. Obtaining values from IERS can have arbitrary latencies, and can impact performance
 *     severely. For performance critical applications, you should consider specifying the EOP
 *     values more directly, e.g. from a local file instead.
 *
 *  4. This function assumes that the files served from IERS remain accessible and their format
 *     does not change over time.
 *
 *  5. This function caches EOP data from the last online query. As such repeated calls within the
 *     same data bracket (typically the same MJD day) will return fast and will reuse the last
 *     data obtained from the IERS.
 *
 *  6. This function uses cubic spline interpolation of the published data points.
 *
 *  7. The returned EOP values are assumed to be ITRF 2020 values, which is appropriate
 *     at the time this __SuperNOVAS__ module was written or last updated. If IERS later publishes
 *     data in some other future ITRF realization, this module may need to be updated, accordingly.
 *     However, the ITRF realization is unlikely to matter significantly.
 *
 *  8. If __SuperNOVAS__ was built without cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *     configuration options), then this function will return an error (`errno` set to `ENOSYS`).
 *
 * @param t               [s] UNIX time (seconds since 0 UTC 1 Jan 1970).
 * @param timeout_millis  [ms] HTTP connection timeout, or &lt;=0 to leave unchanged.
 * @param[out] eop        Output EOP data structure to populate
 * @return                0 if successful or else -1 (errno will indicate the type of error).
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop
 * @sa novas_set_eop_url()
 * @sa novas_make_frame(), novas_set_time(), novas_set_auto_fetch_eop(), @ref earth
 */
int novas_fetch_eop_unix(time_t t, long timeout_millis, novas_eop *eop) {
  double jd = NOVAS_JD_J2000 + (t - UNIX_SECONDS_0UTC_1JAN2000) / 86400.0;
  prop_error("novas_fetch_eop_unix", novas_fetch_eop(jd, timeout_millis, eop), 0);
  return 0;
}

/**
 * Disable or Re-enabled the automatic fetching of Earth Orientation Parameter values from
 * IERS, when these are left undefined (NAN) when initializing astrometric time or observing
 * frame instances.
 *
 * If set to TRUE (non-zero) then when an astrometric time is initialized with an undefined EOP,
 * whose dUT1 value is NAN, it will automatically fetch appropriate leap second and dUT1 values
 * from IERS, if possible. Similarly, if an observing frame is configured with a _x_<sub>p</sub>
 * or _y_<sub>p</sub> values are NAN, the frame will fetch appropriate polar offsets from IERS
 * for the time of observation.
 *
 * The default value of this global setting is TRUE (1), hence the automatic fetching of EOP
 * values from IERS is enabled by default, and will override NAN EOP values with the fetched
 * interpolated data, whenever possible.
 *
 * If you do not want this behavior, e.g. because of the associated arbitrary latencies involved
 * with getting data from IERS, or because you are using __SuperNOVAS__ offline, then simply
 * call this function with FALSE (0) to disable the automatic fetching of EOP values.
 *
 * NOTES:
 *
 *  - If __SuperNOVAS__ was built without cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *    configuration options), then this function will return an error (`errno` set to `ENOSYS`) if
 *    trying to enable EOP fetching.

 *  - This call is not thread-safe. You should avoid calling it from concurrent threads if you
 *    want it to have predictable behavior.
 *
 * @param enabled     TRUE (non-zero) to enabled automatic fetching of EOP values from IERS,
 *                    or else FALSE (0) to disable. By default EOP fetching is enabled.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_is_auto_fetch_eop(), novas_fetch_eop(), novas_lookup_leap()
 * @sa novas_set_time(), novas_make_frame()
 * @sa Time, Frame, GeodeticObserver, CalendarDate::to_time()
 */
int novas_set_auto_fetch_eop(int enabled) {
#ifdef WITHOUT_CURL
  if(enabled)
    return novas_error(-1, ENOSYS, "novas_set_auto_fetch_eop", "SuperNOVAS was built without cURL support (WITHOUT_CURL).");
#else
  auto_fetch_eop = (enabled != 0);
#endif
  return 0;
}

/**
 * Checks if automatic fetching of Earth Orientation Parameter values from IERS is
 * allowed.
 *
 * NOTES:
 *
 *  - If __SuperNOVAS__ was built without cURL support (`WITHOUT_CURL` or `WITHOUT_LIBC` build
 *    configuration options), then this function will return an error (`errno` set to `ENOSYS`).
 *
 * @return    TRUE (1) if automatic fetching of EOP values from IERS is allowed, or else
 *            FALSE (0) if fetching is disabled.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_set_auto_fetch_eop()
 */
int novas_is_auto_fetch_eop() {
#ifdef WITHOUT_CURL
  return 0;
#else
  return auto_fetch_eop;
#endif
}
