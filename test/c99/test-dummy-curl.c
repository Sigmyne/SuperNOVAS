/**
 * @file
 *
 * @date Created  on May 15, 2026
 * @author Attila Kovacs
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <curl/curl.h>

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#undef curl_easy_setopt

typedef size_t (*write_function)(const char *ptr, size_t size, size_t nmemb, void *userdata);

typedef struct {
  char *buf;          ///< Allocated byte buffer
  size_t capacity;    ///< Capacity of byte buffer
  size_t size;        ///< Number of bytes filled
} download_buffer;

static CURL *curl;

static write_function write_call;
static download_buffer *pData;
static char *src;
static size_t srclen, dstlen;


CURL_EXTERN CURL* curl_easy_init(void) {
  return curl;
}

CURL_EXTERN void curl_easy_cleanup(CURL *curl) {

}

CURLcode curl_easy_setopt(CURL *handle, CURLoption option, ...) {
  va_list varg;

  va_start(varg, option);

  if(option == CURLOPT_WRITEFUNCTION)
    write_call = va_arg(varg, write_function);
  else if(option == CURLOPT_WRITEDATA) {
    pData = va_arg(varg, download_buffer*);
    if(dstlen) pData->capacity = dstlen;
  }

  va_end(varg);

  return CURLE_OK;
}

CURL_EXTERN CURLcode curl_easy_perform(CURL *curl) {
  int n = write_call(src, 1, srclen, pData);
  if(n < 0)
    return CURLE_WRITE_ERROR;
  if(n != srclen)
    return CURLE_PARTIAL_FILE;
  return CURLE_OK;
}

static int check(const char *func, int exp, int error) {
  if(error != exp) {
    fprintf(stderr, "ERROR! %s: expected %d, got %d\n", func, exp, error);
    return 1;
  }
  return 0;
}

static int test_curl_init() {
  int n = 0;
  novas_eop eop = {};

  curl = NULL;
  if(check("curl_init:lookup_leap", NOVAS_INVALID_LEAP, novas_lookup_leap(0L))) n++;
  if(check("curl_init:fetch_eop", -1, novas_fetch_eop(NOVAS_JD_J2000, 0, &eop))) n++;

  return n;
}

static int test_curl_write_overflow() {
  char buf[8192];
  novas_eop eop = {};

  curl = (CURL *) 1L;

  src = buf;
  srclen = sizeof(buf);

  if(check("curl_write_overflow", -1, novas_fetch_eop(NOVAS_JD_J2000, 0, &eop))) return 1;
  return 0;
}

int main(int argc, const char *argv[]) {
  int n = 0;

  if(test_curl_init()) n++;
  if(test_curl_write_overflow()) n++;

  if(n) fprintf(stderr, " -- FAILED %d tests\n", n);
  else fprintf(stderr, " -- OK\n");

  return n;
}


