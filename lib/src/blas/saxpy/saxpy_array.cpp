/*
Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "include/hcblaslib.h"
#include <hc.hpp>
#include <hc_math.hpp>

#define BLOCK_SIZE 256

void axpy_HC(hc::accelerator_view accl_view, __int64_t n, float alpha,
             const float *X, __int64_t xOffset, __int64_t incx, float *Y,
             __int64_t yOffset, __int64_t incy) {
  if (n <= 102400) {
    __int64_t size = (n + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);
    hc::extent<1> compute_domain(size);
    hc::parallel_for_each(accl_view, compute_domain.tile(BLOCK_SIZE), [=
    ](hc::tiled_index<1> tidx)[[hc]] {
      if (tidx.global[0] < n) {
        __int64_t Y_index = yOffset + tidx.global[0];
        Y[Y_index] = (hc::fast_math::isnan(static_cast<float>(Y[Y_index])) ||
                      hc::fast_math::isinf(static_cast<float>(Y[Y_index])))
                         ? 0
                         : Y[Y_index];
        Y[Y_index] += X[xOffset + tidx.global[0]] * alpha;
      }
    }) ;
  } else {
    int step_sz;

    if (n <= 409600) {
      step_sz = 3;
    } else if (n <= 921600) {
      step_sz = 5;
    } else if (n <= 1939526) {
      step_sz = 7;
    } else {
      step_sz = 15;
    }

    __int64_t size = (n / step_sz + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);
    __int64_t nBlocks = size / BLOCK_SIZE;
    hc::extent<1> compute_domain(size);
    hc::parallel_for_each(accl_view, compute_domain.tile(BLOCK_SIZE), [=
    ](hc::tiled_index<1> tidx)[[hc]] {
      if (tidx.tile[0] != nBlocks - 1) {
        for (int iter = 0; iter < step_sz; iter++) {
          __int64_t Y_index = yOffset + tidx.tile[0] * 256 * step_sz +
                              tidx.local[0] + iter * 256;
          Y[Y_index] = (hc::fast_math::isnan(static_cast<float>(Y[Y_index])) ||
                        hc::fast_math::isinf(static_cast<float>(Y[Y_index])))
                           ? 0
                           : Y[Y_index];
          Y[Y_index] += X[xOffset + tidx.tile[0] * 256 * step_sz +
                          tidx.local[0] + iter * 256] *
                        alpha;
        }
      } else {
        int n_iter = ((n - ((nBlocks - 1) * 256 * step_sz)) / 256) + 1;

        for (int iter = 0; iter < n_iter; iter++) {
          if (((nBlocks - 1) * 256 * step_sz + iter * 256 + tidx.local[0]) <
              n) {
            __int64_t Y_index = yOffset + tidx.tile[0] * 256 * step_sz +
                                tidx.local[0] + iter * 256;
            Y[Y_index] =
                (hc::fast_math::isnan(static_cast<float>(Y[Y_index])) ||
                 hc::fast_math::isinf(static_cast<float>(Y[Y_index])))
                    ? 0
                    : Y[Y_index];
            Y[Y_index] += X[xOffset + tidx.tile[0] * 256 * step_sz +
                            tidx.local[0] + iter * 256] *
                          alpha;
          }
        }
      }
    }) ;
  }
}

void axpy_HC(hc::accelerator_view accl_view, __int64_t n, float alpha,
             const float *X, __int64_t xOffset, __int64_t incx, float *Y,
             __int64_t yOffset, __int64_t incy, __int64_t X_batchOffset,
             __int64_t Y_batchOffset, int batchSize) {
  if (n <= 102400) {
    __int64_t size = (n + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);
    hc::extent<2> compute_domain(batchSize, size);
    hc::parallel_for_each(accl_view, compute_domain.tile(1, BLOCK_SIZE), [=
    ](hc::tiled_index<2> tidx)[[hc]] {
      int elt = tidx.tile[0];

      if (tidx.global[1] < n) {
        __int64_t Y_index = yOffset + Y_batchOffset * elt + tidx.global[1];
        Y[Y_index] = (hc::fast_math::isnan(static_cast<float>(Y[Y_index])) ||
                      hc::fast_math::isinf(static_cast<float>(Y[Y_index])))
                         ? 0
                         : Y[Y_index];
        Y[Y_index] += X[xOffset + X_batchOffset * elt + tidx.global[1]] * alpha;
      }
    }) ;
  } else {
    int step_sz;

    if (n <= 409600) {
      step_sz = 3;
    } else if (n <= 921600) {
      step_sz = 5;
    } else if (n <= 1939526) {
      step_sz = 7;
    } else {
      step_sz = 15;
    }

    __int64_t size = (n / step_sz + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);
    __int64_t nBlocks = size / BLOCK_SIZE;
    hc::extent<2> compute_domain(batchSize, size);
    hc::parallel_for_each(accl_view, compute_domain.tile(1, BLOCK_SIZE), [=
    ](hc::tiled_index<2> tidx)[[hc]] {
      int elt = tidx.tile[0];

      if (tidx.tile[1] != nBlocks - 1) {
        for (int iter = 0; iter < step_sz; iter++) {
          __int64_t Y_index = yOffset + Y_batchOffset * elt +
                              tidx.tile[1] * 256 * step_sz + tidx.local[1] +
                              iter * 256;
          Y[Y_index] = (hc::fast_math::isnan(static_cast<float>(Y[Y_index])) ||
                        hc::fast_math::isinf(static_cast<float>(Y[Y_index])))
                           ? 0
                           : Y[Y_index];
          Y[Y_index] +=
              X[xOffset + X_batchOffset * elt + tidx.tile[1] * 256 * step_sz +
                tidx.local[1] + iter * 256] *
              alpha;
        }
      } else {
        int n_iter = ((n - ((nBlocks - 1) * 256 * step_sz)) / 256) + 1;

        for (int iter = 0; iter < n_iter; iter++) {
          if (((nBlocks - 1) * 256 * step_sz + iter * 256 + tidx.local[1]) <
              n) {
            __int64_t Y_index = yOffset + Y_batchOffset * elt +
                                tidx.tile[1] * 256 * step_sz + tidx.local[1] +
                                iter * 256;
            Y[Y_index] =
                (hc::fast_math::isnan(static_cast<float>(Y[Y_index])) ||
                 hc::fast_math::isinf(static_cast<float>(Y[Y_index])))
                    ? 0
                    : Y[Y_index];
            Y[Y_index] +=
                X[xOffset + X_batchOffset * elt + tidx.tile[1] * 256 * step_sz +
                  tidx.local[1] + iter * 256] *
                alpha;
          }
        }
      }
    }) ;
  }
}

/* SAXPY - Type I : Inputs and outputs are device pointers */
hcblasStatus Hcblaslibrary::hcblas_saxpy(hc::accelerator_view accl_view,
                                         const int N, const float &alpha,
                                         const float *X, const int incX,
                                         float *Y, const int incY,
                                         const __int64_t xOffset,
                                         const __int64_t yOffset) {
  /*Check the conditions*/
  if (X == NULL || Y == NULL || N <= 0 || incX <= 0 || incY <= 0) {
    return HCBLAS_INVALID;
  }

  if (alpha == 0) {
    return HCBLAS_SUCCEEDS;
  }

  axpy_HC(accl_view, N, alpha, X, xOffset, incX, Y, yOffset, incY);
  return HCBLAS_SUCCEEDS;
}

/* SAXPY - Type II : Inputs and outputs are device pointers with batch
 * processing */
hcblasStatus Hcblaslibrary::hcblas_saxpy(
    hc::accelerator_view accl_view, const int N, const float &alpha,
    const float *X, const int incX, const __int64_t X_batchOffset, float *Y,
    const int incY, const __int64_t Y_batchOffset, const __int64_t xOffset,
    const __int64_t yOffset, const int batchSize) {
  /*Check the conditions*/
  if (X == NULL || Y == NULL || N <= 0 || incX <= 0 || incY <= 0) {
    return HCBLAS_INVALID;
  }

  if (alpha == 0) {
    return HCBLAS_SUCCEEDS;
  }

  axpy_HC(accl_view, N, alpha, X, xOffset, incX, Y, yOffset, incY,
          X_batchOffset, Y_batchOffset, batchSize);
  return HCBLAS_SUCCEEDS;
}

