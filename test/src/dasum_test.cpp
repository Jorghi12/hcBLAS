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
#include <cblas.h>
#include <cstdlib>
#include <hc_am.hpp>
#include <iostream>

unsigned int global_seed = 100;

int main(int argc, char **argv) {
  /* HCBLAS implementation */
  hc::accelerator accl;
  hc::accelerator_view av = accl.get_default_view();
  Hcblaslibrary hc(&av);
  if (argc < 3) {
    std::cout << "No sufficient commandline arguments specified"
              << "argc :" << argc << std::endl;
    return -1;
  }
  int N = atoi(argv[1]);
  int Imple_type = atoi(argv[2]);
  double asumhcblas;
  int incX = 1;
  __int64_t xOffset = 0;
  hcblasStatus status;
  int batchSize = 128;
  __int64_t X_batchOffset = N;
  if (N > 10000) batchSize = 50;
  /* CBLAS implementation */
  bool ispassed = 1;
  double asumcblas = 0.0;
  double *asumcblastemp = (double *)calloc(batchSize, sizeof(double));
  /* CBLAS implementation */
  __int64_t lenx = 1 + (N - 1) * abs(incX);
  std::vector<hc::accelerator> acc = hc::accelerator::get_all();
  hc::accelerator_view accl_view = (acc[1].get_default_view());

  /* Implementation type I - Inputs and Outputs are HCC double array containers
   */

  if (Imple_type == 1) {
    double *X = (double *)calloc(lenx, sizeof(double));
    double *devX = hc::am_alloc(sizeof(double) * lenx, acc[1], 0);
    for (int i = 0; i < lenx; i++) {
      X[i] = rand_r(&global_seed) % 10;
    }
    accl_view.copy(X, devX, lenx * sizeof(double));
    status = hc.hcblas_dasum(accl_view, N, devX, incX, xOffset, &asumhcblas);
    asumcblas = cblas_dasum(N, X, incX);
    if (asumhcblas != asumcblas) {
      ispassed = 0;
      std::cout << " HCDASUM " << asumhcblas
                << " does not match with CBLASDASUM " << asumcblas << std::endl;
    }
    if (!ispassed) std::cout << "TEST FAILED" << std::endl;
    if (status) std::cout << "TEST FAILED" << std::endl;
    free(X);
    hc::am_free(devX);
  }

  /* Implementation type II - Inputs and Outputs are HCC double array containers
     with batch processing */

  else {
    double *Xbatch = (double *)calloc(lenx * batchSize, sizeof(double));
    double *devXbatch =
        hc::am_alloc(sizeof(double) * lenx * batchSize, acc[1], 0);
    for (int i = 0; i < lenx * batchSize; i++) {
      Xbatch[i] = rand_r(&global_seed) % 10;
    }
    accl_view.copy(Xbatch, devXbatch, lenx * batchSize * sizeof(double));
    status = hc.hcblas_dasum(accl_view, N, devXbatch, incX, xOffset,
                             &asumhcblas, X_batchOffset, batchSize);
    for (int i = 0; i < batchSize; i++) {
      asumcblastemp[i] = cblas_dasum(N, Xbatch + i * N, incX);
      asumcblas += asumcblastemp[i];
    }
    if (asumhcblas != asumcblas) {
      ispassed = 0;
      std::cout << " HCDASUM " << asumhcblas
                << " does not match with CBLASDASUM " << asumcblas << std::endl;
    }
    if (!ispassed) std::cout << "TEST FAILED" << std::endl;
    if (status) std::cout << "TEST FAILED" << std::endl;
    free(Xbatch);
    hc::am_free(devXbatch);
  }
  return 0;
}
