#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "novas.h"

#define THREAD_COUNT 8
#define REQUEST_COUNT 32

enum test_mode { MIXED, SAME_DATE, CACHE, INVALID_DATE, MISSING_FILE, MALFORMED_FILE };
static enum test_mode mode;

typedef struct {
  novas_eop expected[REQUEST_COUNT];
  int expected_status[REQUEST_COUNT];
  int expected_errno[REQUEST_COUNT];
  int id;
  int status;
} worker_state;

static pthread_mutex_t gate_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gate_condition = PTHREAD_COND_INITIALIZER;
static int ready_count;
static int is_released;

static double request_jd(int worker, int request) {
  if(mode == SAME_DATE)
    return 2440000.25;
  if(mode == CACHE)
    request /= 2;
  if(mode == INVALID_DATE && request % 2 == 0)
    return 2300000.0;
  // Alternate sparse C01, dense C01 without and with UT1/LOD, and C04.
  // Include the rapid series from the local file.
  static const double dates[] = {2400000.0, 2420000.0, 2435800.0, 2440000.0, 2459000.0};
  // Keep requests outside each thread's previous interpolation interval.
  // CACHE repeats each request once to test the thread-local cache.
  return dates[(worker + request) % 5] + 40.0 * worker + 80.0 * (request / 5);
}

static int is_same_value(double actual, double expected) {
  return actual == expected || (isnan(actual) && isnan(expected));
}

static int is_same_eop(const novas_eop *actual, const novas_eop *expected) {
  return actual->series == expected->series && actual->leap == expected->leap
        && is_same_value(actual->jd, expected->jd)
        && is_same_value(actual->xp, expected->xp) && is_same_value(actual->xp_err, expected->xp_err)
        && is_same_value(actual->yp, expected->yp) && is_same_value(actual->yp_err, expected->yp_err)
        && is_same_value(actual->dut1, expected->dut1) && is_same_value(actual->dut1_err, expected->dut1_err)
        && is_same_value(actual->lod, expected->lod) && is_same_value(actual->lod_err, expected->lod_err);
}

static int configure_eop(void) {
  if(novas_set_leap_list(RESOURCES "/leap-seconds.list"))
    return 1;
  if(novas_set_eop_url(EOP_C01_IAU2000, 1900, "file://" RESOURCES "/EOP_C01_IAU2000_1846-now.txt"))
    return 1;
  if(novas_set_eop_url(EOP_RAPID_IAU2000, 2020, "file://" RESOURCES "/finals.all.iau2000.txt"))
    return 1;
  if(mode == MISSING_FILE || mode == MALFORMED_FILE) {
    // A C01 record has an invalid line length for the C04 parser.
    const char *url = mode == MISSING_FILE ? "file://" RESOURCES "/missing-eop-file.txt"
          : "file://" RESOURCES "/C01-bad.txt";
    int expected_error = mode == MISSING_FILE ? EAGAIN : EBADMSG;
    // The URL remains selected when its initial checkout fails.
    int status = novas_set_eop_url(EOP_C04_IAU2000_0UTC, 2020, url);
    int error = errno;
    if(status != -1 || error != expected_error) {
      fprintf(stderr, "EOP checkout: status %d/-1, errno %d/%d\n", status, error, expected_error);
      return 1;
    }
    return 0;
  }
  return novas_set_eop_url(EOP_C04_IAU2000_0UTC, 2020, "file://" RESOURCES "/EOP_20u24_C04_one_file_1962-now.txt");
}

static void *fetch_eop(void *arg) {
  worker_state *state = (worker_state *) arg;
  int i;

  pthread_mutex_lock(&gate_mutex);
  ready_count++;
  pthread_cond_broadcast(&gate_condition);
  while(is_released == 0) {
    pthread_cond_wait(&gate_condition, &gate_mutex);
  }
  pthread_mutex_unlock(&gate_mutex);

  for(i = 0; i < REQUEST_COUNT; i++) {
    novas_eop eop;
    double jd = request_jd(state->id, i);
    int status;
    int error;
    errno = 0;
    status = novas_fetch_eop(jd, 0, &eop);
    error = errno;
    if(status != state->expected_status[i]
          || (status == 0 && !is_same_eop(&eop, &state->expected[i]))
          || (status != 0 && error != state->expected_errno[i])) {
      fprintf(stderr, "Worker %d request %d (JD %.1f): EOP fetch or comparison failed; status %d/%d, errno %d/%d\n",
            state->id, i, jd, status, state->expected_status[i], error, state->expected_errno[i]);
      state->status = 1;
    }
  }

  return NULL;
}

int main(int argc, char **argv) {
  pthread_t threads[THREAD_COUNT];
  worker_state states[THREAD_COUNT] = {{0}};
  int i;
  int status = 0;

  if(argc == 2) {
    const char *names[] = {"mixed", "same-date", "cache", "invalid-date", "missing-file", "malformed-file"};
    for(i = 0; i < 6 && strcmp(argv[1], names[i]); i++) {
    }
    if(i == 6)
      return 1;
    mode = (enum test_mode) i;
  }
  if(argc > 2 || configure_eop())
    return 1;

  for(i = 0; i < THREAD_COUNT; i++) {
    int request;
    states[i].id = i;
    for(request = 0; request < REQUEST_COUNT; request++) {
      double jd = request_jd(i, request);
      int expected_error = 0;
      if(jd == 2300000.0)
        expected_error = ERANGE;
      if(jd >= 2440000.0 && jd < 2450000.0) {
        if(mode == MISSING_FILE)
          expected_error = EAGAIN;
        if(mode == MALFORMED_FILE)
          expected_error = EBADMSG;
      }
      errno = 0;
      states[i].expected_status[request] = novas_fetch_eop(jd, 0, &states[i].expected[request]);
      states[i].expected_errno[request] = errno;
      if(states[i].expected_status[request] != (expected_error ? -1 : 0)
            || (expected_error && errno != expected_error)) {
        fprintf(stderr, "Reference worker %d request %d (JD %.1f): unexpected status or errno\n", i, request, jd);
        return 1;
      }
    }
  }

  novas_reset_eop();
  if(configure_eop())
    return 1;

  for(i = 0; i < THREAD_COUNT; i++) {
    if(pthread_create(&threads[i], NULL, fetch_eop, &states[i]))
      return 1;
  }

  pthread_mutex_lock(&gate_mutex);
  while(ready_count < THREAD_COUNT) {
    pthread_cond_wait(&gate_condition, &gate_mutex);
  }
  is_released = 1;
  pthread_cond_broadcast(&gate_condition);
  pthread_mutex_unlock(&gate_mutex);

  for(i = 0; i < THREAD_COUNT; i++) {
    pthread_join(threads[i], NULL);
    status |= states[i].status;
  }

  novas_reset_eop();
  return status;
}
