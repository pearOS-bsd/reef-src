// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#include <stdarg.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct _xlocale *locale_t;
#define LC_COLLATE_MASK (1 << 0)
#define LC_CTYPE_MASK (1 << 1)
#define LC_MESSAGES_MASK (1 << 2)
#define LC_MONETARY_MASK (1 << 3)
#define LC_NUMERIC_MASK (1 << 4)
#define LC_TIME_MASK (1 << 5)
#define LC_ALL_MASK 63
#define LC_GLOBAL_LOCALE ((locale_t)-1)
locale_t newlocale(int, const char *, locale_t); void freelocale(locale_t); locale_t uselocale(locale_t); locale_t duplocale(locale_t);
double strtod_l(const char *, char **, locale_t); float strtof_l(const char *, char **, locale_t); long double strtold_l(const char *, char **, locale_t);
long long strtoll_l(const char *, char **, int, locale_t); unsigned long long strtoull_l(const char *, char **, int, locale_t);
int isalnum_l(int, locale_t); int isalpha_l(int, locale_t); int isblank_l(int, locale_t); int iscntrl_l(int, locale_t); int isdigit_l(int, locale_t);
int isgraph_l(int, locale_t); int islower_l(int, locale_t); int isprint_l(int, locale_t); int ispunct_l(int, locale_t); int isspace_l(int, locale_t);
int isupper_l(int, locale_t); int isxdigit_l(int, locale_t); int tolower_l(int, locale_t); int toupper_l(int, locale_t);
int iswalnum_l(wint_t, locale_t); int iswalpha_l(wint_t, locale_t); int iswblank_l(wint_t, locale_t); int iswcntrl_l(wint_t, locale_t); int iswdigit_l(wint_t, locale_t);
int iswgraph_l(wint_t, locale_t); int iswlower_l(wint_t, locale_t); int iswprint_l(wint_t, locale_t); int iswpunct_l(wint_t, locale_t); int iswspace_l(wint_t, locale_t);
int iswupper_l(wint_t, locale_t); int iswxdigit_l(wint_t, locale_t); wint_t towlower_l(wint_t, locale_t); wint_t towupper_l(wint_t, locale_t);
int strcoll_l(const char *, const char *, locale_t); size_t strxfrm_l(char *, const char *, size_t, locale_t);
int wcscoll_l(const wchar_t *, const wchar_t *, locale_t); size_t wcsxfrm_l(wchar_t *, const wchar_t *, size_t, locale_t);
size_t strftime_l(char *, size_t, const char *, const struct tm *, locale_t); size_t wcsftime_l(wchar_t *, size_t, const wchar_t *, const struct tm *, locale_t);
size_t mbrtowc_l(wchar_t *, const char *, size_t, mbstate_t *, locale_t); size_t wcrtomb_l(char *, wchar_t, mbstate_t *, locale_t);
size_t mbsrtowcs_l(wchar_t *, const char **, size_t, mbstate_t *, locale_t); size_t wcsrtombs_l(char *, const wchar_t **, size_t, mbstate_t *, locale_t);
size_t mbrlen_l(const char *, size_t, mbstate_t *, locale_t); int mbtowc_l(wchar_t *, const char *, size_t, locale_t); int mbsinit_l(const mbstate_t *, locale_t);
wint_t btowc_l(int, locale_t); int wctob_l(wint_t, locale_t);
int snprintf_l(char *, size_t, locale_t, const char *, ...); int asprintf_l(char **, locale_t, const char *, ...); int vasprintf_l(char **, locale_t, const char *, va_list);
int sscanf_l(const char *, locale_t, const char *, ...);
int ___mb_cur_max(void); int ___mb_cur_max_l(locale_t);
#define MB_CUR_MAX_L(l) ___mb_cur_max_l(l)
#ifdef __cplusplus
}
#endif
