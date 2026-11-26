// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <time.h>
struct timeval { time_t tv_sec; int tv_usec; };
struct timezone { int tz_minuteswest, tz_dsttime; };
int gettimeofday(struct timeval *, void *); int utimes(const char *, const struct timeval[2]); int futimes(int, const struct timeval[2]);
#ifdef __cplusplus
}
#endif
