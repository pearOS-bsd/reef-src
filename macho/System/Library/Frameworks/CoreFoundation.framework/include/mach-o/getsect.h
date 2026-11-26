// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <mach-o/loader.h>
const struct segment_command *getsegbyname(const char *segname);
const struct section *getsectbyname(const char *segname, const char *sectname);
uint8_t *getsectiondata(const struct mach_header_64 *mhp, const char *segname, const char *sectname, unsigned long *size);
uint8_t *getsegmentdata(const struct mach_header_64 *mhp, const char *segname, unsigned long *size);
