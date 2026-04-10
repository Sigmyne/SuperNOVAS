/**
 * @file
 *
 * @date Created  on Apr 10, 2026
 * @author Attila Kovacs
 *
 *  SuperNOVAS module for managing computations on CUDA capable GPUs.
 */

#include <iostream>
#include <cerrno>

/// \cond PRIVATE
extern "C" {
#  define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#  include "novas.h"
#  include "nutation/novas-nut.h"
}

#include "cuda/novas-cuda.h"
/// \endcond

/// The maximum number of parallel streams to use for concurrent processing SuperNOVAS calls by
/// default.
#define DEFAULT_MAX_STREAMS   32

/// \cond PRIVATE
static lock_type parallel_lock;       ///< mutex lock to access parallel CUDA streams
static cuda_parallel *parallel;       ///< array of parallel CUDA streams
static int n_streams;                 ///< number of parallel CUDA streams in array
/// \endcond

/// \cond PROTECTED
cuda_parallel *novas_get_cuda_stream() {
  cuda_parallel *p = NULL;

  while(!p) {
    novas_lock(&parallel_lock);
    for(int i = 0; i < n_streams; i++) {
      p = &parallel[i];

      if(!p->is_used) {
        if(!p->is_initialized) {
          novas_init_lock(&p->mutex, 0);
          cudaStreamCreate(&p->stream);
          p->is_initialized = true;
        }
        novas_lock(&p->mutex);
        p->is_used = true;
      }
    }
    novas_unlock(&parallel_lock);
    sched_yield();
  }

  return p;
}
/// \endcond

/// \cond PROTECTED
void novas_release_cuda_stream(cuda_parallel *p) {
  p->is_used = false;
  novas_unlock(&p->mutex);
}
/// \endcond

/// \cond PROTECTED
static int novas_no_cuda_async() {
  if(n_streams > 0) {
    user_iau2000a = NULL;
    user_iau2000a = NULL;
  }

  for(int i = 0; i < n_streams; i++) {
    cuda_parallel *p = &parallel[i];
    if(p->is_initialized) {
      novas_lock(&p->mutex);
      p->is_used = false;
      p->is_initialized = false;
      cudaStreamDestroy(p->stream);
      novas_unlock(&p->mutex);
      novas_destroy_lock(&p->mutex);
    }
  }

  n_streams = 0;

  if(parallel) {
    std::free(parallel);
    parallel = NULL;
  }

  return 0;
}
/// \endcond

/// \cond PROTECTED
static void novas_cuda_cleanup() {
  novas_no_cuda();
  cudaDeviceReset();
}
/// \endcond

/**
 * Make SuperNOVAS use a CUDA capable GPU for accelerated intensive harmonic series calculations
 * such as for nutation. If there is no CUDA capable device on your computer, this call will
 * return an error, and processing will continue on the CPU (or a previously selected other
 * accelerator) as before.
 *
 * The number of streams affects the performance scaling. A single nutation calculations may use
 * up to 4 streams in parallel. An ELP2000 moon model calculation may use 6 at once. If you want
 * to such calculations in multiple CPU threads you will likkely benefit from using more streams.
 * At some point, however, you will utilize all SMs on the GPU at once. When that happens you will
 * no longer see benefit from further increasing the number of streams used by SuperNOVAS.
 * Therefore, you probably never need more streams that 2x (or a few times) the number of SMs on
 * your GPU.
 *
 * Alternatively, you may set the maximum number of streams to limit how much of the GPU's
 * resources SuperNOVAS may use at once (it cannot use more SMs than streams), for example if you
 * want to ensure your GPU is available for other tasks and processes also.
 *
 * @param max_streams   Specify the maximum number of concurrent CUDA streams that SuperNOVAS may
 *                      use, or set to &lt;=0 to use a default number of streams.
 * @return              0 if successful, or else -1 if there is no CUDA capable device found
 *                      (errno set to ENODEV),
 *                      or if the requested number of stream resources could not be allocated
 *                      (errno set by `malloc()`).
 *
 * @sa novas_no_cuda()
 */
int novas_use_cuda(int max_streams) {
  static const char *fn = "novas_use_cuda";
  static int initialized;

  int device;
  cudaError_t err = cudaGetDevice(&device);
  if(err != cudaSuccess)
    return novas_error(-1, ENODEV, fn, "No CUDA capable device found.");

  if(!initialized) {
#if THREAD_SAFE
    novas_init_lock(&parallel_lock, 0);
#endif
    atexit(novas_cuda_cleanup);
    initialized = 1;
  }

  if(max_streams <= 0)
    max_streams = DEFAULT_MAX_STREAMS;

  novas_lock(&parallel_lock);

  if(parallel)
    novas_no_cuda_async();

  parallel = (cuda_parallel *) std::calloc(max_streams, sizeof(cuda_parallel));
  if(!parallel) {
    novas_unlock(&parallel_lock);
    return novas_error(-1, errno, fn, "failed to alloc %d stream handles", max_streams);
  }

  n_streams = max_streams;

  user_iau2000a = novas_cuda_iau2000a;
  user_iau2000b = novas_cuda_iau2000b;

  novas_unlock(&parallel_lock);

  return 0;
}

/**
 * Stop using a CUDA capable GPU for accelerated SuperNOVAS calculations, and revert back to
 * calculating on the CPU exclusively. If CUDA was not being used, then this call will do
 * nothing, and won't affect the currently configured compute configuration.
 *
 * @return    0
 *
 * @sa novas_use_cuda()
 */
int novas_no_cuda() {
  novas_lock(&parallel_lock);
  int retval = novas_no_cuda_async();
  novas_unlock(&parallel_lock);
  return retval;
}


