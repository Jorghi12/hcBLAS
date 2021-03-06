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
#include "include/hipblas.h"
#include "include/hcblas.h"

#ifdef __cplusplus
extern "C" {
#endif

hipblasStatus_t hipblasCreate(hipblasHandle_t *handle) {
  int deviceId;
  hipError_t err;
  hipblasStatus_t retval = HIPBLAS_STATUS_SUCCESS;

  if (handle == nullptr) {
    handle = new hipblasHandle_t();
  }

  err = hipGetDevice(&deviceId);
  if (err == hipSuccess) {
    hc::accelerator_view *av;
    err = hipHccGetAcceleratorView(hipStreamDefault, &av);
    if (err == hipSuccess) {
      retval = hipHCBLASStatusToHIPStatus(hcblasCreate(&*handle, av));
    } else {
      retval = HIPBLAS_STATUS_INVALID_VALUE;
    }
  }
  return retval;
}

hipblasStatus_t hipblasSetStream(hipblasHandle_t handle, hipStream_t streamId) {
  if (handle == nullptr) {
    return HIPBLAS_STATUS_NOT_INITIALIZED;
  }
  hc::accelerator_view *pAcclView;
  hipError_t err = hipHccGetAcceleratorView(streamId, &pAcclView);
  if (err != hipSuccess) {
    return HIPBLAS_STATUS_NOT_INITIALIZED;
  }
  return hipHCBLASStatusToHIPStatus(
      hcblasSetAcclView(handle, *pAcclView, static_cast<void *>(streamId)));
}

hipblasStatus_t hipblasGetStream(hipblasHandle_t handle,
                                 hipStream_t *streamId) {
  if (handle == nullptr) {
    return HIPBLAS_STATUS_NOT_INITIALIZED;
  }
  hc::accelerator_view *pAcclView = NULL;
  return hipHCBLASStatusToHIPStatus(
      hcblasGetAcclView(handle, pAcclView,
                        reinterpret_cast<void **>(streamId)));
}

hcblasOperation_t hipOperationToHCCOperation(hipblasOperation_t op) {
  switch (op) {
    case HIPBLAS_OP_N:
      return HCBLAS_OP_N;

    case HIPBLAS_OP_T:
      return HCBLAS_OP_T;

    case HIPBLAS_OP_C:
      return HCBLAS_OP_C;

    default:
      throw "Non existent OP";
  }
}

hipblasOperation_t HCCOperationToHIPOperation(hcblasOperation_t op) {
  switch (op) {
    case HCBLAS_OP_N:
      return HIPBLAS_OP_N;

    case HCBLAS_OP_T:
      return HIPBLAS_OP_T;

    case HCBLAS_OP_C:
      return HIPBLAS_OP_C;

    default:
      throw "Non existent OP";
  }
}

hipblasStatus_t hipHCBLASStatusToHIPStatus(hcblasStatus_t hcStatus) {
  switch (hcStatus) {
    case HCBLAS_STATUS_SUCCESS:
      return HIPBLAS_STATUS_SUCCESS;
    case HCBLAS_STATUS_NOT_INITIALIZED:
      return HIPBLAS_STATUS_NOT_INITIALIZED;
    case HCBLAS_STATUS_ALLOC_FAILED:
      return HIPBLAS_STATUS_ALLOC_FAILED;
    case HCBLAS_STATUS_INVALID_VALUE:
      return HIPBLAS_STATUS_INVALID_VALUE;
    case HCBLAS_STATUS_MAPPING_ERROR:
      return HIPBLAS_STATUS_MAPPING_ERROR;
    case HCBLAS_STATUS_EXECUTION_FAILED:
      return HIPBLAS_STATUS_EXECUTION_FAILED;
    case HCBLAS_STATUS_INTERNAL_ERROR:
      return HIPBLAS_STATUS_INTERNAL_ERROR;
    default:
      throw "Unimplemented status";
  }
}

hipblasStatus_t hipblasDestroy(hipblasHandle_t handle) {
  return hipHCBLASStatusToHIPStatus(hcblasDestroy(&handle));
}

hipblasStatus_t hipblasSetVector(int n, int elemSize, const void *x, int incx,
                                 void *y, int incy) {
  hipblasHandle_t handle = NULL;
  hipblasStatus_t status = HIPBLAS_STATUS_INTERNAL_ERROR;
  status = hipblasCreate(&handle);
  if (status == HIPBLAS_STATUS_SUCCESS) {
    status = hipHCBLASStatusToHIPStatus(
        hcblasSetVector(handle, n, elemSize, x, incx, y, incy));
  }
  hipblasDestroy(handle);
  return status;
}

hipblasStatus_t hipblasGetVector(int n, int elemSize, const void *x, int incx,
                                 void *y, int incy) {
  hipblasHandle_t handle = NULL;
  hipblasStatus_t status = HIPBLAS_STATUS_INTERNAL_ERROR;
  status = hipblasCreate(&handle);
  if (status == HIPBLAS_STATUS_SUCCESS) {
    status = hipHCBLASStatusToHIPStatus(hcblasGetVector(
        handle, n, elemSize, x, incx, y, incy));  // HGSOS no need for handle
  }
  hipblasDestroy(handle);
  return status;
}

hipblasStatus_t hipblasSetMatrix(int rows, int cols, int elemSize,
                                 const void *A, int lda, void *B, int ldb) {
  hipblasHandle_t handle = NULL;
  hipblasStatus_t status = HIPBLAS_STATUS_INTERNAL_ERROR;
  status = hipblasCreate(&handle);
  if (status == HIPBLAS_STATUS_SUCCESS) {
    status = hipHCBLASStatusToHIPStatus(
        hcblasSetMatrix(handle, rows, cols, elemSize, A, lda, B, ldb));
  }
  hipblasDestroy(handle);
  return status;
}

hipblasStatus_t hipblasGetMatrix(int rows, int cols, int elemSize,
                                 const void *A, int lda, void *B, int ldb) {
  hipblasHandle_t handle = NULL;
  hipblasStatus_t status = HIPBLAS_STATUS_INTERNAL_ERROR;
  status = hipblasCreate(&handle);
  if (status == HIPBLAS_STATUS_SUCCESS) {
    status = hipHCBLASStatusToHIPStatus(
        hcblasGetMatrix(handle, rows, cols, elemSize, A, lda, B, ldb));
  }
  hipblasDestroy(handle);
  return status;
}

hipblasStatus_t hipblasSasum(hipblasHandle_t handle, int n, const float *x,
                             int incx, float *result) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSasum(handle, n, const_cast<float *>(x), incx, result));
}

hipblasStatus_t hipblasDasum(hipblasHandle_t handle, int n, const double *x,
                             int incx, double *result) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDasum(handle, n, const_cast<double *>(x), incx, result));
}

hipblasStatus_t hipblasSasumBatched(hipblasHandle_t handle, int n, float *x,
                                    int incx, float *result, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSasumBatched(handle, n, x, incx, result, batchCount));
}

hipblasStatus_t hipblasDasumBatched(hipblasHandle_t handle, int n, double *x,
                                    int incx, double *result, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDasumBatched(handle, n, x, incx, result, batchCount));
}

hipblasStatus_t hipblasSaxpy(hipblasHandle_t handle, int n, const float *alpha,
                             const float *x, int incx, float *y, int incy) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSaxpy(handle, n, alpha, x, incx, y, incy));
}

hipblasStatus_t hipblasDaxpy(hipblasHandle_t handle, int n, const double *alpha,
                             const double *x, int incx, double *y, int incy) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDaxpy(handle, n, alpha, x, incx, y, incy));
}

hipblasStatus_t hipblasSaxpyBatched(hipblasHandle_t handle, int n,
                                    const float *alpha, const float *x,
                                    int incx, float *y, int incy,
                                    int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSaxpyBatched(handle, n, alpha, x, incx, y, incy, batchCount));
}

hipblasStatus_t hipblasScopy(hipblasHandle_t handle, int n, const float *x,
                             int incx, float *y, int incy) {
  return hipHCBLASStatusToHIPStatus(hcblasScopy(handle, n, x, incx, y, incy));
}

hipblasStatus_t hipblasDcopy(hipblasHandle_t handle, int n, const double *x,
                             int incx, double *y, int incy) {
  return hipHCBLASStatusToHIPStatus(hcblasDcopy(handle, n, x, incx, y, incy));
}

hipblasStatus_t hipblasScopyBatched(hipblasHandle_t handle, int n,
                                    const float *x, int incx, float *y,
                                    int incy, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasScopyBatched(handle, n, x, incx, y, incy, batchCount));
}

hipblasStatus_t hipblasDcopyBatched(hipblasHandle_t handle, int n,
                                    const double *x, int incx, double *y,
                                    int incy, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDcopyBatched(handle, n, x, incx, y, incy, batchCount));
}

hipblasStatus_t hipblasSdot(hipblasHandle_t handle, int n, const float *x,
                            int incx, const float *y, int incy, float *result) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSdot(handle, n, x, incx, y, incy, result));
}

hipblasStatus_t hipblasDdot(hipblasHandle_t handle, int n, const double *x,
                            int incx, const double *y, int incy,
                            double *result) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDdot(handle, n, x, incx, y, incy, result));
}

hipblasStatus_t hipblasSdotBatched(hipblasHandle_t handle, int n,
                                   const float *x, int incx, const float *y,
                                   int incy, float *result, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSdotBatched(handle, n, x, incx, y, incy, result, batchCount));
}

hipblasStatus_t hipblasDdotBatched(hipblasHandle_t handle, int n,
                                   const double *x, int incx, const double *y,
                                   int incy, double *result, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDdotBatched(handle, n, x, incx, y, incy, result, batchCount));
}

hipblasStatus_t hipblasSscal(hipblasHandle_t handle, int n, const float *alpha,
                             float *x, int incx) {
  return hipHCBLASStatusToHIPStatus(hcblasSscal(handle, n, alpha, x, incx));
}

hipblasStatus_t hipblasDscal(hipblasHandle_t handle, int n, const double *alpha,
                             double *x, int incx) {
  return hipHCBLASStatusToHIPStatus(hcblasDscal(handle, n, alpha, x, incx));
}

hipblasStatus_t hipblasCscal(hipblasHandle_t handle, int n,
                             const hipComplex *alpha, hipComplex *x, int incx) {
  return hipHCBLASStatusToHIPStatus(
      hcblasCscal(handle, n, reinterpret_cast<const hcComplex *>(alpha),
                  reinterpret_cast<hcComplex *>(x), incx));
}

hipblasStatus_t hipblasZscal(hipblasHandle_t handle, int n,
                             const hipDoubleComplex *alpha, hipDoubleComplex *x,
                             int incx) {
  return hipHCBLASStatusToHIPStatus(hcblasZscal(
      handle, n, (const hcDoubleComplex *)alpha,
      reinterpret_cast<hcDoubleComplex *>(x), incx));
}

hipblasStatus_t hipblasCsscal(hipblasHandle_t handle, int n, const float *alpha,
                              hipComplex *x, int incx) {
  return hipHCBLASStatusToHIPStatus(
      hcblasCsscal(handle, n, alpha, reinterpret_cast<hcComplex *>(x), incx));
}

hipblasStatus_t hipblasZdscal(hipblasHandle_t handle, int n,
                              const double *alpha, hipDoubleComplex *x,
                              int incx) {
  return hipHCBLASStatusToHIPStatus(
      hcblasZdscal(handle, n, alpha, reinterpret_cast<hcDoubleComplex *>(x),
                   incx));
}

hipblasStatus_t hipblasSscalBatched(hipblasHandle_t handle, int n,
                                    const float *alpha, float *x, int incx,
                                    int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSscalBatched(handle, n, alpha, x, incx, batchCount));
}

hipblasStatus_t hipblasDscalBatched(hipblasHandle_t handle, int n,
                                    const double *alpha, double *x, int incx,
                                    int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDscalBatched(handle, n, alpha, x, incx, batchCount));
}

hipblasStatus_t hipblasCscalBatched(hipblasHandle_t handle, int n,
                                    const hipComplex *alpha, hipComplex *x,
                                    int incx, int batchCount) {
  return hipHCBLASStatusToHIPStatus(hcblasCscalBatched(
      handle, n, reinterpret_cast<const hcComplex *>(alpha),
      reinterpret_cast<hcComplex *>(x), incx, batchCount));
}

hipblasStatus_t hipblasZscalBatched(hipblasHandle_t handle, int n,
                                    const hipDoubleComplex *alpha,
                                    hipDoubleComplex *x, int incx,
                                    int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasZscalBatched(handle, n, (const hcDoubleComplex *)alpha,
                         reinterpret_cast<hcDoubleComplex *>(x), incx,
                         batchCount));
}

hipblasStatus_t hipblasCsscalBatched(hipblasHandle_t handle, int n,
                                     const float *alpha, hipComplex *x,
                                     int incx, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasCsscalBatched(handle, n, alpha, reinterpret_cast<hcComplex *>(x),
                          incx, batchCount));
}

hipblasStatus_t hipblasZdscalBatched(hipblasHandle_t handle, int n,
                                     const double *alpha, hipDoubleComplex *x,
                                     int incx, int batchCount) {
  return hipHCBLASStatusToHIPStatus(hcblasZdscalBatched(
      handle, n, alpha, reinterpret_cast<hcDoubleComplex *>(x), incx,
      batchCount));
}

hipblasStatus_t hipblasSgemv(hipblasHandle_t handle, hipblasOperation_t trans,
                             int m, int n, const float *alpha, const float *A,
                             int lda, const float *x, int incx,
                             const float *beta, float *y, int incy) {
  // TODO(Neelakandan): Remove const_cast
  return hipHCBLASStatusToHIPStatus(
      hcblasSgemv(handle, hipOperationToHCCOperation(trans), m, n, alpha,
                  const_cast<float *>(A), lda, const_cast<float *>(x), incx,
                  beta, y, incy));
}

hipblasStatus_t hipblasDgemv(hipblasHandle_t handle, hipblasOperation_t trans,
                             int m, int n, const double *alpha, const double *A,
                             int lda, const double *x, int incx,
                             const double *beta, double *y, int incy) {
  // TODO(Neelakandan): Remove const_cast
  return hipHCBLASStatusToHIPStatus(
      hcblasDgemv(handle, hipOperationToHCCOperation(trans), m, n, alpha,
                  const_cast<double *>(A), lda, const_cast<double *>(x), incx,
                  beta, y, incy));
}

hipblasStatus_t hipblasSgemvBatched(hipblasHandle_t handle,
                                    hipblasOperation_t trans, int m, int n,
                                    const float *alpha, float *A, int lda,
                                    float *x, int incx, const float *beta,
                                    float *y, int incy, int batchCount) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSgemvBatched(handle, hipOperationToHCCOperation(trans), m, n, alpha,
                         A, lda, x, incx, beta, y, incy, batchCount));
}

hipblasStatus_t hipblasSger(hipblasHandle_t handle, int m, int n,
                            const float *alpha, const float *x, int incx,
                            const float *y, int incy, float *A, int lda) {
  return hipHCBLASStatusToHIPStatus(
      hcblasSger(handle, m, n, alpha, x, incx, y, incy, A, lda));
}

hipblasStatus_t hipblasDger(hipblasHandle_t handle, int m, int n,
                            const double *alpha, const double *x, int incx,
                            const double *y, int incy, double *A, int lda) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDger(handle, m, n, alpha, x, incx, y, incy, A, lda));
}

hipblasStatus_t hipblasSgerBatched(hipblasHandle_t handle, int m, int n,
                                   const float *alpha, const float *x, int incx,
                                   const float *y, int incy, float *A, int lda,
                                   int batchCount) {
  return hipHCBLASStatusToHIPStatus(hcblasSgerBatched(
      handle, m, n, alpha, x, incx, y, incy, A, lda, batchCount));
}

hipblasStatus_t hipblasDgerBatched(hipblasHandle_t handle, int m, int n,
                                   const double *alpha, const double *x,
                                   int incx, const double *y, int incy,
                                   double *A, int lda, int batchCount) {
  return hipHCBLASStatusToHIPStatus(hcblasDgerBatched(
      handle, m, n, alpha, x, incx, y, incy, A, lda, batchCount));
}

hipblasStatus_t hipblasSgemm(hipblasHandle_t handle, hipblasOperation_t transa,
                             hipblasOperation_t transb, int m, int n, int k,
                             const float *alpha, const float *A, int lda,
                             const float *B, int ldb, const float *beta,
                             float *C, int ldc) {
  // TODO(Neelakandan): Remove const_cast
  return hipHCBLASStatusToHIPStatus(hcblasSgemm(
      handle, hipOperationToHCCOperation(transa),
      hipOperationToHCCOperation(transb), m, n, k, alpha,
      const_cast<float *>(A), lda, const_cast<float *>(B), ldb, beta, C, ldc));
}

hipblasStatus_t hipblasDgemm(hipblasHandle_t handle, hipblasOperation_t transa,
                             hipblasOperation_t transb, int m, int n, int k,
                             const double *alpha, const double *A, int lda,
                             const double *B, int ldb, const double *beta,
                             double *C, int ldc) {
  return hipHCBLASStatusToHIPStatus(
      hcblasDgemm(handle, hipOperationToHCCOperation(transa),
                  hipOperationToHCCOperation(transb), m, n, k, alpha,
                  const_cast<double *>(A), lda, const_cast<double *>(B), ldb,
                  beta, C, ldc));
}

hipblasStatus_t hipblasCgemm(hipblasHandle_t handle, hipblasOperation_t transa,
                             hipblasOperation_t transb, int m, int n, int k,
                             const hipComplex *alpha, const hipComplex *A,
                             int lda, const hipComplex *B, int ldb,
                             const hipComplex *beta, hipComplex *C, int ldc) {
  return hipHCBLASStatusToHIPStatus(hcblasCgemm(
      handle, hipOperationToHCCOperation(transa),
      hipOperationToHCCOperation(transb), m, n, k, (const hcComplex *)(alpha),
      const_cast<hcComplex *>((const hcComplex *)(A)), lda,
      const_cast<hcComplex *>((const hcComplex *)(B)), ldb,
      reinterpret_cast<const hcComplex *>(beta),
      reinterpret_cast<hcComplex *>(C), ldc));
}

hipblasStatus_t hipblasZgemm(hipblasHandle_t handle, hipblasOperation_t transa,
                             hipblasOperation_t transb, int m, int n, int k,
                             const hipDoubleComplex *alpha,
                             const hipDoubleComplex *A, int lda,
                             const hipDoubleComplex *B, int ldb,
                             const hipDoubleComplex *beta, hipDoubleComplex *C,
                             int ldc) {
  return hipHCBLASStatusToHIPStatus(hcblasZgemm(
      handle, hipOperationToHCCOperation(transa),
      hipOperationToHCCOperation(transb), m, n, k,
      (const hcDoubleComplex *)(alpha),
      const_cast<hcDoubleComplex *>((const hcDoubleComplex *)(A)), lda,
      const_cast<hcDoubleComplex *>((const hcDoubleComplex *)(B)), ldb,
      reinterpret_cast<const hcDoubleComplex *>(beta),
      reinterpret_cast<hcDoubleComplex *>(C), ldc));
}

hipblasStatus_t hipblasHgemm(hipblasHandle_t handle, hipblasOperation_t transa,
                             hipblasOperation_t transb, int m, int n, int k,
                             const __half *alpha, __half *A, int lda, __half *B,
                             int ldb, const __half *beta, __half *C, int ldc) {
  return hipHCBLASStatusToHIPStatus(
      hcblasHgemm(handle, hipOperationToHCCOperation(transa),
                  hipOperationToHCCOperation(transb), m, n, k, alpha, A, lda, B,
                  ldb, beta, C, ldc));
}

hipblasStatus_t hipblasSgemmBatched(hipblasHandle_t handle,
                                    hipblasOperation_t transa,
                                    hipblasOperation_t transb, int m, int n,
                                    int k, const float *alpha, const float *A[],
                                    int lda, const float *B[], int ldb,
                                    const float *beta, float *C[], int ldc,
                                    int batchCount) {
  // TODO(Neelakandan): Remove const cast
  return hipHCBLASStatusToHIPStatus(
      hcblasSgemmBatched(handle, hipOperationToHCCOperation(transa),
                         hipOperationToHCCOperation(transb), m, n, k, alpha,
                         const_cast<float **>(A), lda, const_cast<float **>(B),
                         ldb, beta, C, ldc, batchCount));
}

hipblasStatus_t hipblasDgemmBatched(
    hipblasHandle_t handle, hipblasOperation_t transa,
    hipblasOperation_t transb, int m, int n, int k, const double *alpha,
    const double *A[], int lda, const double *B[], int ldb, const double *beta,
    double *C[], int ldc, int batchCount) {
  return hipHCBLASStatusToHIPStatus(hcblasDgemmBatched(
      handle, hipOperationToHCCOperation(transa),
      hipOperationToHCCOperation(transb), m, n, k, alpha,
      const_cast<double **>(A), lda, const_cast<double **>(B), ldb, beta, C,
      ldc, batchCount));
}

hipblasStatus_t hipblasCgemmBatched(
    hipblasHandle_t handle, hipblasOperation_t transa,
    hipblasOperation_t transb, int m, int n, int k, const hipComplex *alpha,
    const hipComplex *A[], int lda, const hipComplex *B[], int ldb,
    const hipComplex *beta, hipComplex *C[], int ldc, int batchCount) {
  return hipHCBLASStatusToHIPStatus(hcblasCgemmBatched(
      handle, hipOperationToHCCOperation(transa),
      hipOperationToHCCOperation(transb), m, n, k, (const hcComplex *)alpha,
      const_cast<hcComplex **>((const hcComplex **)(A)), lda,
      const_cast<hcComplex **>((const hcComplex **)(B)), ldb,
      reinterpret_cast<const hcComplex *>(beta),
      reinterpret_cast<hcComplex **>(C), ldc, batchCount));
}

hipblasStatus_t hipblasZgemmBatched(
    hipblasHandle_t handle, hipblasOperation_t transa,
    hipblasOperation_t transb, int m, int n, int k,
    const hipDoubleComplex *alpha, const hipDoubleComplex *A[], int lda,
    const hipDoubleComplex *B[], int ldb, const hipDoubleComplex *beta,
    hipDoubleComplex *C[], int ldc, int batchCount) {
  return hipHCBLASStatusToHIPStatus(hcblasZgemmBatched(
      handle, hipOperationToHCCOperation(transa),
      hipOperationToHCCOperation(transb), m, n, k,
      (const hcDoubleComplex *)alpha,
      const_cast<hcDoubleComplex **>((const hcDoubleComplex **)(A)), lda,
      const_cast<hcDoubleComplex **>((const hcDoubleComplex **)(B)), ldb,
      reinterpret_cast<const hcDoubleComplex *>(beta),
      reinterpret_cast<hcDoubleComplex **>(C), ldc, batchCount));
}

#ifdef __cplusplus
}
#endif

