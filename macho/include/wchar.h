// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef int wint_t;
typedef union { char __mbstate8[128]; long long _mbstateL; } __mbstate_t;
typedef __mbstate_t mbstate_t;
#define WEOF ((wint_t)-1)
#define WCHAR_MIN (-2147483647 - 1)
#define WCHAR_MAX 2147483647
struct tm;
typedef struct __sFILE __FILE_wchar;
size_t wcslen(const wchar_t *); int wcscmp(const wchar_t *, const wchar_t *); int wcsncmp(const wchar_t *, const wchar_t *, size_t);
int wcscoll(const wchar_t *, const wchar_t *); size_t wcsxfrm(wchar_t *, const wchar_t *, size_t);
wchar_t *wcscpy(wchar_t *, const wchar_t *); wchar_t *wcsncpy(wchar_t *, const wchar_t *, size_t); wchar_t *wcscat(wchar_t *, const wchar_t *); wchar_t *wcsncat(wchar_t *, const wchar_t *, size_t);
wchar_t *wcschr(const wchar_t *, wchar_t); wchar_t *wcsrchr(const wchar_t *, wchar_t); wchar_t *wcsstr(const wchar_t *, const wchar_t *); wchar_t *wcspbrk(const wchar_t *, const wchar_t *);
size_t wcsspn(const wchar_t *, const wchar_t *); size_t wcscspn(const wchar_t *, const wchar_t *); wchar_t *wcstok(wchar_t *, const wchar_t *, wchar_t **);
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t); int wmemcmp(const wchar_t *, const wchar_t *, size_t); wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t); wchar_t *wmemset(wchar_t *, wchar_t, size_t);
wint_t btowc(int); int wctob(wint_t); int mbsinit(const mbstate_t *);
size_t mbrlen(const char *, size_t, mbstate_t *); size_t mbrtowc(wchar_t *, const char *, size_t, mbstate_t *); size_t wcrtomb(char *, wchar_t, mbstate_t *);
size_t mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *); size_t wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);
size_t mbsnrtowcs(wchar_t *, const char **, size_t, size_t, mbstate_t *); size_t wcsnrtombs(char *, const wchar_t **, size_t, size_t, mbstate_t *);
double wcstod(const wchar_t *, wchar_t **); float wcstof(const wchar_t *, wchar_t **); long double wcstold(const wchar_t *, wchar_t **);
long wcstol(const wchar_t *, wchar_t **, int); unsigned long wcstoul(const wchar_t *, wchar_t **, int);
long long wcstoll(const wchar_t *, wchar_t **, int); unsigned long long wcstoull(const wchar_t *, wchar_t **, int);
size_t wcsftime(wchar_t *, size_t, const wchar_t *, const struct tm *);
int swprintf(wchar_t *, size_t, const wchar_t *, ...); int vswprintf(wchar_t *, size_t, const wchar_t *, va_list);
int swscanf(const wchar_t *, const wchar_t *, ...); int vswscanf(const wchar_t *, const wchar_t *, va_list);
wint_t getwc(struct __sFILE *); wint_t putwc(wchar_t, struct __sFILE *); wint_t ungetwc(wint_t, struct __sFILE *); wint_t fgetwc(struct __sFILE *); wint_t fputwc(wchar_t, struct __sFILE *);
wint_t getwchar(void); wint_t putwchar(wchar_t); int fwide(struct __sFILE *, int);
wchar_t *fgetws(wchar_t *, int, struct __sFILE *); int fputws(const wchar_t *, struct __sFILE *);
int fwprintf(struct __sFILE *, const wchar_t *, ...); int wprintf(const wchar_t *, ...); int vwprintf(const wchar_t *, va_list); int vfwprintf(struct __sFILE *, const wchar_t *, va_list);
int fwscanf(struct __sFILE *, const wchar_t *, ...); int wscanf(const wchar_t *, ...); int vfwscanf(struct __sFILE *, const wchar_t *, va_list); int vwscanf(const wchar_t *, va_list);
#ifdef __cplusplus
}
#endif
