/* SPDX-FileCopyrightText: 2026 Alexandru Balan */
/* SPDX-License-Identifier: BSD-2-Clause */
/* The part of Darwin's <iconv.h> that the libxml2 headers need. */
#ifndef _LIBICONV_H
#define _LIBICONV_H
#include <stddef.h>
typedef void *iconv_t;
iconv_t iconv_open(const char *, const char *);
size_t iconv(iconv_t, char **, size_t *, char **, size_t *);
int iconv_close(iconv_t);
#endif
