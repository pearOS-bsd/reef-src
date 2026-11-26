// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
typedef unsigned int fsblkcnt_t; typedef unsigned int fsfilcnt_t;
struct statvfs { unsigned long f_bsize, f_frsize; fsblkcnt_t f_blocks, f_bfree, f_bavail; fsfilcnt_t f_files, f_ffree, f_favail; unsigned long f_fsid, f_flag, f_namemax; };
int statvfs(const char *, struct statvfs *);
#ifdef __cplusplus
}
#endif
