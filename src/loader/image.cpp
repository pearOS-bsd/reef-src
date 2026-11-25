// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "image.h"

#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include "macho.h"
#include <mutex>
#include <sys/mman.h>
#include <unistd.h>

namespace cutefish::reef {

uint64_t readUleb(const uint8_t*& at, const uint8_t* end)
{
    uint64_t value = 0;
    int shift = 0;
    uint8_t byte;
    do {
        if (at >= end)
            return value;
        byte = *at++;
        if (shift < 64)
            value |= uint64_t(byte & 0x7f) << shift;
        shift += 7;
    } while (byte & 0x80);
    return value;
}

int64_t readSleb(const uint8_t*& at, const uint8_t* end)
{
    uint64_t value = 0;
    int shift = 0;
    uint8_t byte = 0;
    do {
        if (at >= end)
            break;
        byte = *at++;
        if (shift < 64)
            value |= uint64_t(byte & 0x7f) << shift;
        shift += 7;
    } while (byte & 0x80);
    if (shift < 64 && (byte & 0x40))
        value |= uint64_t(-1) << shift;
    return int64_t(value);
}

static std::mutex reserveMutex;
static uintptr_t nextReservation = 0x100000000;

void* reserveMemory(size_t size)
{
    std::lock_guard lock(reserveMutex);
    for (; nextReservation + size <= 0x1000000000; nextReservation += 0x10000000) {
        void* memory = mmap(reinterpret_cast<void*>(nextReservation), size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
        if (memory != MAP_FAILED) {
            nextReservation += (size + 0xfffff) & ~uintptr_t(0xfffff);
            return memory;
        }
    }
    return MAP_FAILED;
}

Image::~Image()
{
    if (fd >= 0)
        close(fd);
}

static size_t pageSize()
{
    static const size_t size = size_t(sysconf(_SC_PAGESIZE));
    return size;
}

void Image::protect()
{
    const size_t page = pageSize();
    for (const Segment& segment : segments) {
        if (segment.protection && (segment.protection & VM_PROT_EXECUTE)) {
            const uintptr_t start = slide + segment.address;
            if (start % page == 0)
                mprotect(reinterpret_cast<void*>(start), (segment.size + page - 1) & -page, PROT_READ | PROT_EXEC);
        }
    }
}

const Section* Image::findSection(std::string_view segment, std::string_view name) const
{
    for (const Section& section : sections) {
        if ((name.empty() || section.name == name) && (segment.empty() || section.segment == segment))
            return &section;
    }
    return nullptr;
}

bool Image::hasObjC() const
{
    for (const Section& section : sections) {
        if (section.name == "__objc_imageinfo")
            return true;
    }
    return false;
}

const char* Image::symbolAt(uintptr_t address, uint64_t* start)
{
    const char* strings = reinterpret_cast<const char*>(data + stringOffset);
    if (symbolsByAddress.empty()) {
        const nlist_64* symbols = reinterpret_cast<const nlist_64*>(data + symbolOffset);
        for (uint32_t i = 0; i < symbolCount; i++) {
            const nlist_64& symbol = symbols[i];
            if ((symbol.n_type & N_STAB) == 0 && (symbol.n_type & N_TYPE) == N_SECT && symbol.n_strx < stringSize)
                symbolsByAddress.emplace_back(uint32_t(symbol.n_value - preferredBase), symbol.n_strx);
        }
        std::sort(symbolsByAddress.begin(), symbolsByAddress.end(), [strings](const auto& a, const auto& b) {
            if (a.first != b.first)
                return a.first < b.first;
            return strcmp(strings + a.second, strings + b.second) < 0;
        });
        symbolsByAddress.shrink_to_fit();
    }
    const uint32_t offset = uint32_t(address - header);
    const auto found = std::upper_bound(symbolsByAddress.begin(), symbolsByAddress.end(), offset,
                                        [](uint32_t value, const auto& symbol) { return value < symbol.first; });
    if (found == symbolsByAddress.begin())
        return nullptr;
    *start = found[-1].first;
    return strings + found[-1].second;
}

void Image::readSymbols()
{
    const nlist_64* symbols = reinterpret_cast<const nlist_64*>(data + symbolOffset);
    const char* strings = reinterpret_cast<const char*>(data + stringOffset);
    for (uint32_t i = 0; i < symbolCount; i++) {
        const nlist_64& symbol = symbols[i];
        if ((symbol.n_type & N_STAB) == 0 && (symbol.n_type & N_TYPE) == N_SECT && (symbol.n_type & N_EXT) && symbol.n_strx < stringSize) {
            const char* name = strings + symbol.n_strx;
            exports.push_back({std::string_view(name, strnlen(name, stringSize - symbol.n_strx)), symbol.n_value});
        }
    }
    std::sort(exports.begin(), exports.end(), [](const NamedSymbol& a, const NamedSymbol& b) {
        if (const int order = a.name.compare(b.name))
            return order < 0;
        return a.address < b.address;
    });
}

std::vector<Bind> Image::binds() const
{
    std::vector<Bind> binds;
    if (chainedSize == 0) {
        // The opcodes of LC_DYLD_INFO.
        auto parse = [this, &binds](const uint8_t* at, const uint8_t* end, bool lazy) {
            std::string_view name;
            int ordinal = 0;
            bool weak = false;
            size_t segment = 0;
            uint64_t offset = 0;
            const auto record = [&] {
                const uint64_t address = segment < segments.size() ? segments[segment].address + offset : 0;
                binds.push_back({name, ordinal, weak, address});
            };
            while (at < end) {
                const uint8_t immediate = *at & BIND_IMMEDIATE_MASK;
                const uint8_t opcode = *at & BIND_OPCODE_MASK;
                at++;
                switch (opcode) {
                case BIND_OPCODE_DONE:
                    if (!lazy)
                        return;
                    break;
                case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
                    ordinal = immediate;
                    break;
                case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:
                    ordinal = int(readUleb(at, end));
                    break;
                case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:
                    ordinal = immediate ? int(int8_t(BIND_OPCODE_MASK | immediate)) : 0;
                    break;
                case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM: {
                    weak = immediate & BIND_SYMBOL_FLAGS_WEAK_IMPORT;
                    const char* text = reinterpret_cast<const char*>(at);
                    name = std::string_view(text, strnlen(text, size_t(end - at)));
                    at += name.size() + 1;
                    break;
                }
                case BIND_OPCODE_SET_TYPE_IMM:
                    break;
                case BIND_OPCODE_SET_ADDEND_SLEB:
                    readSleb(at, end);
                    break;
                case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
                    segment = immediate;
                    offset = readUleb(at, end);
                    break;
                case BIND_OPCODE_ADD_ADDR_ULEB:
                    offset += readUleb(at, end);
                    break;
                case BIND_OPCODE_DO_BIND:
                    record();
                    offset += 8;
                    break;
                case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
                    record();
                    offset += 8 + readUleb(at, end);
                    break;
                case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
                    record();
                    offset += (immediate + 1) * 8;
                    break;
                case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB: {
                    const uint64_t count = readUleb(at, end);
                    const uint64_t skip = readUleb(at, end);
                    for (uint64_t i = 0; i < count; i++) {
                        record();
                        offset += skip + 8;
                    }
                    break;
                }
                default:
                    return;
                }
            }
        };
        const auto [start, end] = linkedit(bindOffset, bindSize);
        parse(start, end, false);
        const auto [lazyStart, lazyEnd] = linkedit(lazyBindOffset, lazyBindSize);
        parse(lazyStart, lazyEnd, true);
        return binds;
    }

    // LC_DYLD_CHAINED_FIXUPS.
    const uint64_t size = chainedSize;
    if (size < sizeof(dyld_chained_fixups_header))
        return binds;
    const uint8_t* base = data + chainedOffset;
    dyld_chained_fixups_header fixups;
    memcpy(&fixups, base, sizeof fixups);
    if (fixups.imports_format - 1 > 2)
        return binds;
    static constexpr uint64_t strides[] = {4, 8, 16};
    const uint64_t stride = strides[fixups.imports_format - 1];
    if (fixups.fixups_version != 0 || fixups.symbols_format != 0 || stride == 0 || fixups.imports_offset > size ||
        size - fixups.imports_offset < uint64_t(fixups.imports_count) * stride || fixups.symbols_offset > size || fixups.starts_offset + 4 > size)
        return binds;
    const uint64_t symbolsSize = size - fixups.symbols_offset;
    std::vector<Bind> imports(fixups.imports_count);
    const uint8_t* import = base + fixups.imports_offset;
    if (fixups.imports_format == DYLD_CHAINED_IMPORT_ADDEND64) {
        for (Bind& bind : imports) {
            uint64_t value;
            memcpy(&value, import, 8);
            const uint16_t ordinal = uint16_t(value);
            bind.ordinal = ordinal > 0xffef ? int(int16_t(ordinal)) : ordinal;
            bind.weak = value >> 16 & 1;
            const uint64_t name = value >> 32;
            if (name < symbolsSize) {
                const char* text = reinterpret_cast<const char*>(base + fixups.symbols_offset + name);
                bind.name = std::string_view(text, strnlen(text, symbolsSize - name));
            }
            import += stride;
        }
    } else {
        for (Bind& bind : imports) {
            uint32_t value;
            memcpy(&value, import, 4);
            const uint8_t ordinal = uint8_t(value);
            bind.ordinal = ordinal >= 0xf0 ? int(int8_t(ordinal)) : ordinal;
            bind.weak = value >> 8 & 1;
            const uint32_t name = value >> 9;
            if (name < symbolsSize) {
                const char* text = reinterpret_cast<const char*>(base + fixups.symbols_offset + name);
                bind.name = std::string_view(text, strnlen(text, symbolsSize - name));
            }
            import += stride;
        }
    }
    std::vector<bool> used(imports.size());
    uint32_t segmentCount;
    memcpy(&segmentCount, base + fixups.starts_offset, 4);
    for (uint32_t i = 0; i < segmentCount; i++) {
        if (fixups.starts_offset + (i + 2) * 4 > size)
            break;
        uint32_t offset;
        memcpy(&offset, base + fixups.starts_offset + 4 + i * 4, 4);
        if (!offset)
            continue;
        const auto* starts = reinterpret_cast<const dyld_chained_starts_in_segment*>(base + fixups.starts_offset + offset);
        if ((starts->pointer_format & ~4) != DYLD_CHAINED_PTR_64)
            continue;
        for (uint16_t page = 0; page < starts->page_count; page++) {
            const uint16_t start = starts->page_start[page];
            if (start & DYLD_CHAINED_PTR_START_NONE)
                continue;
            uint64_t at = starts->segment_offset + uint64_t(page) * starts->page_size + start;
            while (at + 8 <= reservationSize) {
                uint64_t value;
                memcpy(&value, reinterpret_cast<const void*>(header + at), 8);
                if (int64_t(value) < 0) {
                    const uint64_t ordinal = value & 0xffffff;
                    if (ordinal < imports.size()) {
                        Bind bind = imports[ordinal];
                        bind.address = preferredBase + at;
                        binds.push_back(bind);
                        used[ordinal] = true;
                    }
                }
                const uint64_t next = value >> 51 & 0xfff;
                if (!next)
                    break;
                at += next * 4;
            }
        }
    }
    // Imports with no fixup are still needed.
    for (size_t i = 0; i < imports.size(); i++) {
        if (!used[i])
            binds.push_back(imports[i]);
    }
    return binds;
}

std::optional<Export> Image::findExport(std::string_view name) const
{
    if (exportSize) {
        const uint8_t* start = data + exportOffset;
        const uint8_t* end = start + exportSize;
        const uint8_t* at = start;
        while (at < end) {
            const uint64_t terminal = readUleb(at, end);
            if (name.empty() && terminal) {
                const uint64_t flags = readUleb(at, end);
                Export found;
                found.weak = flags & EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION;
                if (flags & EXPORT_SYMBOL_FLAGS_REEXPORT) {
                    found.kind = ExportKind::Reexport;
                    found.ordinal = int(readUleb(at, end));
                    const char* text = reinterpret_cast<const char*>(at);
                    found.name = std::string(text, strnlen(text, size_t(end - at)));
                } else if (flags & EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER) {
                    found.kind = ExportKind::Resolver;
                    found.address = readUleb(at, end);
                    found.resolver = readUleb(at, end);
                } else {
                    found.kind = ExportKind(flags & EXPORT_SYMBOL_FLAGS_KIND_MASK);
                    found.address = readUleb(at, end);
                }
                return found;
            }
            at += terminal;
            if (at >= end)
                return std::nullopt;
            const uint8_t children = *at++;
            bool descended = false;
            for (uint8_t i = 0; i < children && at < end; i++) {
                const char* edge = reinterpret_cast<const char*>(at);
                const size_t length = strnlen(edge, size_t(end - at));
                at += length + 1;
                const uint64_t child = readUleb(at, end);
                if (length <= name.size() && name.compare(0, length, edge, length) == 0) {
                    name.remove_prefix(length);
                    at = start + child;
                    descended = true;
                    break;
                }
            }
            if (!descended)
                return std::nullopt;
        }
        return std::nullopt;
    }
    const auto found = std::lower_bound(exports.begin(), exports.end(), name, [](const NamedSymbol& symbol, std::string_view key) { return symbol.name < key; });
    if (found == exports.end() || found->name != name)
        return std::nullopt;
    Export result;
    result.address = found->address - preferredBase;
    return result;
}

bool Image::map(std::string& error)
{
    uint64_t low = UINT64_MAX;
    uint64_t high = 0;
    for (const Segment& segment : segments) {
        if (segment.protection == 0 && segment.fileSize == 0 && segment.address == 0)
            continue;
        low = std::min(low, segment.address);
        high = std::max(high, segment.address + segment.size);
    }
    if (high <= low) {
        error = path + " has no segments to map";
        return false;
    }
    const size_t page = pageSize();
    const size_t total = (high - 1 + page - low) & -page;
    reservationSize = total;
    void* memory = reserveMemory(total);
    if (memory == MAP_FAILED) {
        error = path + ": cannot reserve address space";
        return false;
    }
    reservation = uintptr_t(memory);
    slide = intptr_t(memory) - intptr_t(low);
    // What comes from the file: mapped where it is aligned, copied for the rest.
    struct Mapping
    {
        uint64_t address;
        uint64_t fileOffset;
        uint64_t mapped;
        uint64_t size;
        uint64_t offset;
    };
    std::vector<Mapping> mappings;
    for (const Segment& segment : segments) {
        if (segment.protection == 0 && segment.address == 0 && segment.fileSize == 0)
            continue;
        const uint64_t size = std::min(segment.fileSize, segment.size);
        const uint64_t fileOffset = sliceOffset + segment.fileOffset;
        uint64_t mapped = 0;
        if (fd >= 0 && segment.address % page == 0 && fileOffset % page == 0)
            mapped = size & -page;
        mappings.push_back({segment.address, fileOffset, mapped, size, segment.fileOffset});
    }
    for (size_t i = 0; i < mappings.size();) {
        uint64_t length = mappings[i].mapped;
        if (!length) {
            i++;
            continue;
        }
        size_t last = i;
        while (last + 1 < mappings.size() && mappings[last + 1].mapped && mappings[last + 1].address == mappings[i].address + length &&
               mappings[last + 1].fileOffset == mappings[i].fileOffset + length) {
            last++;
            length += mappings[last].mapped;
        }
        if (mmap(reinterpret_cast<void*>(mappings[i].address + slide), length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fd,
                 off_t(mappings[i].fileOffset)) == MAP_FAILED) {
            for (size_t j = i; j <= last; j++)
                mappings[j].mapped = 0;
        }
        i = last + 1;
    }
    for (const Mapping& mapping : mappings) {
        if (mapping.mapped < mapping.size)
            memcpy(reinterpret_cast<void*>(slide + mapping.address + mapping.mapped), data + mapping.offset + mapping.mapped, mapping.size - mapping.mapped);
    }
    header = slide + preferredBase;
    return true;
}

static std::string fixedName(const char* name) { return std::string(name, strnlen(name, 16)); }

bool Image::parse(std::string& error)
{
    if (size < sizeof(mach_header_64)) {
        error = path + " is not a Mach-O file";
        return false;
    }
    const auto* machHeader = reinterpret_cast<const mach_header_64*>(data);
    if (machHeader->magic != MH_MAGIC_64) {
        error = path + " is not a 64-bit Mach-O file";
        return false;
    }
    if (machHeader->cputype != CPU_TYPE_X86_64) {
        const char* has = machHeader->cputype == CPU_TYPE_ARM64 ? "arm64" : "other";
        error = path + " is built for another processor: it has only " + has + ", this computer is x86_64";
        return false;
    }
    const uint32_t type = machHeader->filetype;
    if ((type & ~4u) == MH_EXECUTE) {
        if (type == MH_EXECUTE && !(machHeader->flags & MH_PIE)) {
            error = path + " is not position independent";
            return false;
        }
    } else if (type != MH_BUNDLE) {
        error = path + ": unsupported Mach-O file type";
        return false;
    }
    fileType = type;
    const uint8_t* at = data + sizeof(mach_header_64);
    const uint8_t* end = at + machHeader->sizeofcmds;
    if (end > data + size) {
        error = path + ": load commands run past the end of the file";
        return false;
    }
    for (uint32_t i = 0; i < machHeader->ncmds; i++) {
        if (at + sizeof(load_command) > end) {
            error = path + ": truncated load command";
            return false;
        }
        const auto* command = reinterpret_cast<const load_command*>(at);
        if (command->cmdsize < sizeof(load_command) || at + command->cmdsize > end) {
            error = path + ": bad load command size";
            return false;
        }
        switch (command->cmd) {
        case LC_SYMTAB: {
            const auto* symtab = reinterpret_cast<const symtab_command*>(at);
            symbolOffset = symtab->symoff;
            symbolCount = symtab->nsyms;
            stringOffset = symtab->stroff;
            stringSize = symtab->strsize;
            break;
        }
        case LC_UNIXTHREAD: {
            const uint32_t* words = reinterpret_cast<const uint32_t*>(at);
            if (words[2] != x86_THREAD_STATE64 || words[3] != 42 || command->cmdsize - 16 < 168) {
                error = path + ": unsupported LC_UNIXTHREAD state";
                return false;
            }
            // rip.
            uint64_t rip;
            memcpy(&rip, at + 16 + 16 * 8, 8);
            threadEntry = rip;
            break;
        }
        case LC_LOAD_DYLIB:
        case LC_LAZY_LOAD_DYLIB:
        case LC_LOAD_WEAK_DYLIB:
        case LC_REEXPORT_DYLIB:
        case LC_LOAD_UPWARD_DYLIB: {
            const uint32_t offset = reinterpret_cast<const dylib_command*>(at)->name;
            DylibKind kind = DylibKind::Normal;
            if (command->cmd == LC_LOAD_WEAK_DYLIB)
                kind = DylibKind::Weak;
            else if (command->cmd == LC_REEXPORT_DYLIB)
                kind = DylibKind::Reexport;
            else if (command->cmd == LC_LOAD_UPWARD_DYLIB)
                kind = DylibKind::Upward;
            Dylib dylib;
            if (offset < command->cmdsize) {
                const char* name = reinterpret_cast<const char*>(at + offset);
                dylib.path = std::string(name, strnlen(name, command->cmdsize - offset));
            }
            dylib.kind = kind;
            dylibs.push_back(std::move(dylib));
            break;
        }
        case LC_ID_DYLIB: {
            const uint32_t offset = reinterpret_cast<const dylib_command*>(at)->name;
            if (offset < command->cmdsize) {
                const char* name = reinterpret_cast<const char*>(at + offset);
                installName = std::string(name, strnlen(name, command->cmdsize - offset));
            }
            break;
        }
        case LC_SEGMENT_64: {
            segment_command_64 segment;
            memcpy(&segment, at, sizeof segment);
            segments.push_back({fixedName(segment.segname), segment.vmaddr, segment.vmsize, segment.fileoff, segment.filesize, segment.initprot});
            const auto* sectionData = reinterpret_cast<const section_64*>(at + sizeof(segment_command_64));
            for (uint32_t j = 0; j < segment.nsects; j++) {
                section_64 section;
                memcpy(&section, sectionData + j, sizeof section);
                sections.push_back({fixedName(section.segname), fixedName(section.sectname), section.addr, section.size, section.flags});
            }
            break;
        }
        case LC_RPATH: {
            const uint32_t offset = reinterpret_cast<const rpath_command*>(at)->path;
            std::string rpath;
            if (offset < command->cmdsize) {
                const char* name = reinterpret_cast<const char*>(at + offset);
                rpath = std::string(name, strnlen(name, command->cmdsize - offset));
            }
            rpaths.push_back(std::move(rpath));
            break;
        }
        case LC_DYLD_INFO:
        case LC_DYLD_INFO_ONLY: {
            const auto* info = reinterpret_cast<const dyld_info_command*>(at);
            rebaseOffset = info->rebase_off;
            rebaseSize = info->rebase_size;
            bindOffset = info->bind_off;
            bindSize = info->bind_size;
            lazyBindOffset = info->lazy_bind_off;
            lazyBindSize = info->lazy_bind_size;
            weakBindOffset = info->weak_bind_off;
            weakBindSize = info->weak_bind_size;
            exportOffset = info->export_off;
            exportSize = info->export_size;
            break;
        }
        case LC_MAIN:
            entry = reinterpret_cast<const entry_point_command*>(at)->entryoff;
            break;
        case LC_DYLD_EXPORTS_TRIE: {
            const auto* trie = reinterpret_cast<const linkedit_data_command*>(at);
            exportOffset = trie->dataoff;
            exportSize = trie->datasize;
            break;
        }
        case LC_DYLD_CHAINED_FIXUPS: {
            const auto* fixups = reinterpret_cast<const linkedit_data_command*>(at);
            chainedOffset = fixups->dataoff;
            chainedSize = fixups->datasize;
            break;
        }
        default:
            if (command->cmd & LC_REQ_DYLD) {
                error = path + ": unknown required load command " + std::to_string(command->cmd);
                return false;
            }
            break;
        }
        at += command->cmdsize;
    }
    const auto inside = [this](uint32_t offset, uint32_t length) { return offset <= size && length <= size - offset; };
    if (!inside(rebaseOffset, rebaseSize) || !inside(bindOffset, bindSize) || !inside(lazyBindOffset, lazyBindSize) ||
        !inside(weakBindOffset, weakBindSize) || !inside(chainedOffset, chainedSize) || !inside(exportOffset, exportSize) ||
        !inside(symbolOffset, symbolCount * uint32_t(sizeof(nlist_64))) || !inside(stringOffset, stringSize)) {
        error = path + ": linkedit data runs past the end of the file";
        return false;
    }
    for (const Segment& segment : segments) {
        if (segment.fileOffset > size || segment.fileSize > size - segment.fileOffset) {
            error = path + ": segment " + segment.name + " runs past the end of the file";
            return false;
        }
    }
    for (const Segment& segment : segments) {
        if (segment.fileOffset == 0 && segment.fileSize != 0) {
            preferredBase = segment.address;
            if (!entry && threadEntry && preferredBase <= *threadEntry) {
                entry = *threadEntry - preferredBase;
                entryFromThread = true;
            }
            if (exportSize == 0)
                readSymbols();
            return true;
        }
    }
    error = path + ": no segment maps the Mach-O header";
    return false;
}

std::unique_ptr<Image> Image::open(const std::string& path, std::string& error)
{
    const int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        error = "cannot open " + path;
        return nullptr;
    }
    struct stat status;
    if (fstat(fd, &status) != 0 || status.st_size < 8) {
        close(fd);
        error = path + " is not a Mach-O file";
        return nullptr;
    }
    void* file = mmap(nullptr, size_t(status.st_size), PROT_READ, MAP_PRIVATE, fd, 0);
    if (file == MAP_FAILED) {
        close(fd);
        error = "cannot map " + path;
        return nullptr;
    }
    auto image = std::make_unique<Image>();
    image->path = path;
    image->fd = fd;
    image->data = static_cast<const uint8_t*>(file);
    image->size = size_t(status.st_size);
    image->status = status;
    const uint32_t* words = static_cast<const uint32_t*>(file);
    if ((words[0] & 0xfeffffff) == FAT_CIGAM) {
        const uint32_t count = __builtin_bswap32(words[1]);
        const bool wide = words[0] == FAT_CIGAM_64;
        const size_t entrySize = wide ? 32 : 20;
        std::string architectures;
        bool found = false;
        for (uint32_t i = 0; i < count; i++) {
            const size_t at = 8 + i * entrySize;
            if (at + entrySize > image->size)
                break;
            const uint8_t* entry = image->data + at;
            uint32_t cputype;
            memcpy(&cputype, entry, 4);
            if (cputype == __builtin_bswap32(CPU_TYPE_X86_64)) {
                uint64_t offset;
                uint64_t length;
                if (wide) {
                    memcpy(&offset, entry + 8, 8);
                    memcpy(&length, entry + 16, 8);
                    offset = __builtin_bswap64(offset);
                    length = __builtin_bswap64(length);
                } else {
                    uint32_t value;
                    memcpy(&value, entry + 8, 4);
                    offset = __builtin_bswap32(value);
                    memcpy(&value, entry + 12, 4);
                    length = __builtin_bswap32(value);
                }
                if (offset + length > image->size) {
                    error = path + ": truncated slice";
                    return nullptr;
                }
                image->data += offset;
                image->size = length;
                image->sliceOffset = offset;
                found = true;
                break;
            }
            std::string name = __builtin_bswap32(cputype) == CPU_TYPE_ARM64 ? "arm64" : "other";
            if (!architectures.empty())
                name.insert(0, ", ");
            architectures += name;
        }
        if (!found) {
            error = path + " has no slice for this processor: it has only " + architectures + ", this computer is x86_64";
            return nullptr;
        }
    }
    if (image->parse(error) && image->map(error)) {
        close(image->fd);
        image->fd = -1;
        return image;
    }
    return nullptr;
}

} // namespace cutefish::reef
