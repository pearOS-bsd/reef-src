// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// vDSP functions GarageBand imports and Reef's Accelerate lacks, with vDSP's conventions (vDSP.h): strides in
// elements, vDSP_Length counts, unscaled FFTs, index results multiplied by the stride.
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef long vDSP_Stride;
typedef unsigned long vDSP_Length;
typedef struct { float *realp, *imagp; } DSPSplitComplex;
typedef struct { double *realp, *imagp; } DSPDoubleSplitComplex;
typedef struct { float real, imag; } DSPComplex;
typedef struct OpaqueFFTSetup *FFTSetup;
typedef struct OpaqueFFTSetupD *FFTSetupD;
typedef int FFTDirection;

extern void vDSP_fft_zrip(FFTSetup, const DSPSplitComplex *, vDSP_Stride, vDSP_Length, FFTDirection);

// Windows: vDSP_HALF_WINDOW 1 gives the first (N + 1) / 2 values, vDSP_HANN_NORM 2 scales Hann by 0.8165
static vDSP_Length windowLength(vDSP_Length n, int flag) { return (flag & 1) ? (n + 1) / 2 : n; }

void vDSP_hann_window(float *c, vDSP_Length n, int flag) {
    const double scale = (flag & 2) ? 0.8165 : 0.5;
    for (vDSP_Length i = 0; i < windowLength(n, flag); i++) c[i] = (float)(scale * (1.0 - cos(2.0 * M_PI * i / n)));
}
void vDSP_hamm_window(float *c, vDSP_Length n, int flag) {
    for (vDSP_Length i = 0; i < windowLength(n, flag); i++) c[i] = (float)(0.54 - 0.46 * cos(2.0 * M_PI * i / n));
}
void vDSP_blkman_window(float *c, vDSP_Length n, int flag) {
    for (vDSP_Length i = 0; i < windowLength(n, flag); i++)
        c[i] = (float)(0.42 - 0.5 * cos(2.0 * M_PI * i / n) + 0.08 * cos(4.0 * M_PI * i / n));
}

// Reductions
void vDSP_dotprD(const double *a, vDSP_Stride ia, const double *b, vDSP_Stride ib, double *c, vDSP_Length n) {
    double s = 0; for (vDSP_Length i = 0; i < n; i++) s += a[i * ia] * b[i * ib]; *c = s;
}
void vDSP_distancesq(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Length n) {
    float s = 0; for (vDSP_Length i = 0; i < n; i++) { float d = a[i * ia] - b[i * ib]; s += d * d; } *c = s;
}
void vDSP_sveD(const double *a, vDSP_Stride ia, double *c, vDSP_Length n) {
    double s = 0; for (vDSP_Length i = 0; i < n; i++) s += a[i * ia]; *c = s;
}
void vDSP_svemg(const float *a, vDSP_Stride ia, float *c, vDSP_Length n) {
    float s = 0; for (vDSP_Length i = 0; i < n; i++) s += fabsf(a[i * ia]); *c = s;
}
void vDSP_meanv(const float *a, vDSP_Stride ia, float *c, vDSP_Length n) {
    float s = 0; for (vDSP_Length i = 0; i < n; i++) s += a[i * ia]; *c = n ? s / n : NAN;
}
void vDSP_meanvD(const double *a, vDSP_Stride ia, double *c, vDSP_Length n) {
    double s = 0; for (vDSP_Length i = 0; i < n; i++) s += a[i * ia]; *c = n ? s / n : NAN;
}
void vDSP_maxvD(const double *a, vDSP_Stride ia, double *c, vDSP_Length n) {
    double m = -INFINITY; for (vDSP_Length i = 0; i < n; i++) if (a[i * ia] > m) m = a[i * ia]; *c = m;
}
void vDSP_minv(const float *a, vDSP_Stride ia, float *c, vDSP_Length n) {
    float m = INFINITY; for (vDSP_Length i = 0; i < n; i++) if (a[i * ia] < m) m = a[i * ia]; *c = m;
}
void vDSP_minvD(const double *a, vDSP_Stride ia, double *c, vDSP_Length n) {
    double m = INFINITY; for (vDSP_Length i = 0; i < n; i++) if (a[i * ia] < m) m = a[i * ia]; *c = m;
}
void vDSP_maxvi(const float *a, vDSP_Stride ia, float *c, vDSP_Length *ic, vDSP_Length n) {
    float m = -INFINITY; vDSP_Length k = 0;
    for (vDSP_Length i = 0; i < n; i++) if (a[i * ia] > m) { m = a[i * ia]; k = i; }
    *c = m; *ic = k * ia;
}
void vDSP_maxviD(const double *a, vDSP_Stride ia, double *c, vDSP_Length *ic, vDSP_Length n) {
    double m = -INFINITY; vDSP_Length k = 0;
    for (vDSP_Length i = 0; i < n; i++) if (a[i * ia] > m) { m = a[i * ia]; k = i; }
    *c = m; *ic = k * ia;
}
void vDSP_minvi(const float *a, vDSP_Stride ia, float *c, vDSP_Length *ic, vDSP_Length n) {
    float m = INFINITY; vDSP_Length k = 0;
    for (vDSP_Length i = 0; i < n; i++) if (a[i * ia] < m) { m = a[i * ia]; k = i; }
    *c = m; *ic = k * ia;
}
void vDSP_minviD(const double *a, vDSP_Stride ia, double *c, vDSP_Length *ic, vDSP_Length n) {
    double m = INFINITY; vDSP_Length k = 0;
    for (vDSP_Length i = 0; i < n; i++) if (a[i * ia] < m) { m = a[i * ia]; k = i; }
    *c = m; *ic = k * ia;
}
void vDSP_minmgvi(const float *a, vDSP_Stride ia, float *c, vDSP_Length *ic, vDSP_Length n) {
    float m = INFINITY; vDSP_Length k = 0;
    for (vDSP_Length i = 0; i < n; i++) if (fabsf(a[i * ia]) < m) { m = fabsf(a[i * ia]); k = i; }
    *c = m; *ic = k * ia;
}
void vDSP_normalize(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, float *mean, float *deviation, vDSP_Length n) {
    double s = 0, q = 0;
    for (vDSP_Length i = 0; i < n; i++) s += a[i * ia];
    double m = n ? s / n : 0;
    for (vDSP_Length i = 0; i < n; i++) { double d = a[i * ia] - m; q += d * d; }
    double sd = n ? sqrt(q / n) : 0;
    *mean = (float)m; *deviation = (float)sd;
    if (c) for (vDSP_Length i = 0; i < n; i++) c[i * ic] = (float)((a[i * ia] - m) / sd);
}
void vDSP_nzcros(const float *a, vDSP_Stride ia, vDSP_Length b, vDSP_Length *c, vDSP_Length *d, vDSP_Length n) {
    vDSP_Length count = 0, last = 0;
    for (vDSP_Length i = 1; i < n && count < b; i++) {
        if (signbit(a[(i - 1) * ia]) != signbit(a[i * ia])) { count++; last = i * ia; }
    }
    *c = last; *d = count;
}

// Element-wise
void vDSP_vabs(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = fabsf(a[i * ia]); }
void vDSP_vneg(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = -a[i * ia]; }
void vDSP_vnegD(const double *a, vDSP_Stride ia, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = -a[i * ia]; }
void vDSP_vsq(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] * a[i * ia]; }
void vDSP_vsqD(const double *a, vDSP_Stride ia, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] * a[i * ia]; }
void vDSP_vfrac(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] - truncf(a[i * ia]); }
void vDSP_vaddD(const double *a, vDSP_Stride ia, const double *b, vDSP_Stride ib, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] + b[i * ib]; }
// vDSP_vsub and vDSP_vdiv take the subtrahend and the divisor first
void vDSP_vsubD(const double *b, vDSP_Stride ib, const double *a, vDSP_Stride ia, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] - b[i * ib]; }
void vDSP_vdivD(const double *b, vDSP_Stride ib, const double *a, vDSP_Stride ia, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] / b[i * ib]; }
void vDSP_vmulD(const double *a, vDSP_Stride ia, const double *b, vDSP_Stride ib, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] * b[i * ib]; }
void vDSP_vsaddD(const double *a, vDSP_Stride ia, const double *b, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] + *b; }
void vDSP_svdiv(const float *a, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = *a / b[i * ib]; }
void vDSP_vmax(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] >= b[i * ib] ? a[i * ia] : b[i * ib]; }
void vDSP_vmin(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] <= b[i * ib] ? a[i * ia] : b[i * ib]; }
void vDSP_vmaxmg(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = fmaxf(fabsf(a[i * ia]), fabsf(b[i * ib])); }
void vDSP_vdist(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = sqrtf(a[i * ia] * a[i * ia] + b[i * ib] * b[i * ib]); }
void vDSP_vasm(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, const float *c, float *d, vDSP_Stride id, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) d[i * id] = (a[i * ia] + b[i * ib]) * *c; }
void vDSP_vma(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, const float *c, vDSP_Stride ic, float *d, vDSP_Stride id, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) d[i * id] = a[i * ia] * b[i * ib] + c[i * ic]; }
void vDSP_vsmsma(const float *a, vDSP_Stride ia, const float *b, const float *c, vDSP_Stride ic, const float *d, float *e, vDSP_Stride ie, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) e[i * ie] = a[i * ia] * *b + c[i * ic] * *d; }
void vDSP_vthr(const float *a, vDSP_Stride ia, const float *b, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] >= *b ? a[i * ia] : *b; }
void vDSP_vthres(const float *a, vDSP_Stride ia, const float *b, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = a[i * ia] >= *b ? a[i * ia] : 0; }
void vDSP_vlim(const float *a, vDSP_Stride ia, const float *b, const float *c, float *d, vDSP_Stride id, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) d[i * id] = a[i * ia] >= *b ? *c : -*c; }
void vDSP_vclipc(const float *a, vDSP_Stride ia, const float *lo, const float *hi, float *d, vDSP_Stride id, vDSP_Length n, vDSP_Length *nlo, vDSP_Length *nhi) {
    vDSP_Length l = 0, h = 0;
    for (vDSP_Length i = 0; i < n; i++) {
        float v = a[i * ia];
        if (v < *lo) { v = *lo; l++; } else if (v > *hi) { v = *hi; h++; }
        d[i * id] = v;
    }
    *nlo = l; *nhi = h;
}
// F = 0: power (10 log10), F = 1: amplitude (20 log10), relative to the zero reference B
void vDSP_vdbcon(const float *a, vDSP_Stride ia, const float *b, float *c, vDSP_Stride ic, vDSP_Length n, unsigned int f) {
    const float k = f ? 20.0f : 10.0f;
    for (vDSP_Length i = 0; i < n; i++) c[i * ic] = k * log10f(a[i * ia] / *b);
}
void vDSP_vfill(const float *a, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = *a; }
void vDSP_vfillD(const double *a, double *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = *a; }
void vDSP_vfix16(const float *a, vDSP_Stride ia, short *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = (short)a[i * ia]; }
void vDSP_vfix32(const float *a, vDSP_Stride ia, int *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = (int)a[i * ia]; }
void vDSP_vfixr16(const float *a, vDSP_Stride ia, short *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = (short)lrintf(a[i * ia]); }
void vDSP_vfixr32(const float *a, vDSP_Stride ia, int *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = (int)lrintf(a[i * ia]); }
void vDSP_vflt8(const char *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) { for (vDSP_Length i = 0; i < n; i++) c[i * ic] = (float)(signed char)a[i * ia]; }
void vDSP_vrvrs(float *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n / 2; i++) { float t = c[i * ic]; c[i * ic] = c[(n - 1 - i) * ic]; c[(n - 1 - i) * ic] = t; }
}

// Generation
void vDSP_vgen(const float *a, const float *b, float *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) c[i * ic] = n > 1 ? *a + (*b - *a) * (float)i / (float)(n - 1) : *a;
}
void vDSP_vramp(const float *a, const float *b, float *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) c[i * ic] = *a + (float)i * *b;
}
void vDSP_vrampmul(const float *in, vDSP_Stride is, float *start, const float *step, float *out, vDSP_Stride os, vDSP_Length n) {
    float g = *start;
    for (vDSP_Length i = 0; i < n; i++) { out[i * os] = in[i * is] * g; g += *step; }
    *start = g;
}
void vDSP_vrampmuladd(const float *in, vDSP_Stride is, float *start, const float *step, float *out, vDSP_Stride os, vDSP_Length n) {
    float g = *start;
    for (vDSP_Length i = 0; i < n; i++) { out[i * os] += in[i * is] * g; g += *step; }
    *start = g;
}
void vDSP_vrampmul2(const float *i0, const float *i1, vDSP_Stride is, float *start, const float *step, float *o0, float *o1, vDSP_Stride os, vDSP_Length n) {
    float g = *start;
    for (vDSP_Length i = 0; i < n; i++) { o0[i * os] = i0[i * is] * g; o1[i * os] = i1[i * is] * g; g += *step; }
    *start = g;
}
// Piecewise linear: B holds increasing positions of the M values of A; C gets N interpolated points
void vDSP_vgenp(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n, vDSP_Length m) {
    for (vDSP_Length i = 0; i < n; i++) {
        float x = (float)i;
        if (m == 0) { c[i * ic] = 0; continue; }
        if (x <= b[0]) { c[i * ic] = a[0]; continue; }
        if (x >= b[(m - 1) * ib]) { c[i * ic] = a[(m - 1) * ia]; continue; }
        vDSP_Length k = 0;
        while (k + 1 < m && b[(k + 1) * ib] <= x) k++;
        float x0 = b[k * ib], x1 = b[(k + 1) * ib];
        c[i * ic] = a[k * ia] + (a[(k + 1) * ia] - a[k * ia]) * (x - x0) / (x1 - x0);
    }
}
void vDSP_vpoly(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n, vDSP_Length p) {
    for (vDSP_Length i = 0; i < n; i++) {
        float x = b[i * ib], y = a[0];
        for (vDSP_Length k = 1; k <= p; k++) y = y * x + a[k * ia];
        c[i * ic] = y;
    }
}
// Table lookups: linear (vlint), quadratic (vqint), scaled index with linear interpolation (vtabi)
void vDSP_vlint(const float *a, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n, vDSP_Length m) {
    for (vDSP_Length i = 0; i < n; i++) {
        float x = b[i * ib];
        long k = (long)x;
        if (k < 0) k = 0;
        if ((vDSP_Length)k >= m - 1) { c[i * ic] = a[m - 1]; continue; }
        float f = x - (float)k;
        c[i * ic] = a[k] + f * (a[k + 1] - a[k]);
    }
}
void vDSP_vqint(const float *a, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length n, vDSP_Length m) {
    for (vDSP_Length i = 0; i < n; i++) {
        float x = b[i * ib];
        long k = lrintf(x);
        if (k < 1) k = 1;
        if ((vDSP_Length)k > m - 2) k = (long)m - 2;
        float f = x - (float)k;
        float y0 = a[k - 1], y1 = a[k], y2 = a[k + 1];
        c[i * ic] = y1 + 0.5f * f * ((y2 - y0) + f * (y2 - 2 * y1 + y0));
    }
}
void vDSP_vtabi(const float *a, vDSP_Stride ia, const float *s1, const float *s2, const float *table, vDSP_Length m, float *d, vDSP_Stride id, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) {
        float x = *s1 * a[i * ia] + *s2;
        if (x <= 0) { d[i * id] = table[0]; continue; }
        if (x >= (float)(m - 1)) { d[i * id] = table[m - 1]; continue; }
        long k = (long)x;
        float f = x - (float)k;
        d[i * id] = table[k] + f * (table[k + 1] - table[k]);
    }
}
void vDSP_vswmax(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n, vDSP_Length w) {
    for (vDSP_Length i = 0; i < n; i++) {
        float m = a[i * ia];
        for (vDSP_Length k = 1; k < w; k++) if (a[(i + k) * ia] > m) m = a[(i + k) * ia];
        c[i * ic] = m;
    }
}

// Sorting (order 1 ascending, -1 descending)
static int upF(const void *x, const void *y) { float a = *(const float *)x, b = *(const float *)y; return a < b ? -1 : a > b; }
static int downF(const void *x, const void *y) { return upF(y, x); }
static int upD(const void *x, const void *y) { double a = *(const double *)x, b = *(const double *)y; return a < b ? -1 : a > b; }
static int downD(const void *x, const void *y) { return upD(y, x); }
void vDSP_vsort(float *c, vDSP_Length n, int order) { qsort(c, n, sizeof *c, order < 0 ? downF : upF); }
void vDSP_vsortD(double *c, vDSP_Length n, int order) { qsort(c, n, sizeof *c, order < 0 ? downD : upD); }
static const float *sortValues;
static int order_;
static int byValue(const void *x, const void *y) {
    float a = sortValues[*(const vDSP_Length *)x], b = sortValues[*(const vDSP_Length *)y];
    int r = a < b ? -1 : a > b;
    return order_ < 0 ? -r : r;
}
void vDSP_vsorti(const float *c, vDSP_Length *indices, vDSP_Length *list, vDSP_Length n, int order) {
    sortValues = c; order_ = order;
    qsort(indices, n, sizeof *indices, byValue);
}

// Matrices
void vDSP_mmov(const float *a, float *c, vDSP_Length m, vDSP_Length n, vDSP_Length ta, vDSP_Length tc) {
    for (vDSP_Length r = 0; r < n; r++) memmove(c + r * tc, a + r * ta, m * sizeof *a);
}
void vDSP_mmovD(const double *a, double *c, vDSP_Length m, vDSP_Length n, vDSP_Length ta, vDSP_Length tc) {
    for (vDSP_Length r = 0; r < n; r++) memmove(c + r * tc, a + r * ta, m * sizeof *a);
}
void vDSP_mmul(const float *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, float *c, vDSP_Stride ic, vDSP_Length m, vDSP_Length n, vDSP_Length p) {
    for (vDSP_Length i = 0; i < m; i++)
        for (vDSP_Length j = 0; j < n; j++) {
            float s = 0;
            for (vDSP_Length k = 0; k < p; k++) s += a[(i * p + k) * ia] * b[(k * n + j) * ib];
            c[(i * n + j) * ic] = s;
        }
}
void vDSP_mmulD(const double *a, vDSP_Stride ia, const double *b, vDSP_Stride ib, double *c, vDSP_Stride ic, vDSP_Length m, vDSP_Length n, vDSP_Length p) {
    for (vDSP_Length i = 0; i < m; i++)
        for (vDSP_Length j = 0; j < n; j++) {
            double s = 0;
            for (vDSP_Length k = 0; k < p; k++) s += a[(i * p + k) * ia] * b[(k * n + j) * ib];
            c[(i * n + j) * ic] = s;
        }
}
// C (M rows, N columns) is the transpose of A (N rows, M columns)
void vDSP_mtransD(const double *a, vDSP_Stride ia, double *c, vDSP_Stride ic, vDSP_Length m, vDSP_Length n) {
    for (vDSP_Length i = 0; i < m; i++)
        for (vDSP_Length j = 0; j < n; j++) c[(i * n + j) * ic] = a[(j * m + i) * ia];
}
// 2-D FIR of an image (M rows, N columns) with a P x Q kernel; outputs whose kernel leaves the image are 0
void vDSP_imgfirD(const double *a, vDSP_Length m, vDSP_Length n, const double *f, double *c, vDSP_Length p, vDSP_Length q) {
    for (vDSP_Length r = 0; r < m; r++)
        for (vDSP_Length s = 0; s < n; s++) {
            long r0 = (long)r - (long)p / 2, s0 = (long)s - (long)q / 2;
            if (r0 < 0 || s0 < 0 || r0 + (long)p > (long)m || s0 + (long)q > (long)n) { c[r * n + s] = 0; continue; }
            double sum = 0;
            for (vDSP_Length i = 0; i < p; i++)
                for (vDSP_Length j = 0; j < q; j++) sum += a[(r0 + i) * n + s0 + j] * f[i * q + j];
            c[r * n + s] = sum;
        }
}

// Polar and rectangular coordinates of interleaved pairs
void vDSP_polar(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) {
        float x = a[i * ia], y = a[i * ia + 1];
        c[i * ic] = sqrtf(x * x + y * y);
        c[i * ic + 1] = atan2f(y, x);
    }
}
void vDSP_rect(const float *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) {
        float r = a[i * ia], t = a[i * ia + 1];
        c[i * ic] = r * cosf(t);
        c[i * ic + 1] = r * sinf(t);
    }
}

// Split complex vectors
void vDSP_zvadd(const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *b, vDSP_Stride ib, const DSPSplitComplex *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) {
        c->realp[i * ic] = a->realp[i * ia] + b->realp[i * ib];
        c->imagp[i * ic] = a->imagp[i * ia] + b->imagp[i * ib];
    }
}
// conjugate A times B when conjugate is -1, A times B when 1
void vDSP_zvmul(const DSPSplitComplex *, vDSP_Stride, const DSPSplitComplex *, vDSP_Stride, const DSPSplitComplex *, vDSP_Stride, vDSP_Length, int);
void vDSP_zvcmul(const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *b, vDSP_Stride ib, const DSPSplitComplex *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) {
        float ar = a->realp[i * ia], ai = -a->imagp[i * ia], br = b->realp[i * ib], bi = b->imagp[i * ib];
        c->realp[i * ic] = ar * br - ai * bi;
        c->imagp[i * ic] = ar * bi + ai * br;
    }
}
void vDSP_zvcma(const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *b, vDSP_Stride ib, const DSPSplitComplex *c, vDSP_Stride ic, const DSPSplitComplex *d, vDSP_Stride id, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) {
        float ar = a->realp[i * ia], ai = -a->imagp[i * ia], br = b->realp[i * ib], bi = b->imagp[i * ib];
        float cr = c->realp[i * ic], ci = c->imagp[i * ic];
        d->realp[i * id] = ar * br - ai * bi + cr;
        d->imagp[i * id] = ar * bi + ai * br + ci;
    }
}
void vDSP_zrvmul(const DSPSplitComplex *a, vDSP_Stride ia, const float *b, vDSP_Stride ib, const DSPSplitComplex *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) {
        c->realp[i * ic] = a->realp[i * ia] * b[i * ib];
        c->imagp[i * ic] = a->imagp[i * ia] * b[i * ib];
    }
}
void vDSP_zvabs(const DSPSplitComplex *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) c[i * ic] = hypotf(a->realp[i * ia], a->imagp[i * ia]);
}
void vDSP_zvphas(const DSPSplitComplex *a, vDSP_Stride ia, float *c, vDSP_Stride ic, vDSP_Length n) {
    for (vDSP_Length i = 0; i < n; i++) c[i * ic] = atan2f(a->imagp[i * ia], a->realp[i * ia]);
}
void vDSP_zvsma(const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *b, const DSPSplitComplex *c, vDSP_Stride ic, const DSPSplitComplex *d, vDSP_Stride id, vDSP_Length n) {
    float br = b->realp[0], bi = b->imagp[0];
    for (vDSP_Length i = 0; i < n; i++) {
        float ar = a->realp[i * ia], ai = a->imagp[i * ia];
        d->realp[i * id] = ar * br - ai * bi + c->realp[i * ic];
        d->imagp[i * id] = ar * bi + ai * br + c->imagp[i * ic];
    }
}
void vDSP_zvzsml(const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *b, const DSPSplitComplex *c, vDSP_Stride ic, vDSP_Length n) {
    float br = b->realp[0], bi = b->imagp[0];
    for (vDSP_Length i = 0; i < n; i++) {
        float ar = a->realp[i * ia], ai = a->imagp[i * ia];
        c->realp[i * ic] = ar * br - ai * bi;
        c->imagp[i * ic] = ar * bi + ai * br;
    }
}

// Complex FFTs (radix 2, unscaled; forward is e^-i): the setup of Reef's vDSP_create_fftsetup is not needed
static void fftDouble(double *re, double *im, long stride, unsigned log2n, int direction) {
    const long n = 1L << log2n;
    for (long i = 1, j = 0; i < n; i++) {
        long bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) {
            double t = re[i * stride]; re[i * stride] = re[j * stride]; re[j * stride] = t;
            t = im[i * stride]; im[i * stride] = im[j * stride]; im[j * stride] = t;
        }
    }
    for (long len = 2; len <= n; len <<= 1) {
        const double angle = (direction > 0 ? -2.0 : 2.0) * M_PI / (double)len;
        for (long k = 0; k < len / 2; k++) {
            const double wr = cos(angle * k), wi = sin(angle * k);
            for (long s = k; s < n; s += len) {
                long u = s * stride, v = (s + len / 2) * stride;
                double xr = re[v] * wr - im[v] * wi, xi = re[v] * wi + im[v] * wr;
                re[v] = re[u] - xr; im[v] = im[u] - xi;
                re[u] += xr; im[u] += xi;
            }
        }
    }
}

static void fftFloat(float *re, float *im, long stride, unsigned log2n, int direction) {
    const long n = 1L << log2n;
    double *r = malloc(n * sizeof *r), *i = malloc(n * sizeof *i);
    for (long k = 0; k < n; k++) { r[k] = re[k * stride]; i[k] = im[k * stride]; }
    fftDouble(r, i, 1, log2n, direction);
    for (long k = 0; k < n; k++) { re[k * stride] = (float)r[k]; im[k * stride] = (float)i[k]; }
    free(r); free(i);
}

void vDSP_fft_zip(FFTSetup s, const DSPSplitComplex *c, vDSP_Stride ic, vDSP_Length log2n, FFTDirection d) { fftFloat(c->realp, c->imagp, ic, (unsigned)log2n, d); }
void vDSP_fft_zipt(FFTSetup s, const DSPSplitComplex *c, vDSP_Stride ic, const DSPSplitComplex *t, vDSP_Length log2n, FFTDirection d) { fftFloat(c->realp, c->imagp, ic, (unsigned)log2n, d); }
void vDSP_fft_ziptD(FFTSetupD s, const DSPDoubleSplitComplex *c, vDSP_Stride ic, const DSPDoubleSplitComplex *t, vDSP_Length log2n, FFTDirection d) { fftDouble(c->realp, c->imagp, ic, (unsigned)log2n, d); }
void vDSP_fft_zop(FFTSetup s, const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *c, vDSP_Stride ic, vDSP_Length log2n, FFTDirection d) {
    const long n = 1L << log2n;
    for (long k = 0; k < n; k++) { c->realp[k * ic] = a->realp[k * ia]; c->imagp[k * ic] = a->imagp[k * ia]; }
    fftFloat(c->realp, c->imagp, ic, (unsigned)log2n, d);
}
void vDSP_fft_zopt(FFTSetup s, const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *c, vDSP_Stride ic, const DSPSplitComplex *t, vDSP_Length log2n, FFTDirection d) { vDSP_fft_zop(s, a, ia, c, ic, log2n, d); }
void vDSP_fft_zopD(FFTSetupD s, const DSPDoubleSplitComplex *a, vDSP_Stride ia, const DSPDoubleSplitComplex *c, vDSP_Stride ic, vDSP_Length log2n, FFTDirection d) {
    const long n = 1L << log2n;
    for (long k = 0; k < n; k++) { c->realp[k * ic] = a->realp[k * ia]; c->imagp[k * ic] = a->imagp[k * ia]; }
    fftDouble(c->realp, c->imagp, ic, (unsigned)log2n, d);
}
void vDSP_fft_zoptD(FFTSetupD s, const DSPDoubleSplitComplex *a, vDSP_Stride ia, const DSPDoubleSplitComplex *c, vDSP_Stride ic, const DSPDoubleSplitComplex *t, vDSP_Length log2n, FFTDirection d) { vDSP_fft_zopD(s, a, ia, c, ic, log2n, d); }
// Real out of place: the in-place real FFT of Reef's vDSP on a copy (packed DC and Nyquist, scaled by 2 forward)
void vDSP_fft_zrop(FFTSetup s, const DSPSplitComplex *a, vDSP_Stride ia, const DSPSplitComplex *c, vDSP_Stride ic, vDSP_Length log2n, FFTDirection d) {
    const long half = 1L << (log2n - 1);
    for (long k = 0; k < half; k++) { c->realp[k * ic] = a->realp[k * ia]; c->imagp[k * ic] = a->imagp[k * ia]; }
    vDSP_fft_zrip(s, c, ic, log2n, d);
}

// Wiener-Levinson: solves the Toeplitz system with autocorrelation A (L values) for the filter F from C;
// P receives the prediction error filter. IERR 0 on success.
void vDSP_wiener(vDSP_Length l, const float *a, const float *c, float *f, float *p, int iflg, int *ierr) {
    *ierr = 0;
    if (!l || a[0] == 0) { *ierr = 1; return; }
    double *af = calloc(l, sizeof *af), *tmp = calloc(l, sizeof *tmp), *ff = calloc(l, sizeof *ff);
    double err = a[0];
    af[0] = 1;
    ff[0] = c[0] / a[0];
    for (vDSP_Length m = 1; m < l; m++) {
        double k = 0;
        for (vDSP_Length j = 0; j < m; j++) k += af[j] * a[m - j];
        k = -k / err;
        for (vDSP_Length j = 0; j <= m; j++) tmp[j] = af[j] + k * (j ? af[m - j] : 0) * 1.0;
        tmp[m] = k;
        for (vDSP_Length j = 1; j < m; j++) tmp[j] = af[j] + k * af[m - j];
        memcpy(af, tmp, (m + 1) * sizeof *af);
        err *= 1 - k * k;
        if (err <= 0) { *ierr = 1; break; }
        double g = c[m];
        for (vDSP_Length j = 0; j < m; j++) g -= ff[j] * a[m - j];
        double q = g / err;
        for (vDSP_Length j = 0; j <= m; j++) ff[j] += q * af[m - j];
    }
    for (vDSP_Length j = 0; j < l; j++) { f[j] = (float)ff[j]; if (p) p[j] = (float)af[j]; }
    free(af); free(tmp); free(ff);
}
