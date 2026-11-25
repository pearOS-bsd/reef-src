// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// LD_PRELOAD: every look at a clock is a tenth of a millisecond later, so that what depends on time is the same in each run.
#include <time.h>

static long long now = 1000000000000LL;

int clock_gettime(clockid_t clock, struct timespec* time)
{
    (void)clock;
    now += 100000;
    time->tv_sec = now / 1000000000;
    time->tv_nsec = now % 1000000000;
    return 0;
}
