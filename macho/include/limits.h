// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#define CHAR_BIT 8
#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX - 1)
#define UINT_MAX 4294967295U
#define LONG_MAX 9223372036854775807L
#define LONG_MIN (-LONG_MAX - 1L)
#define ULONG_MAX 18446744073709551615UL
#define SHRT_MAX 32767
#define UCHAR_MAX 255
#define PATH_MAX 1024
#define MB_LEN_MAX 1
#define NAME_MAX 255
#define SSIZE_MAX LONG_MAX
#define LLONG_MAX 9223372036854775807LL
#define LLONG_MIN (-LLONG_MAX - 1LL)
#define ULLONG_MAX 18446744073709551615ULL
#define SCHAR_MAX 127
#define SCHAR_MIN (-128)
#define CHAR_MAX 127
#define CHAR_MIN (-128)
#define SHRT_MIN (-32768)
#define USHRT_MAX 65535
