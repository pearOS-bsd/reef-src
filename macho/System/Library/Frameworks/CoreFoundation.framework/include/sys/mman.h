// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <sys/types.h>
#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define MAP_SHARED 1
#define MAP_PRIVATE 2
#define MAP_FIXED 0x10
#define MAP_FILE 0
#define MAP_RESILIENT_CODESIGN 0x2000
#define MAP_ANON 0x1000
#define MAP_ANONYMOUS MAP_ANON
#define MAP_FAILED ((void *)-1)
#define MADV_NORMAL 0
#define MADV_RANDOM 1
#define MADV_SEQUENTIAL 2
#define MADV_WILLNEED 3
#define MADV_DONTNEED 4
void *mmap(void *, size_t, int, int, int, off_t); int munmap(void *, size_t); int madvise(void *, size_t, int); int mprotect(void *, size_t, int); int msync(void *, size_t, int);
