// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
struct mach_header;
struct mach_header_64 { uint32_t magic; int32_t cputype, cpusubtype; uint32_t filetype, ncmds, sizeofcmds, flags, reserved; };
typedef struct { uint64_t sectname_unused; } __dyld_dummy;
void _dyld_register_func_for_add_image(void (*)(const struct mach_header *, intptr_t));
void _dyld_register_func_for_remove_image(void (*)(const struct mach_header *, intptr_t));
uint32_t _dyld_image_count(void);
const struct mach_header *_dyld_get_image_header(uint32_t);
intptr_t _dyld_get_image_vmaddr_slide(uint32_t);
const char *_dyld_get_image_name(uint32_t);
#ifdef __cplusplus
}
#endif
