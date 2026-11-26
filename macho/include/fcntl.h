// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_NONBLOCK 4
#define O_APPEND 8
#define O_CREAT 0x200
#define O_TRUNC 0x400
#define O_EXCL 0x800
#define O_NOCTTY 0x20000
#define O_SYNC 0x80
#define O_NDELAY O_NONBLOCK
#define O_DSYNC 0x400000
#define O_CLOEXEC 0x1000000
#define O_DIRECTORY 0x100000
#define O_NOFOLLOW 0x100
#define AT_FDCWD (-2)
#define AT_SYMLINK_NOFOLLOW 0x20
#define AT_REMOVEDIR 0x80
#define F_GETFL 3
#define F_SETFL 4
#define F_GETFD 1
#define F_SETFD 2
#define FD_CLOEXEC 1
#define F_NOCACHE 48
#define F_FULLFSYNC 51
int open(const char *, int, ...); int openat(int, const char *, int, ...); int fcntl(int, int, ...); int creat(const char *, mode_t);
#ifdef __cplusplus
}
#endif
