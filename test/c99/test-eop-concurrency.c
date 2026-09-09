#include <math.h>
#include <pthread.h>
#include <stdio.h>

#include "novas.h"

#define THREAD_COUNT 8
#define REQUEST_COUNT 32

typedef struct {
  novas_eop expected[REQUEST_COUNT];
  int id;
  int status;
} worker_state;

static pthread_mutex_t gate_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gate_condition = PTHREAD_COND_INITIALIZER;
static int ready_count;
static int is_released;

static double request_jd(int worker, int request) {
  // Alternate sparse C01, dense C01 without and with UT1/LOD, and C04.
  static const double dates[] = {2400000.0, 2420000.0, 2435800.0, 2440000.0};
  // Keep requests outside each thread's previous interpolation interval.
  return dates[(worker + request) % 4] + 40.0 * worker + 80.0 * (request / 4);
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
    if(novas_fetch_eop(jd, 0, &eop) || !is_same_eop(&eop, &state->expected[i])) {
      fprintf(stderr, "Worker %d request %d (JD %.1f): EOP fetch or comparison failed\n", state->id, i, jd);
      state->status = 1;
      return NULL;
    }
  }

  return NULL;
}

int main(void) {
  pthread_t threads[THREAD_COUNT];
  worker_state states[THREAD_COUNT] = {{0}};
  int i;
  int status = 0;

  if(configure_eop())
    return 1;

  for(i = 0; i < THREAD_COUNT; i++) {
    int request;
    states[i].id = i;
    for(request = 0; request < REQUEST_COUNT; request++) {
      double jd = request_jd(i, request);
      if(novas_fetch_eop(jd, 0, &states[i].expected[request])) {
        fprintf(stderr, "Reference worker %d request %d (JD %.1f): EOP fetch failed\n", i, request, jd);
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
