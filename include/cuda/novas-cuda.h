/**
 * @file
 *
 * @date Created  on Apr 10, 2026
 * @author Attila Kovacs
 */

#ifndef NOVAS_CUDA_H_
#define NOVAS_CUDA_H_

#include <cuda_runtime.h>

#include "novas-mutex.h"

/// \cond PROTECTED

/**
 * Structure to manage a parallel CUDA stream.
 */
typedef struct {
  bool is_initialized;    ///< (boolean) whether the structure has been initialized
  bool is_used;           ///< (boolean) peek variable whether this stream resource is currently being used
  cudaStream_t stream;    ///< Associated CUDA stream handle
  lock_type mutex;        ///< A mutex providing exclusive access to this stream's resources
} cuda_parallel;
/// \endcond


int novas_use_cuda(int max_streams);
int novas_no_cuda();

/// \cond PROTECTED
cuda_parallel *novas_get_cuda_stream();
void novas_release_cuda_stream(cuda_parallel *p);

int novas_cuda_iau2000a(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps);
int novas_cuda_iau2000b(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps);
/// \endcond



#endif /* NOVAS_CUDA_H_ */
