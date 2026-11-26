// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <sys/time.h>
#define FD_SETSIZE 1024
typedef int __int32_t;
typedef struct fd_set { __int32_t fds_bits[FD_SETSIZE / 32]; } fd_set;
#define FD_SET(n, p) ((p)->fds_bits[(unsigned long)(n) / 32] |= (1U << ((unsigned long)(n) % 32)))
#define FD_CLR(n, p) ((p)->fds_bits[(unsigned long)(n) / 32] &= ~(1U << ((unsigned long)(n) % 32)))
#define FD_ISSET(n, p) ((p)->fds_bits[(unsigned long)(n) / 32] & (1U << ((unsigned long)(n) % 32)))
#define FD_ZERO(p) __builtin_bzero(p, sizeof(*(p)))
int select(int, fd_set *, fd_set *, fd_set *, struct timeval *) __asm("_select");
#ifdef __cplusplus
}
#endif
