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
struct utimbuf { time_t actime; time_t modtime; };
int utime(const char *, const struct utimbuf *);
#ifdef __cplusplus
}
#endif
