// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef int32_t OSSpinLock;
#define OS_SPINLOCK_INIT 0
typedef int64_t OSAtomic_int64_aligned64_t __attribute__((aligned(8)));
bool OSAtomicCompareAndSwapPtr(void *oldValue, void *newValue, void *volatile *theValue);
bool OSAtomicCompareAndSwapPtrBarrier(void *oldValue, void *newValue, void *volatile *theValue);
bool OSAtomicCompareAndSwapLong(long oldValue, long newValue, volatile long *theValue);
bool OSAtomicCompareAndSwap32Barrier(int32_t oldValue, int32_t newValue, volatile int32_t *theValue);
bool OSAtomicCompareAndSwap64Barrier(int64_t oldValue, int64_t newValue, volatile OSAtomic_int64_aligned64_t *theValue);
int32_t OSAtomicAdd32(int32_t amount, volatile int32_t *theValue);
int32_t OSAtomicAdd32Barrier(int32_t amount, volatile int32_t *theValue);
int64_t OSAtomicAdd64(int64_t amount, volatile OSAtomic_int64_aligned64_t *theValue);
int32_t OSAtomicIncrement32(volatile int32_t *theValue);
int32_t OSAtomicIncrement32Barrier(volatile int32_t *theValue);
int32_t OSAtomicDecrement32(volatile int32_t *theValue);
int32_t OSAtomicDecrement32Barrier(volatile int32_t *theValue);
void OSMemoryBarrier(void);
