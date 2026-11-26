// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void __assert_rtn(const char *, const char *, int, const char *) __attribute__((noreturn));
#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#define assert(e) (__builtin_expect(!(e), 0) ? __assert_rtn(__func__, __FILE__, __LINE__, #e) : (void)0)
#endif
#ifdef __cplusplus
}
#endif
