// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
typedef unsigned int socklen_t; typedef unsigned char sa_family_t;
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_INET 2
#define AF_INET6 30
#define PF_UNSPEC AF_UNSPEC
#define PF_INET AF_INET
#define PF_INET6 AF_INET6
#define SOL_SOCKET 0xffff
#define SO_DEBUG 0x1
#define SO_ACCEPTCONN 0x2
#define SO_REUSEADDR 0x4
#define SO_KEEPALIVE 0x8
#define SO_DONTROUTE 0x10
#define SO_BROADCAST 0x20
#define SO_LINGER 0x80
#define SO_OOBINLINE 0x100
#define SO_REUSEPORT 0x200
#define SO_SNDBUF 0x1001
#define SO_RCVBUF 0x1002
#define SO_ERROR 0x1007
#define SO_TYPE 0x1008
#define SOMAXCONN 128
#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
#define MSG_OOB 0x1
#define MSG_PEEK 0x2
struct sockaddr { unsigned char sa_len; sa_family_t sa_family; char sa_data[14]; };
struct sockaddr_storage { unsigned char ss_len; sa_family_t ss_family; char __ss_pad1[6]; long long __ss_align; char __ss_pad2[112]; };
int socket(int, int, int); int bind(int, const struct sockaddr *, socklen_t) __asm("_bind"); int connect(int, const struct sockaddr *, socklen_t);
int listen(int, int); int accept(int, struct sockaddr *, socklen_t *); int getsockname(int, struct sockaddr *, socklen_t *); int getpeername(int, struct sockaddr *, socklen_t *);
int getsockopt(int, int, int, void *, socklen_t *); int setsockopt(int, int, int, const void *, socklen_t); int shutdown(int, int);
ssize_t recv(int, void *, size_t, int); ssize_t send(int, const void *, size_t, int);
#ifdef __cplusplus
}
#endif
