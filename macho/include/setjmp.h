// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef int jmp_buf[37]; typedef int sigjmp_buf[37 + 1];
int setjmp(jmp_buf); void longjmp(jmp_buf, int) __attribute__((noreturn));
#ifdef __cplusplus
}
#endif
