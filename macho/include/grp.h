// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
struct group { char *gr_name; char *gr_passwd; gid_t gr_gid; char **gr_mem; };
struct group *getgrnam(const char *); struct group *getgrgid(gid_t);
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **); int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
#ifdef __cplusplus
}
#endif
