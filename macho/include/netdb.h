// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <netinet/in.h>
struct hostent { char *h_name; char **h_aliases; int h_addrtype; int h_length; char **h_addr_list; };
#define h_addr h_addr_list[0]
struct servent { char *s_name; char **s_aliases; int s_port; char *s_proto; };
struct addrinfo { int ai_flags; int ai_family; int ai_socktype; int ai_protocol; socklen_t ai_addrlen; char *ai_canonname; struct sockaddr *ai_addr; struct addrinfo *ai_next; };
#define AI_PASSIVE 0x1
#define AI_CANONNAME 0x2
#define AI_NUMERICHOST 0x4
#define AI_NUMERICSERV 0x1000
#define AI_ADDRCONFIG 0x400
#define NI_NUMERICHOST 0x2
#define NI_NUMERICSERV 0x8
#define NI_MAXHOST 1025
#define NI_MAXSERV 32
#define EAI_AGAIN 2
#define EAI_NONAME 8
#define EAI_SYSTEM 11
#define NETDB_INTERNAL (-1)
#define HOST_NOT_FOUND 1
extern int h_errno;
struct hostent *gethostbyname(const char *); struct hostent *gethostbyaddr(const void *, socklen_t, int);
struct servent *getservbyname(const char *, const char *);
int getaddrinfo(const char *, const char *, const struct addrinfo *, struct addrinfo **); void freeaddrinfo(struct addrinfo *);
int getnameinfo(const struct sockaddr *, socklen_t, char *, socklen_t, char *, socklen_t, int); const char *gai_strerror(int);
#ifdef __cplusplus
}
#endif
