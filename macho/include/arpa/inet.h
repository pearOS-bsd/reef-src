// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <netinet/in.h>
char *inet_ntoa(struct in_addr); in_addr_t inet_addr(const char *);
const char *inet_ntop(int, const void *, char *, socklen_t); int inet_pton(int, const char *, void *);
#ifdef __cplusplus
}
#endif
