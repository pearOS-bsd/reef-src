// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
typedef long ssize_t; typedef int pid_t; typedef unsigned int uid_t; typedef unsigned int gid_t; typedef unsigned short mode_t; typedef long long off_t;
typedef int dev_t; typedef unsigned long long ino_t; typedef unsigned short nlink_t; typedef int blksize_t; typedef long long blkcnt_t;
typedef unsigned char u_char; typedef unsigned short u_short; typedef unsigned int u_int; typedef unsigned long u_long;
#ifdef __cplusplus
}
#endif
