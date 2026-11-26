// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
#include <mach/machine.h>
struct mach_header { uint32_t magic; cpu_type_t cputype; cpu_subtype_t cpusubtype; uint32_t filetype; uint32_t ncmds; uint32_t sizeofcmds; uint32_t flags; };
struct mach_header_64 { uint32_t magic; cpu_type_t cputype; cpu_subtype_t cpusubtype; uint32_t filetype; uint32_t ncmds; uint32_t sizeofcmds; uint32_t flags; uint32_t reserved; };
#define MH_MAGIC 0xfeedface
#define MH_CIGAM 0xcefaedfe
#define MH_MAGIC_64 0xfeedfacf
#define MH_CIGAM_64 0xcffaedfe
#define MH_OBJECT 1
#define MH_EXECUTE 2
#define MH_DYLIB 6
#define MH_DYLINKER 7
#define MH_BUNDLE 8
#define MH_CORE 4
struct load_command { uint32_t cmd; uint32_t cmdsize; };
#define LC_REQ_DYLD 0x80000000
#define LC_SEGMENT 0x1
#define LC_SEGMENT_64 0x19
#define LC_LOAD_DYLIB 0xc
#define LC_ID_DYLIB 0xd
#define LC_MAIN (0x28 | LC_REQ_DYLD)
#define LC_UUID 0x1b
union lc_str { uint32_t offset; };
struct segment_command { uint32_t cmd, cmdsize; char segname[16]; uint32_t vmaddr, vmsize, fileoff, filesize; int32_t maxprot, initprot; uint32_t nsects, flags; };
struct segment_command_64 { uint32_t cmd, cmdsize; char segname[16]; uint64_t vmaddr, vmsize, fileoff, filesize; int32_t maxprot, initprot; uint32_t nsects, flags; };
struct section { char sectname[16]; char segname[16]; uint32_t addr, size, offset, align, reloff, nreloc, flags, reserved1, reserved2; };
struct section_64 { char sectname[16]; char segname[16]; uint64_t addr, size; uint32_t offset, align, reloff, nreloc, flags, reserved1, reserved2, reserved3; };
struct dylib { union lc_str name; uint32_t timestamp; uint32_t current_version; uint32_t compatibility_version; };
struct dylib_command { uint32_t cmd; uint32_t cmdsize; struct dylib dylib; };
struct uuid_command { uint32_t cmd; uint32_t cmdsize; uint8_t uuid[16]; };
