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
#else
#  define IERS_DATACENTER         "datacenter.iers.org" ///< IERS EOP server name
#endif



/// \cond PRIVATE
#  define NTP_UNIX_EPOCH              2208988800LL      ///< [s] NTP timestamp of UNIX epoch (1970 Jan 1)
#  define N_LEAPS                     28                ///< Number of leap seconds introduced between 1970 and 2027
#  define UNIX_UTC_J2000              (UNIX_SECONDS_0UTC_1JAN2000 + 43200)

#ifdef _MSC_VER
#  define strtok_r        strtok_s                      ///< MSC equivalent
#endif
/// \endcond

// Definitive list of leap seconds NTP timestamps before abolishment on 20 May 2027
static const long long ntp_leap_starts[N_LEAPS] = { //
        2272060800LL, 2287785600LL, 2303683200LL, 2335219200LL, 2366755200LL, 2398291200LL, 2429913600LL, 2461449600LL, //
        2492985600LL, 2524521600LL, 2571782400LL, 2603318400LL, 2634854400LL, 2698012800LL, 2776982400LL, 2840140800LL, //
        2871676800LL, 2918937600LL, 2950473600LL, 2982009600LL, 3029443200LL, 3076704000LL, 3124137600LL, 3345062400LL, //
        3439756800LL, 3550089600LL, 3644697600LL, 3692217600LL };


// ---------------------------------------------------------------------------
#ifndef WITHOUT_CURL

/// \cond PRIVATE

/// The official IERS data service.
#  define IERS_LATEST_URL_PREFIX      "https://" IERS_DATACENTER "/data/latestVersion/"

/// USNO provides the rapid service data for IERS, which is also published by them directly
#  define USNO_URL_PREFIX             "https://maia.usno.navy.mil/"

/// The Observatory of Paris maintains a mirror for some of the EOP data
#  define HPIERS_URL_PREFIX           "https://hpiers.obspm.fr/iers/eop/"

/// The 'brahe' satellite dynamics package also maintains a mirror for some of the EOP data
#  define BRAHE_MIRROR_URL_PREFIX     "https://www.simplespacedata.org/eop/iers/"

/// The set of EOP URLs for the official data products published by IERS
#define EOP_IERS_URL_SET { \
        NULL, \
        IERS_LATEST_URL_PREFIX "finals.all.iau2000.txt", \
        IERS_LATEST_URL_PREFIX "EOP_20u24_C04_one_file_1962-now.txt", \
        IERS_LATEST_URL_PREFIX "EOP_C01_IAU2000_1846-now.txt" \
}

/// Alternative EOP URLs from USNO and Observatory of Paris
#define EOP_ALT_URL_SET { \
        NULL, \
        USNO_URL_PREFIX "ser7/finals2000A.daily.extended", \
        HPIERS_URL_PREFIX "eopc04/eopc04.1962-now", \
        HPIERS_URL_PREFIX "eopc01/eopc01.1846-now" \
}

///  Alternative EOP URLs by 'brahe' and from IERS (for C01)
#define EOP_BRAHE_URL_SET { \
        NULL, \
        BRAHE_MIRROR_URL_PREFIX "finals_all/latest/finals.all.iau2000.txt", \
        BRAHE_MIRROR_URL_PREFIX "c04/latest/EOP_C04_one_file_1962-now.txt", \
        IERS_LATEST_URL_PREFIX "EOP_C01_IAU2000_1846-now.txt" \
}

#ifndef DEFAULT_EOP_URL_SET
#  define DEFAULT_EOP_URL_SET EOP_BRAHE_URL_SET ///< Use the 'brahe' mirrors primarily
#endif


#  define RAPID_JD_START              ( NOVAS_JD_MJD0 + 41684.0 )   ///< [day] Julian date of first entry in rapid service file
#  define C04_JD_START                ( NOVAS_JD_MJD0 + 37665.0 )   ///< [day] Julian date of first entry in C04 series file
#  define C01_JD_START                ( NOVAS_JD_MJD0 - 4703.268 )  ///< [day] Julian date of first entry in C01 series file
#  define C01_SPARSE_LINES            440           ///< Number of lines in C01 series with sparser sampling

/**
 * IERS data file structural description.
 */
typedef struct {
  CURL *curl;       ///< CURL handle (reused)
  enum novas_eop_series series; ///< IERS EOP data series identifier
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
/// \endcond

/// The currently configured URLs for each data series
static const char *urls[NOVAS_NUM_EOP_SERIES];

/// [yr] The default ITRF realization for each of the EOP series
static int itrf_years[NOVAS_NUM_EOP_SERIES] = { -1, 2020, 2020, 2020 };

/// Serial number for the URL configuration
static unsigned long url_version;

/// The default URLs for each data series.
static const char *default_urls[NOVAS_NUM_EOP_SERIES] = DEFAULT_EOP_URL_SET;

/// [yr] The default ITRF realization for each of the EOP series
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
//  alternative: https://maia.usno.navy.mil/ser7/finals2000A.daily.extended
static iers_data_file rapid = { NULL, EOP_RAPID_IAU2000,
        0, 188, 0, RAPID_JD_START, RAPID_JD_START, 1.0,
        6, 17, 27, 36, 46, 58, 68, 78, 86
};

// 1962 -- now
//  alternative: https://hpiers.obspm.fr/iers/eop/eopc04/eopc04.1962-now
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
//  alternative: https://hpiers.obspm.fr/iers/eop/eopc01/eopc01.1846-now
static iers_data_file c01 = { NULL, EOP_C01_IAU2000,
        -1, 312, C01_SPARSE_LINES, C01_JD_START, NOVAS_JD_MJD0 + 11367.380, 0.05 * NOVAS_TROPICAL_YEAR_DAYS,
        0, 12, 71, 22, 83, 32, 93, 226, 281
};

static int auto_fetch_eop = 1;    ///< Enable fetching EOP from IERS as needed by default

#ifdef NOVAS_LOCK_INITIALIZER
static lock_type eop_mutex = NOVAS_LOCK_INITIALIZER;    ///< Mutex for EOP data.
#elif __STDC_VERSION__ >= 201112L
static lock_type eop_mutex;                             ///< Mutex for EOP data.
static once_flag eop_mutex_once = ONCE_FLAG_INIT;       ///< One-time mutex initialization state.
#else
#  error "Oops. No thread safe mutex initialization available."
#endif


#if !defined(NOVAS_LOCK_INITIALIZER) && __STDC_VERSION__ >= 201112L
static void init_eop_mutex() {
  novas_init_lock(&eop_mutex);
}
#endif

static void lock_eop() {
#if !defined(NOVAS_LOCK_INITIALIZER) && __STDC_VERSION__ >= 201112L
  call_once(&eop_mutex_once, init_eop_mutex);
#endif
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

static int novas_fetch_eop_chunk_async(CURL **restrict pCurl, const char *restrict url, long offset, int len, download_buffer *restrict data,
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

  // It's bad news if we get a HTML response (we expect ASCII)
  if(strstr(data->buf, "<html>")) {
    // LCOV_EXCL_START
    return novas_error(-1, EAGAIN, fn, "HTTP request was rejected for %s\n", url);
    // LCOV_EXCL_STOP
  }

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
    return novas_error(-1, EBADMSG, "eop_parse_line", "Corrupted entry or unexpected format (series %d)");

  eop->series = file->series;
  eop->jd = NOVAS_JD_MJD0 + strtod(from + file->ijd, NULL);
  eop->leap = novas_leap_for_jd(eop->jd);

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

  prop_error(fn, novas_fetch_eop_chunk_async(&file->curl, novas_get_eop_url(file->series), 0, head.capacity - 1, &head, timeout_millis), 0);

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
  int i, status;

  lock_eop();

  if(file->head_bytes < 0) {
    if(checkout_eop_file_async(file, timeout_millis)) {
      unlock_eop();
      return novas_trace(fn, -1, 0);
    }
  }

  offset = file->head_bytes + file->line_len * floor((jd - file->jd_start) / file->jd_step);
  status = novas_fetch_eop_chunk_async(&file->curl, novas_get_eop_url(file->series), offset, n * file->line_len, &data, timeout_millis);
  if(status < 0) goto cleanup; // @suppress("Goto statement used")

  for(i = 0; i < n; i++) {
    status = eop_parse_line(file, i, lines, &eop[i]);
    if(status < 0)
      goto cleanup; // @suppress("Goto statement used")
  }

  cleanup:

  unlock_eop();
  prop_error(fn, status, 0);

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
    prop_error(fn, novas_fetch_eop_from_file(&c01, jd - m * c01.jd_step, eop, n, timeout_millis), 0);
    lock_eop();
    c01_sparse.line_len = c01.line_len;
    c01_sparse.head_bytes = c01.head_bytes - (long) c01.start_line * c01.line_len;
    unlock_eop();
  }
  else if(jd >= c01_sparse.jd_start + m * c01_sparse.jd_step) {
    prop_error(fn, novas_fetch_eop_from_file(&c01_sparse, jd - m * c01_sparse.jd_step, eop, n, timeout_millis), 0);
    lock_eop();
    c01.line_len = c01_sparse.line_len;
    c01.head_bytes = c01_sparse.head_bytes + (long) c01.start_line * c01_sparse.line_len;
    unlock_eop();
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

  eop->series = array[1].series;

  eop->xp = 0.0;
  eop->yp = 0.0;
  eop->dut1 = 0.0;
  eop->lod = 0.0;

  for(i = 0; i < 4; i++) {
    const novas_eop *e = &array[i];
    float w = (float) spline_coeff(i - 1 - dx);

    eop->xp += w * e->xp;
    eop->yp += w * e->yp;
    eop->lod += w * e->lod;
    eop->dut1 += w * (e->dut1 - e->leap + eop->leap); // (spline UT1 - TAI) -> dUT1

    eop->xp_err += w * e->xp_err * e->xp_err;
    eop->yp_err += w * e->yp_err * e->yp_err;
    eop->dut1_err += w * e->dut1_err * e->dut1_err;
    eop->lod_err += w * e->lod_err * e->lod_err;
  }

  eop->xp_err = sqrt(eop->xp_err);
  eop->yp_err = sqrt(eop->yp_err);
  eop->dut1_err = sqrt(eop->dut1_err);
  eop->lod_err = sqrt(eop->lod_err);

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
 * Obsoleted function to specify a file containing the leap seconds list. There will be no more leap
 * seconds as of 20 May 2027, so the final list of leap seconds is now hardcoded into SuperNOVAS as
 * of v1.8. There will be leap hours instead, perhaps, in the distant future, but not for some millenia.
 * If someone still uses SuperNOVAS then, well, they must make some modifications to handle it then...
 *
 * REFERENCES:
 *
 *  1. General Conference on Weights and Measures. "Resolution 5: On the adoption of a continuous UTC."
 *     Document presented at the 28th Meeting of the CGPM, Bureau International des Poids et Mesures (BIPM),
 *     Versailles, France, October 15, 2026. https://www.bipm.org/en/cgpm-2026/resolution-5.
 *
 * @param filename      _unused_
 * @return              0 is successful, or -1 if `novas_debug()` was called with `NOVAS_DEBUG_EXTRA`.
 *                      In both cases errno will be set to `ENOSYS`.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa https://hpiers.obspm.fr/iers/bul/bulc/ntp/leap-seconds.list
 * @sa novas_set_eop_url(), novas_lookup_leap(), novas_fetch_eop(), novas_reset_eop()
 *
 * @deprecated There will be no more leap seconds, so we readily know the final leap list, which
 *      is now hardcoded into SuperNOVAS as of v1.8. This function will return 0 (unless extra
 *      level debugging is enabled) to remain compatible with existing working code that uses this
 *      function, but it will no longer actually load the specified leap seconds file. It will
 *      always set `errno` to `ENOSYS` to indicate that the function is no longer meaningful.
 */
int novas_set_leap_list(const char *filename) {
  (void) filename; // unused
  novas_set_errno(ENOSYS, "novas_set_leap_list", "Setting leap seconds lists is no longer supported or needed.");
  return novas_get_debug_mode() == NOVAS_DEBUG_EXTRA ? -1 : 0;
}

/**
 * Returns the leap seconds for the given UNIX timestamp.
 *
 * Leap seconds were first introduced on 1 Jan 1972. Thus for date preceding the introduction,
 * 0 is returned. The last leap of 27 seconds was introduced on 1 January 2017, and the decision
 * has been made to not introduce further leap seconds as of 20 May 2027, leaving 37 seconds
 * for the foreseeable future.
 *
 * REFERENCES:
 *
 *  1. General Conference on Weights and Measures. "Resolution 5: On the adoption of a continuous UTC."
 *     Document presented at the 28th Meeting of the CGPM, Bureau International des Poids et Mesures (BIPM),
 *     Versailles, France, October 15, 2026. https://www.bipm.org/en/cgpm-2026/resolution-5.
 *
 *
 * @param t     [s] UNIX time (seconds since 0 UTC, 1 Jan 1970)
 * @return      The leap seconds for the given time.
 *
 * @since 1.7
 * @author Attila Kovacs
 *
 * @sa novas_leap_for_jd(), novas_fetch_eop(), novas_is_auto_fetch_eop()
 */
int novas_lookup_leap(time_t t) {
  long long ntpdate = t + NTP_UNIX_EPOCH;
  int i;

  for(i = N_LEAPS; --i >= 0; )
    if(ntpdate >= ntp_leap_starts[i]) return 10 + i;

  return 0;
}

/**
 * Returns the leap seconds for the given (UTC-based) Julian date. The UTC-based dates matter only
 * in the immediate vicinity of the leap seconds changes, since leap seconds are introduced on
 * specific UTC dates. If a minute-level fuzz around the times when leap seconds were introduced
 * can be tolerated then the choice of timescale is not relevant.
 *
 * Leap seconds were first introduced on 1 Jan 1972. Thus for date preceding the introduction,
 * 0 is returned. The last leap of 27 seconds was introduced on 1 January 2017, and the decision
 * has been made to not introduce further leap seconds as of 20 May 2027, leaving 37 seconds
 * for the foreseeable future.
 *
 * REFERENCES:
 *
 *  1. General Conference on Weights and Measures. "Resolution 5: On the adoption of a continuous UTC."
 *     Document presented at the 28th Meeting of the CGPM, Bureau International des Poids et Mesures (BIPM),
 *     Versailles, France, October 15, 2026. https://www.bipm.org/en/cgpm-2026/resolution-5.
 *
 * @param jd_utc    [day] Julian date (preferably UTC-based)
 * @return          The leap seconds for the given time, or -1 if the input date is NAN
 *                  (errno set to EINVAL).
 *
 * @since 1.8
 * @author Attila Kovacs
 *
 * @sa novas_lookup_leap()
 */
int novas_leap_for_jd(double jd_utc) {
  time_t ts;

  if(isnan(jd_utc))
    return novas_error(-1, EINVAL, "novas_leap_for_jd", "input Julian date is NAN");

  ts = (time_t) floor((jd_utc - NOVAS_JD_J2000) * NOVAS_DAY) + UNIX_UTC_J2000;

  return novas_lookup_leap(ts);
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
 *  - The IERS data service may encounter outages. Possible alternative URLs for the IERS EOP
 *    series are:
 *     - C01: https://hpiers.obspm.fr/iers/eop/eopc01/eopc01.1846-now
 *     - C04: https://hpiers.obspm.fr/iers/eop/eopc04/eopc04.1962-now
 *     - rapid service: https://maia.usno.navy.mil/ser7/finals2000A.daily.extended
 *
 *  - You may also download the relevant EOP series from IERS or from the alternative sources
 *    to store in local files, and then use this function to set `file://` type urls with absolute
 *    paths. This may be the preferred approach for critical / time critical applications.
 *
 * @param series      The EOP series identifier constant.
 * @param itrf_year   [yr] ITRF realization year. Needed only for precision at the few &mu;as
 *                    level, otherwise, you can set it to something recent, like 2020. It is
 *                    unused if the URL is NULL, or if the series is the leap second list. For
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
 * @sa novas_get_eop_url(), novas_fetch_eop(), novas_get_eop_itrf_year()
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
      prop_error(fn, novas_set_leap_list(NULL), 0);
      return 0;
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

  itrf_years[series] = url ? itrf_year : default_itrf_years[series];

  if(discard)
    free(discard);

  switch(series) {
    case EOP_LEAP_LIST: // it's a dummy entry point here, but here for completeness.
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
 * @sa novas_fetch_eop()
 */
void novas_reset_eop() {
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
 *     The IERS website also encounters outages occasionally (see novas_set_eop_url() for
 *     alternative sources.).
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

#ifndef WITHOUT_CURL
  static THREAD_LOCAL unsigned long version = -1;
  static THREAD_LOCAL novas_eop array[4];
  static THREAD_LOCAL double jd_from, jd_to = -1.0;
#endif

  if(isnan(jd))
    return novas_error(-1, EINVAL, fn, "input jd is NAN");

  if(!eop)
    return novas_error(-1, EINVAL, fn, "output eop is NULL");

  memset(eop, 0, sizeof(novas_eop));

  eop->jd = jd;
  eop->leap = novas_leap_for_jd(jd);

#ifdef WITHOUT_CURL
  return novas_error(-1, ENOSYS, fn, "SuperNOVAS was built without cURL support (WITHOUT_CURL).");
#else

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
 *     does not change over time. As such what works today, may not work the same in the future.
 *     The IERS website also encounters outages occasionally (see novas_set_eop_url() for
 *     alternative sources.).
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
 * whose dUT1 value is NAN, it will automatically fetch dUT1 values
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
