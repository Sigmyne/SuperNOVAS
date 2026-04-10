/**
 * @date Created  on Apr 10, 2026
 * @author Attila Kovacs
 *
 *  Nutation calculations on a CUDA capable GPU.
 */

#include <cuda/cmath>
#include <cerrno>

/// \cond PRIVATE
#  include "cuda/novas-cuda.h"

extern "C" {
#  define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#  include "novas.h"
#  include "nutation/novas-nut.h"
}

#define NOVAS_TABLE __constant__      ///< keywords for declaring coefficient tables.

#include "nutation/iau2006a.tab.c"
#include "nutation/iau2006b.tab.c"

#include "cuda/shsum.cu"              ///< Reduction of partial sums

/// \endcond

__global__ void cuda_nutation_sums(const double *a, const nutation_terms *terms, int n_terms, double *out) {
  extern __shared__ double sums[];

  const int i = blockIdx.x * blockDim.x + threadIdx.x;

  // Initialize thread-sums
  const nutation_terms T = terms[i];
  double arg = 0.0;

  for(int k = T.from; k < T.to; k++)
    arg += T.n[k] * a[k];

  double s, c;
  sincos(arg, &s, &c);

  sums[threadIdx.x] = T.A * s + T.B * c;
  __syncthreads();

  // output block sums
  out[blockIdx.x] = cuda_reduce_shared_sums(sums) * 1e-8 * NOVAS_ARCSEC;
}

__global__ void cuda_nutation_t_sums(double t, const double *a, const nutation_terms *terms, int n_terms, double *out) {
  extern __shared__ double sums[];

  const int i = blockIdx.x * blockDim.x + threadIdx.x;

  // Initialize thread sums...
  sums[threadIdx.x] = 0.0;

  // Initialize thread-sums
  const nutation_terms T = terms[i];
  double arg = 0.0;

  for(int k = T.from; k < T.to; k++)
    arg += T.n[k] * a[k];

  double s, c;
  sincos(arg, &s, &c);

  sums[threadIdx.x] += t * (T.A * s + T.B * c);
  __syncthreads();

  // output block sums
  out[blockIdx.x] = cuda_reduce_shared_sums(sums) * 1e-8 * NOVAS_ARCSEC;
}

static double final_sum(const double *data, int n) {
  double sum = 0.0;
  while(--n >= 0)
    sum += data[n];
  return sum;
}


/// \cond PRIVATE
#define N0    128                           ///< Threads per block for main series
#define N1    64                            ///< Threads per block for time-dependent series

#define MAX_NA0   ( (IAU2000A_N_TERMS_A0 + N0 - 1) / N0 )  ///< Max partial sums for main dpsi series
#define MAX_NB0   ( (IAU2000A_N_TERMS_B0 + N0 - 1) / N0 )  ///< Max partial sums for main deps series
/// \endcond

static int cuda_iau2006_fp(double jd_tt_high, double jd_tt_low, int nA0, int nA1, int nB0, int nB1, double *dpsi, double *deps) {
  // Interval between fundamental epoch J2000.0 and given date.
  const double t = ((jd_tt_high - NOVAS_JD_J2000) + jd_tt_low) / JULIAN_CENTURY_DAYS;

  double a[14] = {0.0};
  cudaError_t err = cudaSuccess;
  int i;

  // Compute fundamental arguments from Simon et al. (1994), in radians.
  fund_args(t, (novas_delaunay_args *)(a));

  // Planetary longitudes, Mercury through Neptune, wrt mean dynamical
  // ecliptic and equinox of J2000
  // (Simon et al. 1994, 5.8.1-5.8.8).
  for(i = NOVAS_MERCURY; i <= NOVAS_NEPTUNE; i++)
    a[4 + i] = planet_lon(t, (enum novas_planet) i);

  // General precession in longitude (Simon et al. 1994), equivalent
  // to 5028.8200 arcsec/cy at J2000.
  a[13] = accum_prec(t);

  // Copy the argument array to the GPU
  double *da = NULL;
  err = cudaMalloc(&da, sizeof(a));

  if(err != cudaSuccess)
    err = cudaMemcpy(da, &a, sizeof(a), cudaMemcpyDefault);

  // Partial sum arrays returned by the GPU calls.
  // The sizes here relate to N0 and N1 above and the maxmimum number of nutation terms...
  double dpsi0[MAX_NA0] = {0.0}, deps0[MAX_NB0] = {0.0};
  double dpsi1 = 0.0, deps1 = 0.0;

  // Various resources we use per stream...
  cuda_parallel *p[4] = {NULL};     // CUDA streams to use
  double *dout[4] = {NULL};         // device output arrays
  double *out[4] = { dpsi0, deps0, &dpsi1, &deps1 };   // local output arrays
  const nutation_terms *T[4] = { A0, B0, A1, B1 };
  const int n[4] = { nA0, nB0, nA1, nB1 };
  const int M[4] = { N0, N0, N1, N1 };
  const int N[4] = { (nA0 + N0) / N0, (nB0 + N0) / N0, 1, 1 };

  // Launch processing in 4 parallel streams
  for(i = 0; i < 4; i++) {
    if(n[i]) {
      if(err != cudaSuccess)
        err = cudaMalloc(&dout[i], N[i] * sizeof(double));
      if(err != cudaSuccess) {
        p[i] = novas_get_cuda_stream();
        cuda_nutation_sums<<<N[i], M[i], M[i] * sizeof(double), p[i]->stream>>>(da, T[i], n[i], dout[i]);
        err = cudaMemcpyAsync(out[i], dout[i], N[i] * sizeof(double), cudaMemcpyDeviceToHost, p[i]->stream);
      }
    }
  }

  // Wait for the computations to finish
  for(i = 0; i < 4; i++) {
    if(p[i]) {
      cudaError e2 = cudaStreamSynchronize(p[i]->stream);
      if(e2 != cudaSuccess) {
        cudaFree(da);
        if(err == cudaSuccess)
          err = e2;
      }

      // Release stream resources.
      if(dout[i])
        cudaFree(&dout[i]);
      novas_release_cuda_stream(p[i]);

      // Calculate the final sums, while other streams may still process...
      if(N[i] > 1)
        out[i][0] = final_sum(out[i], N[i]);
    }
  }

  // Clean up
  if(da)
    cudaFree(da);

  // Return the final results.
  if(deps)
    *deps = deps0[0] + deps1;

  if(dpsi)
    *deps = dpsi0[0] + dpsi1;

  return err == cudaSuccess ? 0 : novas_error(-1, EAGAIN, "cuda_iau2006_fp", "CUDA error: %s", cudaGetErrorString(err));
}

/**
 * Calculates the IAU 2000A full nutation series using the CUDA capable GPU.
 *
 * REFERENCES:
 *
 *  1. IERS Conventions (2003), Chapter 5.
 *  2. Simon et al. (1994) Astronomy and Astrophysics 282, 663-683, esp. Sections 3.4-3.5.
 *  3. Capitaine, N. et al. (2003), Astronomy And Astrophysics 412, pp. 567-586.
 *  4. https://hpiers.obspm.fr/eop-pc/models/nutations/nut.html
 *
 * @param jd_tt_high  [day] High-order part of the Terrestrial Time (TT) based Julian date.
 *                    Typically it may be the integer part of a split date for the highest
 *                    precision, or the full date for normal (reduced) precision.
 * @param jd_tt_low   [day] Low-order part of the Terrestrial Time (TT) based Julian date.
 *                    Typically, it may be the fractional part of a split date for the highest
 *                    precision, or 0.0 for normal (reduced) precision.
 * @param[out] dpsi   [rad] &delta;&psi; Nutation (luni-solar + planetary) in longitude. It may be
 *                    NULL if not required.
 * @param[out] deps   [rad] &delta;&epsilon; Nutation (luni-solar + planetary) in obliquity. It
 *                    may be NULL if not required.
 * @return            0
 *
 * @sa iau2000a()
 */
int novas_cuda_iau2000a(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps) {
  prop_error("cuda_iau200a", cuda_iau2006_fp(jd_tt_high, jd_tt_low, (dpsi ? 1320 : 0), (dpsi ? 38 : 0), (deps ? 1037 : 0), (deps ? 19 : 0), dpsi, deps), 0);
  return 0;
}

/**
 * Calculates the IAU 2000B truncated nutation series using the CUDA capable GPU.
 *
 * REFERENCES:
 *
 *  1. McCarthy, D. and Luzum, B. (2003). "An Abridged Model of the
 *     Precession &amp; Nutation of the Celestial Pole," Celestial
 *     Mechanics and Dynamical Astronomy, Volume 85, Issue 1,
 *     Jan. 2003, p. 37. (IAU 2000B) IERS Conventions (2003), Chapter 5.
 *  2. Capitaine, N. et al. (2003), Astronomy And Astrophysics 412, pp. 567-586.
 *
 * @param jd_tt_high  [day] High-order part of the Terrestrial Time (TT) based Julian date.
 *                    Typically it may be the integer part of a split date for the highest
 *                    precision, or the full date for normal (reduced) precision.
 * @param jd_tt_low   [day] Low-order part of the Terrestrial Time (TT) based Julian date.
 *                    Typically it may be the fractional part of a split date for the highest
 *                    precision, or 0.0 for normal (reduced) precision.
 * @param[out] dpsi   [rad] &delta;&psi; Nutation (luni-solar + planetary) in longitude, It may be
 *                    NULL if not required.
 * @param[out] deps   [rad] &delta;&epsilon; Nutation (luni-solar + planetary) in obliquity. It
 *                    may be NULL if not required.
 * @return            0
 *
 * @sa iau2000b()
 */
int novas_cuda_iau2000b(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps) {
  prop_error("cuda_iau2000b", cuda_iau2006_fp(jd_tt_high, jd_tt_low, (dpsi ? 98 : 0), (dpsi ? 4 : 0), (deps ? 55 : 0), (deps ? 2 : 0), dpsi, deps), 0);
  return 0;
}



