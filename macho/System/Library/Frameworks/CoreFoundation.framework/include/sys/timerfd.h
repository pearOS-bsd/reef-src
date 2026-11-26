// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <time.h>
#define TFD_CLOEXEC 0x80000
#define TFD_NONBLOCK 0x800
#define TFD_TIMER_ABSTIME 1
struct itimerspec { struct timespec it_interval; struct timespec it_value; };
int timerfd_create(int, int); int timerfd_settime(int, int, const struct itimerspec *, struct itimerspec *); int timerfd_gettime(int, struct itimerspec *);
