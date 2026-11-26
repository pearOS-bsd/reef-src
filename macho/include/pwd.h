// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <time.h>
struct passwd { char *pw_name; char *pw_passwd; uid_t pw_uid; gid_t pw_gid; time_t pw_change; char *pw_class; char *pw_gecos; char *pw_dir; char *pw_shell; time_t pw_expire; };
struct passwd *getpwnam(const char *); struct passwd *getpwuid(uid_t);
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **); int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
#ifdef __cplusplus
}
#endif
