// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// The parts of the Mach-O file format the loader reads, written from the format's documentation.

#pragma once

#include <cstdint>

namespace cutefish::reef {

constexpr uint32_t MH_MAGIC_64 = 0xfeedfacf;
constexpr uint32_t FAT_CIGAM = 0xbebafeca;
constexpr uint32_t FAT_CIGAM_64 = 0xbfbafeca;
constexpr int32_t CPU_TYPE_X86_64 = 0x01000007;
constexpr int32_t CPU_TYPE_ARM64 = 0x0100000c;

constexpr uint32_t MH_EXECUTE = 2;
constexpr uint32_t MH_DYLIB = 6;
constexpr uint32_t MH_BUNDLE = 8;
constexpr uint32_t MH_PIE = 0x200000;

constexpr uint32_t LC_REQ_DYLD = 0x80000000;
constexpr uint32_t LC_SYMTAB = 0x2;
constexpr uint32_t LC_UNIXTHREAD = 0x5;
constexpr uint32_t LC_LOAD_DYLIB = 0xc;
constexpr uint32_t LC_ID_DYLIB = 0xd;
constexpr uint32_t LC_LOAD_WEAK_DYLIB = 0x18 | LC_REQ_DYLD;
constexpr uint32_t LC_SEGMENT_64 = 0x19;
constexpr uint32_t LC_RPATH = 0x1c | LC_REQ_DYLD;
constexpr uint32_t LC_REEXPORT_DYLIB = 0x1f | LC_REQ_DYLD;
constexpr uint32_t LC_LAZY_LOAD_DYLIB = 0x20;
constexpr uint32_t LC_DYLD_INFO = 0x22;
constexpr uint32_t LC_DYLD_INFO_ONLY = 0x22 | LC_REQ_DYLD;
constexpr uint32_t LC_LOAD_UPWARD_DYLIB = 0x23 | LC_REQ_DYLD;
constexpr uint32_t LC_MAIN = 0x28 | LC_REQ_DYLD;
constexpr uint32_t LC_DYLD_EXPORTS_TRIE = 0x33 | LC_REQ_DYLD;
constexpr uint32_t LC_DYLD_CHAINED_FIXUPS = 0x34 | LC_REQ_DYLD;

constexpr int VM_PROT_READ = 1;
constexpr int VM_PROT_WRITE = 2;
constexpr int VM_PROT_EXECUTE = 4;

constexpr uint32_t x86_THREAD_STATE64 = 4;

struct mach_header_64
{
    uint32_t magic;
    int32_t cputype;
    int32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
};

struct load_command
{
    uint32_t cmd;
    uint32_t cmdsize;
};

struct segment_command_64
{
    uint32_t cmd;
    uint32_t cmdsize;
    char segname[16];
    uint64_t vmaddr;
    uint64_t vmsize;
    uint64_t fileoff;
    uint64_t filesize;
    int32_t maxprot;
    int32_t initprot;
    uint32_t nsects;
    uint32_t flags;
};

struct section_64
{
    char sectname[16];
    char segname[16];
    uint64_t addr;
    uint64_t size;
    uint32_t offset;
    uint32_t align;
    uint32_t reloff;
    uint32_t nreloc;
    uint32_t flags;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
};

struct symtab_command
{
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t symoff;
    uint32_t nsyms;
    uint32_t stroff;
    uint32_t strsize;
};

struct dylib_command
{
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t name;
    uint32_t timestamp;
    uint32_t currentVersion;
    uint32_t compatibilityVersion;
};

struct rpath_command
{
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t path;
};

struct dyld_info_command
{
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t rebase_off;
    uint32_t rebase_size;
    uint32_t bind_off;
    uint32_t bind_size;
    uint32_t weak_bind_off;
    uint32_t weak_bind_size;
    uint32_t lazy_bind_off;
    uint32_t lazy_bind_size;
    uint32_t export_off;
    uint32_t export_size;
};

struct entry_point_command
{
    uint32_t cmd;
    uint32_t cmdsize;
    uint64_t entryoff;
    uint64_t stacksize;
};

struct linkedit_data_command
{
    uint32_t cmd;
    uint32_t cmdsize;
    uint32_t dataoff;
    uint32_t datasize;
};

struct nlist_64
{
    uint32_t n_strx;
    uint8_t n_type;
    uint8_t n_sect;
    uint16_t n_desc;
    uint64_t n_value;
};

constexpr uint8_t N_STAB = 0xe0;
constexpr uint8_t N_TYPE = 0x0e;
constexpr uint8_t N_EXT = 0x01;
constexpr uint8_t N_SECT = 0xe;

constexpr uint8_t BIND_OPCODE_MASK = 0xf0;
constexpr uint8_t BIND_IMMEDIATE_MASK = 0x0f;
constexpr uint8_t BIND_SYMBOL_FLAGS_WEAK_IMPORT = 0x1;
constexpr uint8_t BIND_OPCODE_DONE = 0x00;
constexpr uint8_t BIND_OPCODE_SET_DYLIB_ORDINAL_IMM = 0x10;
constexpr uint8_t BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB = 0x20;
constexpr uint8_t BIND_OPCODE_SET_DYLIB_SPECIAL_IMM = 0x30;
constexpr uint8_t BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM = 0x40;
constexpr uint8_t BIND_OPCODE_SET_TYPE_IMM = 0x50;
constexpr uint8_t BIND_OPCODE_SET_ADDEND_SLEB = 0x60;
constexpr uint8_t BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB = 0x70;
constexpr uint8_t BIND_OPCODE_ADD_ADDR_ULEB = 0x80;
constexpr uint8_t BIND_OPCODE_DO_BIND = 0x90;
constexpr uint8_t BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB = 0xa0;
constexpr uint8_t BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED = 0xb0;
constexpr uint8_t BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB = 0xc0;

constexpr uint64_t EXPORT_SYMBOL_FLAGS_KIND_MASK = 0x03;
constexpr uint64_t EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION = 0x04;
constexpr uint64_t EXPORT_SYMBOL_FLAGS_REEXPORT = 0x08;
constexpr uint64_t EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER = 0x10;

struct dyld_chained_fixups_header
{
    uint32_t fixups_version;
    uint32_t starts_offset;
    uint32_t imports_offset;
    uint32_t symbols_offset;
    uint32_t imports_count;
    uint32_t imports_format;
    uint32_t symbols_format;
};

struct dyld_chained_starts_in_segment
{
    uint32_t size;
    uint16_t page_size;
    uint16_t pointer_format;
    uint64_t segment_offset;
    uint32_t max_valid_pointer;
    uint16_t page_count;
    uint16_t page_start[1];
};

constexpr uint32_t DYLD_CHAINED_IMPORT = 1;
constexpr uint32_t DYLD_CHAINED_IMPORT_ADDEND = 2;
constexpr uint32_t DYLD_CHAINED_IMPORT_ADDEND64 = 3;
constexpr uint16_t DYLD_CHAINED_PTR_64 = 2;
constexpr uint16_t DYLD_CHAINED_PTR_64_OFFSET = 6;
constexpr uint16_t DYLD_CHAINED_PTR_START_NONE = 0x8000;

} // namespace cutefish::reef
