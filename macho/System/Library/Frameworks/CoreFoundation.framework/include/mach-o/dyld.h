// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
#include <mach-o/loader.h>

extern uint32_t _dyld_image_count(void);
extern const struct mach_header *_dyld_get_image_header(uint32_t image_index);
extern intptr_t _dyld_get_image_vmaddr_slide(uint32_t image_index);
extern const char *_dyld_get_image_name(uint32_t image_index);
