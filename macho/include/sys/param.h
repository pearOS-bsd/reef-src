// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <limits.h>
#define MAXPATHLEN 1024
#define MAXHOSTNAMELEN 256
#define NBBY 8
#define MAXSYMLINKS 32
#ifdef __cplusplus
}
#endif
