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

#include "include/hcblas.h"
#include "include/hcblaslib.h"
#include "gtest/gtest.h"
#include <cblas.h>
#include <cstdlib>
#include <hc_am.hpp>

unsigned int global_seed = 100;

TEST(hcblas_dscal, return_correct_dscal_Implementation_type_1) {
  hc::accelerator accl;
  hc::accelerator_view av = accl.get_default_view();
  Hcblaslibrary hc(&av);
  int N = 23;
  int incX = 1;
  __int64_t xOffset = 0;
  double alpha = 1;
  hcblasStatus status;
  __int64_t lenx = 1 + (N - 1) * abs(incX);
  double* X = (double*)calloc(lenx, sizeof(double));  // host input
  hc::accelerator_view accl_view = hc.currentAcclView;
  hc::accelerator acc = hc.currentAccl;
  /* Implementation type I - Inputs and Outputs are HCC double array containers
   */
  double* devX = hc::am_alloc(sizeof(double) * lenx, acc, 0);
  for (int i = 0; i < lenx; i++) {
    X[i] = rand_r(&global_seed) % 10;
  }
  double* devX1 = NULL;
  accl_view.copy(X, devX, lenx * sizeof(double));
  /* devX1 is NULL */
  status = hc.hcblas_dscal(accl_view, N, alpha, devX1, incX, xOffset);
  EXPECT_EQ(status, HCBLAS_INVALID);
  /* alpha is some scalar */
  status = hc.hcblas_dscal(accl_view, N, alpha, devX, incX, xOffset);
  EXPECT_EQ(status, HCBLAS_SUCCEEDS);
  /* alpha is 0 */
  alpha = 0;
  status = hc.hcblas_dscal(accl_view, N, alpha, devX, incX, xOffset);
  EXPECT_EQ(status, HCBLAS_SUCCEEDS);
  /* N is 0 */
  N = 0;
  status = hc.hcblas_dscal(accl_view, N, alpha, devX, incX, xOffset);
  EXPECT_EQ(status, HCBLAS_INVALID);
  /* incX is 0 */
  incX = 0;
  status = hc.hcblas_dscal(accl_view, N, alpha, devX, incX, xOffset);
  EXPECT_EQ(status, HCBLAS_INVALID);
}

TEST(hcblas_dscal, function_correct_dscal_Implementation_type_1) {
  hc::accelerator accl;
  hc::accelerator_view av = accl.get_default_view();
  Hcblaslibrary hc(&av);
  int N = 23;
  int incX = 1;
  __int64_t xOffset = 0;
  double alpha = 1;
  hcblasStatus status;
  __int64_t lenx = 1 + (N - 1) * abs(incX);
  double* X = (double*)calloc(lenx, sizeof(double));  // host input
  double* Xcblas = (double*)calloc(lenx, sizeof(double));
  hc::accelerator_view accl_view = hc.currentAcclView;
  hc::accelerator acc = hc.currentAccl;
  /* Implementation type I - Inputs and Outputs are HCC double array containers
   */
  double* devX = hc::am_alloc(sizeof(double) * lenx, acc, 0);
  for (int i = 0; i < lenx; i++) {
    X[i] = rand_r(&global_seed) % 10;
    Xcblas[i] = X[i];
  }
  accl_view.copy(X, devX, lenx * sizeof(double));
  status = hc.hcblas_dscal(accl_view, N, alpha, devX, incX, xOffset);
  EXPECT_EQ(status, HCBLAS_SUCCEEDS);
  accl_view.copy(devX, X, lenx * sizeof(double));
  cblas_dscal(N, alpha, Xcblas, incX);
  for (int i = 0; i < lenx; i++) {
    EXPECT_EQ(X[i], Xcblas[i]);
  }
}

TEST(hcblas_dscal, return_correct_dscal_Implementation_type_2) {
  hc::accelerator accl;
  hc::accelerator_view av = accl.get_default_view();
  Hcblaslibrary hc(&av);
  int N = 19;
  int incX = 1;
  int batchSize = 32;
  __int64_t xOffset = 0;
  double alpha = 1;
  hcblasStatus status;
  __int64_t X_batchOffset = N;
  __int64_t lenx = 1 + (N - 1) * abs(incX);
  double* Xbatch =
      (double*)calloc(lenx * batchSize, sizeof(double));  // host input
  hc::accelerator_view accl_view = hc.currentAcclView;
  hc::accelerator acc = hc.currentAccl;
  double* devXbatch = hc::am_alloc(sizeof(double) * lenx * batchSize, acc, 0);
  double* devX1batch = NULL;
  /* Implementation type II - Inputs and Outputs are HCC double array containers
   * with batch processing */
  for (int i = 0; i < lenx * batchSize; i++) {
    Xbatch[i] = rand_r(&global_seed) % 10;
  }
  accl_view.copy(Xbatch, devXbatch, lenx * batchSize * sizeof(double));
  /* x1 is NULL */
  status = hc.hcblas_dscal(accl_view, N, alpha, devX1batch, incX, xOffset,
                           X_batchOffset, batchSize);
  EXPECT_EQ(status, HCBLAS_INVALID);
  /* alpha is some scalar */
  status = hc.hcblas_dscal(accl_view, N, alpha, devXbatch, incX, xOffset,
                           X_batchOffset, batchSize);
  EXPECT_EQ(status, HCBLAS_SUCCEEDS);
  /* alpha is 0 */
  alpha = 0;
  status = hc.hcblas_dscal(accl_view, N, alpha, devXbatch, incX, xOffset,
                           X_batchOffset, batchSize);
  EXPECT_EQ(status, HCBLAS_SUCCEEDS);
  /* N is 0 */
  N = 0;
  status = hc.hcblas_dscal(accl_view, N, alpha, devXbatch, incX, xOffset,
                           X_batchOffset, batchSize);
  EXPECT_EQ(status, HCBLAS_INVALID);
  /* incX is 0 */
  incX = 0;
  status = hc.hcblas_dscal(accl_view, N, alpha, devXbatch, incX, xOffset,
                           X_batchOffset, batchSize);
  EXPECT_EQ(status, HCBLAS_INVALID);
}

TEST(hcblas_dscal, function_correct_dscal_Implementation_type_2) {
  hc::accelerator accl;
  hc::accelerator_view av = accl.get_default_view();
  Hcblaslibrary hc(&av);
  int N = 19;
  int incX = 1;
  int batchSize = 32;
  __int64_t xOffset = 0;
  double alpha = 1;
  hcblasStatus status;
  __int64_t X_batchOffset = N;
  __int64_t lenx = 1 + (N - 1) * abs(incX);
  double* Xbatch =
      (double*)calloc(lenx * batchSize, sizeof(double));  // host input
  double* Xcblasbatch = (double*)calloc(lenx * batchSize, sizeof(double));
  hc::accelerator_view accl_view = hc.currentAcclView;
  hc::accelerator acc = hc.currentAccl;
  double* devXbatch = hc::am_alloc(sizeof(double) * lenx * batchSize, acc, 0);
  /* Implementation type II - Inputs and Outputs are HCC double array containers
   * with batch processing */
  for (int i = 0; i < lenx * batchSize; i++) {
    Xbatch[i] = rand_r(&global_seed) % 10;
    Xcblasbatch[i] = Xbatch[i];
  }
  accl_view.copy(Xbatch, devXbatch, lenx * batchSize * sizeof(double));
  status = hc.hcblas_dscal(accl_view, N, alpha, devXbatch, incX, xOffset,
                           X_batchOffset, batchSize);
  EXPECT_EQ(status, HCBLAS_SUCCEEDS);
  accl_view.copy(devXbatch, Xbatch, lenx * batchSize * sizeof(double));
  for (int i = 0; i < batchSize; i++)
    cblas_dscal(N, alpha, Xcblasbatch + i * N, incX);
  for (int i = 0; i < lenx * batchSize; i++) {
    EXPECT_EQ(Xbatch[i], Xcblasbatch[i]);
  }
}

