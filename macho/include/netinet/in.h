// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/socket.h>
typedef unsigned short in_port_t; typedef unsigned int in_addr_t;
#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define IPPROTO_IPV6 41
#define IPV6_V6ONLY 27
#define INADDR_ANY ((in_addr_t)0x00000000)
#define INADDR_LOOPBACK ((in_addr_t)0x7f000001)
#define INADDR_NONE 0xffffffff
struct in_addr { in_addr_t s_addr; };
struct sockaddr_in { unsigned char sin_len; sa_family_t sin_family; in_port_t sin_port; struct in_addr sin_addr; char sin_zero[8]; };
struct in6_addr { union { unsigned char __u6_addr8[16]; unsigned short __u6_addr16[8]; unsigned int __u6_addr32[4]; } __u6_addr; };
#define s6_addr __u6_addr.__u6_addr8
struct sockaddr_in6 { unsigned char sin6_len; sa_family_t sin6_family; in_port_t sin6_port; unsigned int sin6_flowinfo; struct in6_addr sin6_addr; unsigned int sin6_scope_id; };
extern const struct in6_addr in6addr_any;
#define IN6_IS_ADDR_V4MAPPED(a) ((a)->__u6_addr.__u6_addr32[0] == 0 && (a)->__u6_addr.__u6_addr32[1] == 0 && (a)->__u6_addr.__u6_addr32[2] == htonl(0x0000ffff))
#define IN6_ARE_ADDR_EQUAL(a, b) (__builtin_memcmp(&(a)->s6_addr[0], &(b)->s6_addr[0], sizeof(struct in6_addr)) == 0)
#define IN6ADDR_ANY_INIT {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}}
#define htons(x) __builtin_bswap16(x)
#define ntohs(x) __builtin_bswap16(x)
#define htonl(x) __builtin_bswap32(x)
#define ntohl(x) __builtin_bswap32(x)
#ifdef __cplusplus
}
#endif
