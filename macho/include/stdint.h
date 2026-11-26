// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
typedef signed char int8_t; typedef short int16_t; typedef int int32_t; typedef long long int64_t;
typedef unsigned char uint8_t; typedef unsigned short uint16_t; typedef unsigned int uint32_t; typedef unsigned long long uint64_t;
typedef long intptr_t; typedef unsigned long uintptr_t; typedef long long intmax_t; typedef unsigned long long uintmax_t;
#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL
#define UINT8_MAX 255
#define UINT16_MAX 65535
#define UINT32_MAX 4294967295U
#define UINT64_MAX 18446744073709551615ULL
#define INT32_MIN (-INT32_MAX - 1)
#define INT64_MIN (-INT64_MAX - 1)
#define SIZE_MAX UINT64_MAX
typedef int8_t int_least8_t; typedef int16_t int_least16_t; typedef int32_t int_least32_t; typedef int64_t int_least64_t;
typedef uint8_t uint_least8_t; typedef uint16_t uint_least16_t; typedef uint32_t uint_least32_t; typedef uint64_t uint_least64_t;
typedef int8_t int_fast8_t; typedef int16_t int_fast16_t; typedef int32_t int_fast32_t; typedef int64_t int_fast64_t;
typedef uint8_t uint_fast8_t; typedef uint16_t uint_fast16_t; typedef uint32_t uint_fast32_t; typedef uint64_t uint_fast64_t;
#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX
#define INTMAX_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX
#define PTRDIFF_MIN INT64_MIN
#define SIG_ATOMIC_MAX INT32_MAX
#define INT8_C(c) c
#define INT16_C(c) c
#define INT32_C(c) c
#define INT64_C(c) c##LL
#define UINT8_C(c) c
#define UINT16_C(c) c
#define UINT32_C(c) c##U
#define UINT64_C(c) c##ULL
#define INTMAX_C(c) c##LL
#define UINTMAX_C(c) c##ULL
