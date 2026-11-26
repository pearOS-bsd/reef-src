// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include_next <sys/socket.h>
#define PF_LOCAL AF_UNIX
#define PF_UNIX AF_UNIX
int socketpair(int, int, int, int *);
ssize_t recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
#define SO_SNDTIMEO 0x1005
#define SO_RCVTIMEO 0x1006
