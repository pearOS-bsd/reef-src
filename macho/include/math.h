// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5
#define HUGE_VAL __builtin_huge_val()
#define HUGE_VALF __builtin_huge_valf()
#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")
#define fpclassify(x) __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)
#define isfinite(x) __builtin_isfinite(x)
#define isinf(x) __builtin_isinf(x)
#define isnan(x) __builtin_isnan(x)
#define isnormal(x) __builtin_isnormal(x)
#define signbit(x) __builtin_signbit(x)
#define isgreater(x, y) __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isless(x, y) __builtin_isless(x, y)
#define islessequal(x, y) __builtin_islessequal(x, y)
#define islessgreater(x, y) __builtin_islessgreater(x, y)
#define isunordered(x, y) __builtin_isunordered(x, y)
typedef float float_t; typedef double double_t;
double acos(double); double asin(double); double atan(double); double atan2(double, double); double ceil(double); double cos(double); double cosh(double);
double exp(double); double fabs(double); double floor(double); double fmod(double, double); double frexp(double, int *); double ldexp(double, int);
double log(double); double log10(double); double modf(double, double *); double pow(double, double); double sin(double); double sinh(double); double sqrt(double);
double tan(double); double hypot(double, double); double expm1(double); double log1p(double); double cbrt(double); double acosh(double); double asinh(double); double atanh(double); double copysign(double, double); double nearbyint(double); double rint(double); double erf(double); double erfc(double); double lgamma(double); double tgamma(double); double exp2(double); double log2(double); double scalbn(double, int); double nan(const char *); double fdim(double, double); double fma(double, double, double); double remainder(double, double); long lround(double); long long llround(double); long lrint(double); double tanh(double); double round(double); double trunc(double); double fmax(double, double); double fmin(double, double);
float fabsf(float); float floorf(float); float ceilf(float); float sqrtf(float); float powf(float, float); long double fabsl(long double); long double ldexpl(long double, int);
long double frexpl(long double, int *); long double powl(long double, long double); long double floorl(long double);
#ifdef __cplusplus
}
#endif
