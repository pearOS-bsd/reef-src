// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <sys/types.h>
#include <sys/time.h>
#define FD_SETSIZE 1024
typedef int __int32_t;
typedef __int32_t fd_mask;
#define NFDBITS (sizeof(__int32_t) * 8)
typedef struct fd_set { __int32_t fds_bits[FD_SETSIZE / 32]; } fd_set;
#define FD_SET(n, p) ((p)->fds_bits[(unsigned long)(n) / NFDBITS] |= (1U << ((unsigned long)(n) % NFDBITS)))
#define FD_CLR(n, p) ((p)->fds_bits[(unsigned long)(n) / NFDBITS] &= ~(1U << ((unsigned long)(n) % NFDBITS)))
#define FD_ISSET(n, p) ((p)->fds_bits[(unsigned long)(n) / NFDBITS] & (1U << ((unsigned long)(n) % NFDBITS)))
#define FD_ZERO(p) __builtin_bzero(p, sizeof(*(p)))
int select(int, fd_set *, fd_set *, fd_set *, struct timeval *) __asm("_select");
