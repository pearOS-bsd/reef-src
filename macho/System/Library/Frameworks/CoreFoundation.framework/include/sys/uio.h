// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <sys/types.h>
struct iovec { void *iov_base; size_t iov_len; };
ssize_t readv(int, const struct iovec *, int); ssize_t writev(int, const struct iovec *, int);
