// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX 0x7fffffff
#define MB_CUR_MAX 1
void *malloc(size_t); void *calloc(size_t, size_t); void *realloc(void *, size_t); void free(void *);
int posix_memalign(void **, size_t, size_t); void *aligned_alloc(size_t, size_t); void *valloc(size_t);
void abort(void) __attribute__((noreturn)); void exit(int) __attribute__((noreturn)); void _Exit(int) __attribute__((noreturn));
void quick_exit(int) __attribute__((noreturn)); int atexit(void (*)(void)); int at_quick_exit(void (*)(void));
char *getenv(const char *); int setenv(const char *, const char *, int); int unsetenv(const char *); int putenv(char *); int system(const char *);
int atoi(const char *); long atol(const char *); long long atoll(const char *); double atof(const char *);
double strtod(const char *, char **); float strtof(const char *, char **); long double strtold(const char *, char **);
long strtol(const char *, char **, int); unsigned long strtoul(const char *, char **, int);
long long strtoll(const char *, char **, int); unsigned long long strtoull(const char *, char **, int);
int abs(int); long labs(long); long long llabs(long long);
div_t div(int, int); ldiv_t ldiv(long, long); lldiv_t lldiv(long long, long long);
int rand(void); void srand(unsigned); void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
void *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
int mblen(const char *, size_t); size_t mbstowcs(wchar_t *, const char *, size_t); int mbtowc(wchar_t *, const char *, size_t);
size_t wcstombs(char *, const wchar_t *, size_t); int wctomb(char *, wchar_t);
int arc4random_uniform(unsigned); unsigned arc4random(void); void arc4random_buf(void *, size_t);
char *realpath(const char *, char *) __asm("_realpath$DARWIN_EXTSN");
#ifdef __cplusplus
}
#endif
