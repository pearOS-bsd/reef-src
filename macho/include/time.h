// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef long time_t; typedef long clock_t; typedef int clockid_t;
struct timespec { time_t tv_sec; long tv_nsec; };
struct tm { int tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, tm_yday, tm_isdst; long tm_gmtoff; char *tm_zone; };
#define CLOCKS_PER_SEC 1000000
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 6
#define CLOCK_MONOTONIC_RAW 4
#define CLOCK_PROCESS_CPUTIME_ID 12
#define CLOCK_THREAD_CPUTIME_ID 16
#define TIME_UTC 1
void tzset(void); extern char *tzname[2]; clock_t clock(void); double difftime(time_t, time_t); time_t mktime(struct tm *); time_t time(time_t *);
char *asctime(const struct tm *); char *ctime(const time_t *); struct tm *gmtime(const time_t *); struct tm *localtime(const time_t *);
struct tm *gmtime_r(const time_t *, struct tm *); struct tm *localtime_r(const time_t *, struct tm *);
size_t strftime(char *, size_t, const char *, const struct tm *); char *strptime(const char *, const char *, struct tm *);
int clock_gettime(clockid_t, struct timespec *); int clock_getres(clockid_t, struct timespec *); int nanosleep(const struct timespec *, struct timespec *);
int timespec_get(struct timespec *, int);
#ifdef __cplusplus
}
#endif
