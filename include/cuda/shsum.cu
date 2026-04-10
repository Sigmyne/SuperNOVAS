/**
 * @date Created  on Apr 10, 2026
 * @author Attila Kovacs
 *
 *  CUDA device kernel for reducing partial sums
 */

#include <cuda_runtime.h>

/**
 * Reduces partial sum from threads in a block stored in shared memory.
 *
 * @param in    array containing per-thread partial sums.
 * @return      the reduced sum across the block of threads.
 */
__device__ double cuda_reduce_shared_sums(double *in) {
  // always sum values of similar magnitude first...

#pragma unroll
  for(int stride = (blockDim.x >> 1); stride > 0; stride >>= 1) {
    in[threadIdx.x] += in[threadIdx.x + stride];
  }

  __syncthreads();

  return in[0];
}

