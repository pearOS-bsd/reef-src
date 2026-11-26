// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#define RTLD_LAZY 1
#define RTLD_NOW 2
#define RTLD_NOLOAD 0x10
#define RTLD_DEFAULT ((void *)-2)
typedef struct dl_info { const char *dli_fname; void *dli_fbase; const char *dli_sname; void *dli_saddr; } Dl_info;
void *dlopen(const char *, int);
void *dlsym(void *, const char *);
int dlclose(void *);
char *dlerror(void);
int dladdr(const void *, Dl_info *);
#ifdef __cplusplus
}
#endif
