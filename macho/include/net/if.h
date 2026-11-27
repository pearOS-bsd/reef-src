// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Darwin <net/if.h>: the declarations the rebuilt libraries use.
#ifndef _NET_IF_H_
#define _NET_IF_H_
#define IF_NAMESIZE 16
unsigned int if_nametoindex(const char *);
char *if_indextoname(unsigned int, char *);
#endif
