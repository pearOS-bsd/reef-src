// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
extern int *__error(void);
#define errno (*__error())
#define EPERM 1
#define ENOENT 2
#define EINTR 4
#define EIO 5
#define EBADF 9
#define EAGAIN 35
#define ENOMEM 12
#define EACCES 13
#define EEXIST 17
#define EINVAL 22
#define ERANGE 34
#define EDOM 33
#define ENOSYS 78
#define ETIMEDOUT 60
#define EBUSY 16
#define EDEADLK 11
#define EOVERFLOW 84
#define EILSEQ 92
#define ENOTSUP 45
#ifdef __cplusplus
}
#endif
