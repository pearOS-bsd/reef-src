// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// vForce, vImage, BLAS, LAPACK (64-bit integer interface, $NEWLAPACK$ILP64) and BNNS graph functions GarageBand
// imports and Reef's Accelerate lacks.
#include <complex.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// vForce
void vvexpf(float *y, const float *x, const int *n) { for (int i = 0; i < *n; i++) y[i] = expf(x[i]); }
void vvexp2f(float *y, const float *x, const int *n) { for (int i = 0; i < *n; i++) y[i] = exp2f(x[i]); }
void vvlog2f(float *y, const float *x, const int *n) { for (int i = 0; i < *n; i++) y[i] = log2f(x[i]); }
void vvfloorf(float *y, const float *x, const int *n) { for (int i = 0; i < *n; i++) y[i] = floorf(x[i]); }
void vvfmodf(float *y, const float *x, const float *z, const int *n) { for (int i = 0; i < *n; i++) y[i] = fmodf(x[i], z[i]); }
void vfabsf(float *y, const float *x, const int *n) { for (int i = 0; i < *n; i++) y[i] = fabsf(x[i]); }

// vImage: a tent (triangle) kernel blur of ARGB8888, edges extended
typedef struct { void *data; unsigned long height, width; size_t rowBytes; } vImage_Buffer;
long vImageTentConvolve_ARGB8888(const vImage_Buffer *src, const vImage_Buffer *dest, void *temp, unsigned long x0, unsigned long y0,
                                 uint32_t kernelHeight, uint32_t kernelWidth, const uint8_t *background, uint32_t flags) {
    if (flags & 0x80) return 0;   // kvImageGetTempBufferSize
    if (!(kernelHeight & 1) || !(kernelWidth & 1)) return -21767;   // kvImageInvalidKernelSize
    const long rw = kernelWidth / 2, rh = kernelHeight / 2;
    const long W = (long)src->width, H = (long)src->height;
    for (unsigned long y = 0; y < dest->height; y++)
        for (unsigned long x = 0; x < dest->width; x++) {
            long sum[4] = {0}, total = 0;
            for (long dy = -rh; dy <= rh; dy++)
                for (long dx = -rw; dx <= rw; dx++) {
                    long wgt = (rw + 1 - labs(dx)) * (rh + 1 - labs(dy));
                    long sx = (long)(x + x0) + dx, sy = (long)(y + y0) + dy;
                    if (sx < 0) sx = 0; if (sx >= W) sx = W - 1;
                    if (sy < 0) sy = 0; if (sy >= H) sy = H - 1;
                    const uint8_t *p = (const uint8_t *)src->data + sy * src->rowBytes + sx * 4;
                    for (int c = 0; c < 4; c++) sum[c] += wgt * p[c];
                    total += wgt;
                }
            uint8_t *q = (uint8_t *)dest->data + y * dest->rowBytes + x * 4;
            for (int c = 0; c < 4; c++) q[c] = (uint8_t)((sum[c] + total / 2) / total);
        }
    return 0;
}

// BLAS (CblasRowMajor 101, CblasColMajor 102; CblasNoTrans 111, CblasTrans 112)
void cblas_scopy(int n, const float *x, int incx, float *y, int incy) {
    for (int i = 0; i < n; i++) y[(long)i * incy] = x[(long)i * incx];
}

#define GEMV(NAME, T, I)                                                                                            \
    void NAME(int order, int trans, I m, I n, T alpha, const T *a, I lda, const T *x, I incx, T beta, T *y, I incy) { \
        int t = (order == 101) == (trans == 111) ? 0 : 1; /* 0: y = A x over rows of the row-major view */          \
        I rows = trans == 111 ? m : n, cols = trans == 111 ? n : m;                                                   \
        for (I i = 0; i < rows; i++) {                                                                                \
            T s = 0;                                                                                                  \
            for (I j = 0; j < cols; j++) {                                                                            \
                T e = (order == 101) ? (trans == 111 ? a[i * lda + j] : a[j * lda + i])                              \
                                     : (trans == 111 ? a[j * lda + i] : a[i * lda + j]);                             \
                s += e * x[j * incx];                                                                                 \
            }                                                                                                         \
            y[i * incy] = alpha * s + (beta == 0 ? 0 : beta * y[i * incy]);                                           \
        }                                                                                                             \
        (void)t;                                                                                                      \
    }
GEMV(cblas_sgemv, float, int)
void cblas_dgemv_ilp64(int order, int trans, long m, long n, double alpha, const double *a, long lda, const double *x, long incx,
                       double beta, double *y, long incy) __asm__("_cblas_dgemv$NEWLAPACK$ILP64");
GEMV(cblas_dgemv_ilp64, double, long)

// LAPACK, column-major, 64-bit integers. LU with partial pivoting: dgetrf, dgetrs, dgetri, dgesv; dsysv through LU
// of the full symmetric matrix; zcgesv in double complex. The eigen and singular value decompositions (dgeev,
// dgesvd) report that they did not converge.
typedef long lapack_int;

void dgetrf_(lapack_int *m, lapack_int *n, double *a, lapack_int *lda, lapack_int *ipiv, lapack_int *info) __asm__("_dgetrf$NEWLAPACK$ILP64");
void dgetrf_(lapack_int *m, lapack_int *n, double *a, lapack_int *lda, lapack_int *ipiv, lapack_int *info) {
    lapack_int M = *m, N = *n, L = *lda, k = M < N ? M : N;
    *info = 0;
    for (lapack_int j = 0; j < k; j++) {
        lapack_int p = j;
        for (lapack_int i = j + 1; i < M; i++) if (fabs(a[i + j * L]) > fabs(a[p + j * L])) p = i;
        ipiv[j] = p + 1;
        if (a[p + j * L] == 0) { if (!*info) *info = j + 1; continue; }
        if (p != j) for (lapack_int c = 0; c < N; c++) { double t = a[j + c * L]; a[j + c * L] = a[p + c * L]; a[p + c * L] = t; }
        for (lapack_int i = j + 1; i < M; i++) {
            a[i + j * L] /= a[j + j * L];
            for (lapack_int c = j + 1; c < N; c++) a[i + c * L] -= a[i + j * L] * a[j + c * L];
        }
    }
}

static void getrs(const char *trans, lapack_int n, lapack_int nrhs, const double *a, lapack_int lda, const lapack_int *ipiv, double *b, lapack_int ldb) {
    int t = trans[0] == 'T' || trans[0] == 't' || trans[0] == 'C' || trans[0] == 'c';
    for (lapack_int r = 0; r < nrhs; r++) {
        double *x = b + r * ldb;
        if (!t) {
            for (lapack_int i = 0; i < n; i++) { lapack_int p = ipiv[i] - 1; if (p != i) { double tt = x[i]; x[i] = x[p]; x[p] = tt; } }
            for (lapack_int i = 0; i < n; i++) for (lapack_int k = 0; k < i; k++) x[i] -= a[i + k * lda] * x[k];
            for (lapack_int i = n - 1; i >= 0; i--) { for (lapack_int k = i + 1; k < n; k++) x[i] -= a[i + k * lda] * x[k]; x[i] /= a[i + i * lda]; }
        } else {
            for (lapack_int i = 0; i < n; i++) { for (lapack_int k = 0; k < i; k++) x[i] -= a[k + i * lda] * x[k]; x[i] /= a[i + i * lda]; }
            for (lapack_int i = n - 1; i >= 0; i--) for (lapack_int k = i + 1; k < n; k++) x[i] -= a[k + i * lda] * x[k];
            for (lapack_int i = n - 1; i >= 0; i--) { lapack_int p = ipiv[i] - 1; if (p != i) { double tt = x[i]; x[i] = x[p]; x[p] = tt; } }
        }
    }
}

void dgetrs_ilp64(const char *trans, lapack_int *n, lapack_int *nrhs, const double *a, lapack_int *lda, const lapack_int *ipiv, double *b, lapack_int *ldb, lapack_int *info) __asm__("_dgetrs$NEWLAPACK$ILP64");
void dgetrs_ilp64(const char *trans, lapack_int *n, lapack_int *nrhs, const double *a, lapack_int *lda, const lapack_int *ipiv, double *b, lapack_int *ldb, lapack_int *info) {
    *info = 0;
    getrs(trans, *n, *nrhs, a, *lda, ipiv, b, *ldb);
}

void dgesv_ilp64(lapack_int *n, lapack_int *nrhs, double *a, lapack_int *lda, lapack_int *ipiv, double *b, lapack_int *ldb, lapack_int *info) __asm__("_dgesv$NEWLAPACK$ILP64");
void dgesv_ilp64(lapack_int *n, lapack_int *nrhs, double *a, lapack_int *lda, lapack_int *ipiv, double *b, lapack_int *ldb, lapack_int *info) {
    dgetrf_(n, n, a, lda, ipiv, info);
    if (*info == 0) getrs("N", *n, *nrhs, a, *lda, ipiv, b, *ldb);
}

void dgetri_ilp64(lapack_int *n, double *a, lapack_int *lda, const lapack_int *ipiv, double *work, lapack_int *lwork, lapack_int *info) __asm__("_dgetri$NEWLAPACK$ILP64");
void dgetri_ilp64(lapack_int *n, double *a, lapack_int *lda, const lapack_int *ipiv, double *work, lapack_int *lwork, lapack_int *info) {
    lapack_int N = *n, L = *lda;
    *info = 0;
    if (*lwork == -1) { work[0] = (double)N; return; }
    for (lapack_int i = 0; i < N; i++) if (a[i + i * L] == 0) { *info = i + 1; return; }
    double *inv = calloc((size_t)(N * N), sizeof *inv);
    for (lapack_int i = 0; i < N; i++) inv[i + i * N] = 1;
    getrs("N", N, N, a, L, ipiv, inv, N);
    for (lapack_int c = 0; c < N; c++) for (lapack_int r = 0; r < N; r++) a[r + c * L] = inv[r + c * N];
    free(inv);
}

void dsysv_ilp64(const char *uplo, lapack_int *n, lapack_int *nrhs, double *a, lapack_int *lda, lapack_int *ipiv, double *b, lapack_int *ldb, double *work, lapack_int *lwork, lapack_int *info) __asm__("_dsysv$NEWLAPACK$ILP64");
void dsysv_ilp64(const char *uplo, lapack_int *n, lapack_int *nrhs, double *a, lapack_int *lda, lapack_int *ipiv, double *b, lapack_int *ldb, double *work, lapack_int *lwork, lapack_int *info) {
    lapack_int N = *n, L = *lda;
    *info = 0;
    if (*lwork == -1) { work[0] = (double)(N > 1 ? N : 1); return; }
    int upper = uplo[0] == 'U' || uplo[0] == 'u';
    for (lapack_int c = 0; c < N; c++)
        for (lapack_int r = 0; r < N; r++)
            if (upper ? r > c : r < c) a[r + c * L] = a[c + r * L];
    dgetrf_(n, n, a, lda, ipiv, info);
    if (*info == 0) getrs("N", N, *nrhs, a, L, ipiv, b, *ldb);
}

// Mixed precision complex solve: here all in double complex. ITER 0.
void zcgesv_ilp64(lapack_int *n, lapack_int *nrhs, double complex *a, lapack_int *lda, lapack_int *ipiv, double complex *b, lapack_int *ldb,
                  double complex *x, lapack_int *ldx, double complex *work, float complex *swork, double *rwork, lapack_int *iter, lapack_int *info) __asm__("_zcgesv$NEWLAPACK$ILP64");
void zcgesv_ilp64(lapack_int *n, lapack_int *nrhs, double complex *a, lapack_int *lda, lapack_int *ipiv, double complex *b, lapack_int *ldb,
                  double complex *x, lapack_int *ldx, double complex *work, float complex *swork, double *rwork, lapack_int *iter, lapack_int *info) {
    lapack_int N = *n, L = *lda;
    *info = 0;
    *iter = 0;
    for (lapack_int j = 0; j < N; j++) {
        lapack_int p = j;
        for (lapack_int i = j + 1; i < N; i++) if (cabs(a[i + j * L]) > cabs(a[p + j * L])) p = i;
        ipiv[j] = p + 1;
        if (a[p + j * L] == 0) { *info = j + 1; return; }
        if (p != j) for (lapack_int c = 0; c < N; c++) { double complex t = a[j + c * L]; a[j + c * L] = a[p + c * L]; a[p + c * L] = t; }
        for (lapack_int i = j + 1; i < N; i++) {
            a[i + j * L] /= a[j + j * L];
            for (lapack_int c = j + 1; c < N; c++) a[i + c * L] -= a[i + j * L] * a[j + c * L];
        }
    }
    for (lapack_int r = 0; r < *nrhs; r++) {
        double complex *v = x + r * *ldx;
        for (lapack_int i = 0; i < N; i++) v[i] = b[i + r * *ldb];
        for (lapack_int i = 0; i < N; i++) { lapack_int p = ipiv[i] - 1; if (p != i) { double complex t = v[i]; v[i] = v[p]; v[p] = t; } }
        for (lapack_int i = 0; i < N; i++) for (lapack_int k = 0; k < i; k++) v[i] -= a[i + k * L] * v[k];
        for (lapack_int i = N - 1; i >= 0; i--) { for (lapack_int k = i + 1; k < N; k++) v[i] -= a[i + k * L] * v[k]; v[i] /= a[i + i * L]; }
    }
}

void dgeev_ilp64(const char *jobvl, const char *jobvr, lapack_int *n, double *a, lapack_int *lda, double *wr, double *wi, double *vl, lapack_int *ldvl,
                 double *vr, lapack_int *ldvr, double *work, lapack_int *lwork, lapack_int *info) __asm__("_dgeev$NEWLAPACK$ILP64");
void dgeev_ilp64(const char *jobvl, const char *jobvr, lapack_int *n, double *a, lapack_int *lda, double *wr, double *wi, double *vl, lapack_int *ldvl,
                 double *vr, lapack_int *ldvr, double *work, lapack_int *lwork, lapack_int *info) {
    if (*lwork == -1) { work[0] = (double)(4 * *n); *info = 0; return; }
    *info = *n > 0 ? *n : 1;   // not converged
}

void dgesvd_ilp64(const char *jobu, const char *jobvt, lapack_int *m, lapack_int *n, double *a, lapack_int *lda, double *s, double *u, lapack_int *ldu,
                  double *vt, lapack_int *ldvt, double *work, lapack_int *lwork, lapack_int *info) __asm__("_dgesvd$NEWLAPACK$ILP64");
void dgesvd_ilp64(const char *jobu, const char *jobvt, lapack_int *m, lapack_int *n, double *a, lapack_int *lda, double *s, double *u, lapack_int *ldu,
                  double *vt, lapack_int *ldvt, double *work, lapack_int *lwork, lapack_int *info) {
    if (*lwork == -1) { work[0] = (double)(5 * (*m > *n ? *m : *n)); *info = 0; return; }
    *info = 1;   // not converged
}

// BNNS graphs (compiled Core ML models): no compiler here, compilation fails (a null graph) and nothing runs.
typedef struct { void *data; size_t size; } bnns_graph_t;
typedef struct { void *data; size_t size; } bnns_graph_context_t;
typedef struct { void *data; size_t size; } bnns_graph_compile_options_t;
bnns_graph_compile_options_t BNNSGraphCompileOptionsMakeDefault(void) { return (bnns_graph_compile_options_t){NULL, 0}; }
void BNNSGraphCompileOptionsDestroy(bnns_graph_compile_options_t options) {}
void BNNSGraphCompileOptionsSetTargetSingleThread(bnns_graph_compile_options_t options, _Bool value) {}
bnns_graph_t BNNSGraphCompileFromFile_v2(const char *path, const char *function, bnns_graph_compile_options_t options) { return (bnns_graph_t){NULL, 0}; }
bnns_graph_context_t BNNSGraphContextMake(bnns_graph_t graph) { return (bnns_graph_context_t){NULL, 0}; }
void BNNSGraphContextDestroy_v2(bnns_graph_context_t context) {}
int BNNSGraphContextExecute_v2(bnns_graph_context_t context, const char *function, size_t count, void *arguments, size_t workspaceSize, char *workspace) { return -1; }
size_t BNNSGraphContextGetWorkspaceSize_v2(bnns_graph_context_t context, const char *function) { return 0; }
int BNNSGraphContextGetTensor(bnns_graph_context_t context, const char *function, const char *argument, _Bool fill, void *tensor) { return -1; }
size_t BNNSGraphGetInputCount(bnns_graph_t graph, const char *function) { return 0; }
size_t BNNSGraphGetOutputCount(bnns_graph_t graph, const char *function) { return 0; }
int BNNSGraphGetInputNames_v2(bnns_graph_t graph, const char *function, size_t count, const char **names) { return -1; }
int BNNSGraphGetOutputNames_v2(bnns_graph_t graph, const char *function, size_t count, const char **names) { return -1; }
