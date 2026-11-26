// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
struct utsname { char sysname[256]; char nodename[256]; char release[256]; char version[256]; char machine[256]; };
int uname(struct utsname *);
#ifdef __cplusplus
}
#endif
