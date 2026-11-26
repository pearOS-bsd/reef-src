// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
#define EFD_CLOEXEC 0x80000
#define EFD_NONBLOCK 0x800
#define EFD_SEMAPHORE 1
typedef uint64_t eventfd_t;
int eventfd(unsigned int, int); int eventfd_read(int, eventfd_t *); int eventfd_write(int, eventfd_t);
