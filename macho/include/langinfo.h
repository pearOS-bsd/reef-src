// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <nl_types.h>
#include <xlocale.h>
#define CODESET 0
#define D_T_FMT 1
#define D_FMT 2
#define T_FMT 3
#define AM_STR 5
#define PM_STR 6
#define DAY_1 7
#define ABDAY_1 14
#define MON_1 21
#define ABMON_1 33
#define RADIXCHAR 50
#define THOUSEP 51
#define T_FMT_AMPM 4
char *nl_langinfo(nl_item); char *nl_langinfo_l(nl_item, locale_t);
#ifdef __cplusplus
}
#endif
