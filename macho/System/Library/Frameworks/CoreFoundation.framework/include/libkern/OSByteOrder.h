// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
#define OSSwapInt16(x) __builtin_bswap16(x)
#define OSSwapInt32(x) __builtin_bswap32(x)
#define OSSwapInt64(x) __builtin_bswap64(x)
#define OSSwapHostToBigInt16(x) __builtin_bswap16(x)
#define OSSwapHostToBigInt32(x) __builtin_bswap32(x)
#define OSSwapHostToBigInt64(x) __builtin_bswap64(x)
#define OSSwapBigToHostInt16(x) __builtin_bswap16(x)
#define OSSwapBigToHostInt32(x) __builtin_bswap32(x)
#define OSSwapBigToHostInt64(x) __builtin_bswap64(x)
#define OSSwapHostToLittleInt16(x) (x)
#define OSSwapHostToLittleInt32(x) (x)
#define OSSwapHostToLittleInt64(x) (x)
#define OSSwapLittleToHostInt16(x) (x)
#define OSSwapLittleToHostInt32(x) (x)
#define OSSwapLittleToHostInt64(x) (x)
enum { OSUnknownByteOrder, OSLittleEndian, OSBigEndian };
static inline int32_t OSHostByteOrder(void) { return OSLittleEndian; }
