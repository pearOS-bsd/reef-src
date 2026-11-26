// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef void *nl_catd; typedef int nl_item;
#define NL_SETD 1
#define NL_CAT_LOCALE 1
nl_catd catopen(const char *, int); char *catgets(nl_catd, int, int, const char *); int catclose(nl_catd);
#ifdef __cplusplus
}
#endif
