// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
#define FAT_MAGIC 0xcafebabe
#define FAT_CIGAM 0xbebafeca
struct fat_header { uint32_t magic; uint32_t nfat_arch; };
struct fat_arch { int32_t cputype; int32_t cpusubtype; uint32_t offset; uint32_t size; uint32_t align; };
