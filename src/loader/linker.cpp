// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "linker.h"
#include "cache.h"
#include "macho.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace cutefish::reef {

Linker* Linker::current;

static std::vector<std::string> reportList;

std::vector<std::string>& reports() { return reportList; }

void report(const char* format, ...)
{
    char text[4096];
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(text, sizeof text, format, arguments);
    va_end(arguments);
    std::string message = text;
    while (!message.empty() && message.back() == '\n')
        message.pop_back();
    fprintf(stderr, "reef: %s\n", message.c_str());
    reportList.push_back(message);
}

std::string directoryOf(const std::string& path)
{
    const size_t slash = path.rfind('/');
    if (slash == std::string::npos)
        return ".";
    return path.substr(0, slash);
}

// Missing functions: each has 64 bytes in memory that cannot run, so a call faults and the signal handler reports it.
static char* stubArea;
static const char** stubNames;
static size_t stubCount;
static bool stubZero;

const char* missingFunction(uintptr_t address)
{
    if (!stubArea || address < uintptr_t(stubArea) || address >= uintptr_t(stubArea) + 0x4000000)
        return nullptr;
    const size_t index = (address - uintptr_t(stubArea)) >> 6;
    if (index < stubCount)
        return stubNames[index];
    return "?";
}

bool stubsReturnZero() { return stubZero; }
bool haveStubs() { return stubArea != nullptr; }

// A resolved address that does not depend on where the images were put: the image and the offset, or the value.
uint64_t Linker::encode(uint64_t address) const
{
    if (!m_usedResolvers) {
        const auto found = std::upper_bound(m_ranges.begin(), m_ranges.end(), address,
                                            [](uint64_t value, const std::pair<uintptr_t, size_t>& range) { return value < range.first; });
        if (found != m_ranges.begin()) {
            const size_t index = found[-1].second;
            const Image* image = m_images[index];
            if (address < image->reservation + image->reservationSize) {
                if (m_absolute)
                    return 0xc000000000000000;
                if (!address)
                    return 0x4000000000000000;
                if (index < 0x400)
                    return (address - image->reservation) | uint64_t(index) << 52;
                return 0xc000000000000000;
            }
        }
        if (!m_absolute)
            return address ? 0xc000000000000000 : 0x4000000000000000;
        if (address >> 52 == 0)
            return address | 0x4000000000000000;
    }
    return 0xc000000000000000;
}

Image* Linker::frameworkImage(const Image& image) const
{
    static const std::string coreFoundation = "/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation";
    static const std::string appKit = "/System/Library/Frameworks/AppKit.framework/Versions/C/AppKit";
    static const std::string cfNetwork = "/System/Library/Frameworks/CFNetwork.framework/Versions/A/CFNetwork";
    static const std::string coreServices = "/System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices";
    static const std::string foundation = "/System/Library/Frameworks/Foundation.framework/Versions/C/Foundation";
    const std::string* target;
    if (image.installName == coreFoundation || image.installName == appKit)
        target = &foundation;
    else if (image.installName == cfNetwork || image.installName == coreServices)
        target = &foundation;
    else if (image.installName == foundation)
        target = &appKit;
    else
        return nullptr;
    for (Image* other : m_images) {
        if (other->installName == *target)
            return other;
    }
    return nullptr;
}

void Linker::initialize(Image* image)
{
    int& state = m_initialized[image];
    if (state != 0)
        return;
    state = 1;
    for (const Dylib& dylib : image->dylibs) {
        if (dylib.image && dylib.kind != DylibKind::Upward)
            initialize(dylib.image);
    }
    if (m_initializedCallback && image->hasObjC())
        m_initializedCallback(image->path.c_str(), reinterpret_cast<const void*>(image->header));
    using Initializer = void (*)(int, char**, char**, const char**, ProgramVars*);
    for (const Section& section : image->sections) {
        if ((section.flags & 0xff) == 9) {
            // S_MOD_INIT_FUNC_POINTERS.
            for (uint64_t at = 8; at <= section.size; at += 8)
                (*reinterpret_cast<Initializer*>(image->slide + section.address + at - 8))(m_argumentCount, m_arguments, m_environment, m_apple, &m_vars);
        } else if ((section.flags & 0xff) == 0x16) {
            // S_INIT_FUNC_OFFSETS.
            for (uint64_t at = 4; at <= section.size; at += 4) {
                const uint32_t offset = *reinterpret_cast<const uint32_t*>(image->slide + section.address + at - 4);
                reinterpret_cast<Initializer>(image->header + offset)(m_argumentCount, m_arguments, m_environment, m_apple, &m_vars);
            }
        }
    }
    m_initialized[image] = 2;
}

const ImageInfo* Linker::info(uint32_t index) const
{
    if (index >= m_infos.size())
        return nullptr;
    return &m_infos[index];
}

void Linker::observe(MappedCallback mapped, InitializedCallback initialized, void* unmapped)
{
    std::lock_guard lock(m_mutex);
    m_unmapped = unmapped;
    m_mapped = mapped;
    m_initializedCallback = initialized;
    if (m_images.empty())
        return;
    std::vector<const char*> paths;
    std::vector<const void*> headers;
    for (Image* image : m_images) {
        if (image->hasObjC()) {
            paths.push_back(image->path.c_str());
            headers.push_back(reinterpret_cast<const void*>(image->header));
        }
    }
    if (!paths.empty())
        mapped(paths.size(), paths.data(), headers.data());
    for (Image* image : m_images) {
        const auto found = m_initialized.find(image);
        if (found != m_initialized.end() && found->second == 2 && image->hasObjC())
            initialized(image->path.c_str(), reinterpret_cast<const void*>(image->header));
    }
}

bool Linker::findSymbol(const void* address, HostSymbol* symbol)
{
    std::lock_guard lock(m_mutex);
    return findSymbolUnlocked(address, symbol);
}

bool Linker::findSymbolUnlocked(const void* address, HostSymbol* symbol)
{
    for (Image* image : m_images) {
        if (!image->contains(uintptr_t(address)))
            continue;
        uint64_t start = 0;
        symbol->path = image->path.c_str();
        symbol->base = reinterpret_cast<const void*>(image->header);
        symbol->name = nullptr;
        symbol->address = nullptr;
        if (const char* name = image->symbolAt(uintptr_t(address), &start)) {
            symbol->name = name + (*name == '_');
            symbol->address = reinterpret_cast<const void*>(start + image->header);
        }
        return true;
    }
    return false;
}

bool Linker::contains(uintptr_t address) const { return address >= m_low && address < m_high; }

const void* Linker::imageContaining(const void* address)
{
    std::lock_guard lock(m_mutex);
    for (Image* image : m_images) {
        if (image->contains(uintptr_t(address)))
            return reinterpret_cast<const void*>(image->header);
    }
    return nullptr;
}

bool Linker::rebase(Image& image)
{
    const auto [start, end] = image.linkedit(image.rebaseOffset, image.rebaseSize);
    const uint8_t* at = start;
    size_t segment = 0;
    uint64_t offset = 0;
    int type = 0;
    const auto slide = [&](uint64_t where) {
        if (segment >= image.segments.size() || where + 8 > image.segments[segment].size) {
            report("%s: rebase outside its segment\n", image.path.c_str());
            return false;
        }
        if (type != 1) {
            report("%s: unsupported rebase type %d\n", image.path.c_str(), type);
            return false;
        }
        *reinterpret_cast<uint64_t*>(image.slide + image.segments[segment].address + where) += image.slide;
        return true;
    };
    while (at < end) {
        const uint8_t immediate = *at & 0xf;
        const uint8_t opcode = *at & 0xf0;
        at++;
        switch (opcode) {
        case 0x00:
            return true;
        case 0x10:
            type = immediate;
            break;
        case 0x20:
            segment = immediate;
            offset = readUleb(at, end);
            break;
        case 0x30:
            offset += readUleb(at, end);
            break;
        case 0x40:
            offset += immediate * 8;
            break;
        case 0x50:
            for (int i = 0; i < immediate; i++) {
                if (!slide(offset))
                    return false;
                offset += 8;
            }
            break;
        case 0x60: {
            const uint64_t count = readUleb(at, end);
            for (uint64_t i = 0; i < count; i++) {
                if (!slide(offset))
                    return false;
                offset += 8;
            }
            break;
        }
        case 0x70:
            if (!slide(offset))
                return false;
            offset += readUleb(at, end) + 8;
            break;
        case 0x80: {
            const uint64_t count = readUleb(at, end);
            const uint64_t skip = readUleb(at, end);
            for (uint64_t i = 0; i < count; i++) {
                if (!slide(offset))
                    return false;
                offset += skip + 8;
            }
            break;
        }
        default:
            report("%s: unknown rebase opcode 0x%x\n", image.path.c_str(), opcode);
            return false;
        }
    }
    return true;
}

void* Linker::stub(Image* image, int ordinal, const std::string& name, bool show)
{
    m_usedResolvers = true;
    if (const auto found = m_stubs.find(name); found != m_stubs.end())
        return found->second;
    std::string library = "?";
    if (ordinal > 0 && size_t(ordinal) <= image->dylibs.size())
        library = image->dylibs[ordinal - 1].path;
    if (show)
        fprintf(stderr, "reef: stubbing missing symbol %s in %s (referenced from %s)\n", name.c_str(), library.c_str(), image->path.c_str());
    void* address = nullptr;
    // A class that is missing stays nil.
    if (name.compare(0, 14, "_OBJC_CLASS_$_") != 0 && name.compare(0, 18, "_OBJC_METACLASS_$_") != 0) {
        if (!stubArea) {
            void* area = mmap(nullptr, 0x4000000, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
            if (area == MAP_FAILED)
                return nullptr;
            stubArea = static_cast<char*>(area);
            stubNames = static_cast<const char**>(calloc(0x100000, sizeof(char*)));
            stubZero = m_stubMode == StubMode::Zero;
        }
        if (stubCount >= 0x100000)
            return nullptr;
        stubNames[stubCount] = strdup(name.c_str() + (name[0] == '_'));
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        address = stubArea + stubCount * 64;
        stubCount++;
    }
    m_stubs.emplace(name, address);
    return address;
}

std::string Linker::canonical(const std::string& path, bool* cached)
{
    *cached = false;
    const auto found = m_canonical.find(path);
    if (found != m_canonical.end()) {
        struct stat status;
        struct stat target;
        if (stat(path.c_str(), &status) == 0 && stat(found->second.path.c_str(), &target) == 0 && status.st_dev == found->second.device &&
            status.st_ino == found->second.inode && status.st_dev == target.st_dev && status.st_ino == target.st_ino) {
            m_canonicalUsed.push_back({path, found->second.path, found->second.device, found->second.inode});
            return found->second.path;
        }
    }
    std::error_code error;
    const std::string resolved = std::filesystem::weakly_canonical(std::filesystem::path(path), error).string();
    struct stat status;
    if (!m_canonicalFile.empty() && stat(resolved.c_str(), &status) == 0) {
        m_canonicalUsed.push_back({path, resolved, status.st_dev, status.st_ino});
        m_canonicalChanged = true;
    }
    return resolved;
}

Image* Linker::load(const std::string& path, std::string& error)
{
    auto found = m_byPath.find(path);
    if (found != m_byPath.end())
        return found->second;
    bool cached;
    const std::string resolved = canonical(path, &cached);
    found = m_byPath.find(resolved);
    if (found != m_byPath.end()) {
        m_byPath[path] = found->second;
        return found->second;
    }
    std::unique_ptr<Image> opened = Image::open(resolved, error);
    if (!opened)
        return nullptr;
    Image* image = opened.release();
    m_low = std::min(m_low, image->reservation);
    m_high = std::max(m_high, image->reservation + image->reservationSize);
    m_images.push_back(image);
    m_byPath[resolved] = image;
    m_byPath[path] = image;
    m_infos.push_back({image->header, image->path.c_str(), image->slide});
    if (!m_imageMap.empty()) {
        if (FILE* file = fopen(m_imageMap.c_str(), "a")) {
            for (const Segment& segment : image->segments) {
                if (segment.size) {
                    const uintptr_t start = image->slide + segment.address;
                    fprintf(file, "%lx-%lx %llx %s %s\n", start, start + segment.size, static_cast<unsigned long long>(segment.address),
                            segment.name.c_str(), image->path.c_str());
                }
            }
            fclose(file);
        }
    }
    return image;
}

Linker::Linker(const std::string& root)
    : m_root(root)
{
    current = this;
}

void Linker::saveCanonicalPaths(const std::string& path)
{
    if (!m_canonicalChanged && m_canonicalUsed.size() == m_canonical.size())
        return;
    std::string data;
    for (const CanonicalRecord& record : m_canonicalUsed) {
        const uint32_t length = uint32_t(record.path.size());
        data.append(reinterpret_cast<const char*>(&length), 4);
        data += record.path;
        const uint32_t canonicalLength = uint32_t(record.canonical.size());
        data.append(reinterpret_cast<const char*>(&canonicalLength), 4);
        data += record.canonical;
        data.append(reinterpret_cast<const char*>(&record.device), 8);
        data.append(reinterpret_cast<const char*>(&record.inode), 8);
    }
    writeState(path, "canonical paths 1", data.data(), data.size());
}

void Linker::writePreferences(const char* name, const void* data, size_t size)
{
    if (m_statePath.empty() || m_stateKey.empty())
        return;
    writeState(m_statePath + '.' + name, m_stateKey, data, size);
}

void* Linker::readPreferences(const char* name, size_t* size)
{
    *size = 0;
    if (m_statePath.empty() || m_stateKey.empty())
        return nullptr;
    std::string data;
    if (!readState(m_statePath + '.' + name, m_stateKey, data))
        return nullptr;
    void* copy = malloc(data.empty() ? 1 : data.size());
    if (copy) {
        memcpy(copy, data.data(), data.size());
        *size = data.size();
    }
    return copy;
}

void Linker::missingLibrary(const Image& from, const std::string& name)
{
    const std::string message = name + " (referenced from " + from.path + ")";
    if (std::find(m_missingLibraries.begin(), m_missingLibraries.end(), message) == m_missingLibraries.end())
        m_missingLibraries.push_back(message);
}

std::string Linker::expand(Image* loader, const std::string& path)
{
    if (path.compare(0, 17, "@executable_path/") == 0)
        return directoryOf(m_main->path) + path.substr(16);
    if (path.compare(0, 13, "@loader_path/") == 0)
        return directoryOf(loader->path) + path.substr(12);
    return path;
}

bool Linker::registerThreadLocals(Image& image)
{
    const Section* variables = nullptr;
    uint64_t start = UINT64_MAX;
    uint64_t end = 0;
    for (const Section& section : image.sections) {
        if ((section.flags & 0xff) == 0x13)
            variables = &section;
        else if ((section.flags & 0xff) == 0x11 || (section.flags & 0xff) == 0x12) {
            start = std::min(start, section.address);
            end = std::max(end, section.address + section.size);
        }
    }
    if (!variables)
        return true;
    m_threadLocals.push_back({0, end - start, start, m_threadLocals.size()});
    return true;
}

void Linker::loadCanonicalPaths(const std::string& path)
{
    std::string data;
    if (!readState(path, "canonical paths 1", data) || data.empty())
        return;
    size_t at = 0;
    while (at < data.size()) {
        std::string name;
        std::string resolved;
        CanonicalPath entry;
        uint32_t length;
        if (data.size() - at < 4)
            break;
        memcpy(&length, data.data() + at, 4);
        at += 4;
        if (data.size() - at < length)
            break;
        name.assign(data, at, length);
        at += length;
        if (data.size() - at < 4)
            break;
        memcpy(&length, data.data() + at, 4);
        at += 4;
        if (data.size() - at < length)
            break;
        resolved.assign(data, at, length);
        at += length;
        if (data.size() - at < 8)
            break;
        memcpy(&entry.device, data.data() + at, 8);
        if (data.size() - at - 8 < 8)
            break;
        memcpy(&entry.inode, data.data() + at + 8, 8);
        at += 16;
        entry.path = std::move(resolved);
        m_canonical.emplace(std::move(name), std::move(entry));
        continue;
    }
    if (at < data.size())
        m_canonical.clear();
}

std::optional<std::string> Linker::existing(const std::string& path)
{
    const auto exists = [this](const std::string& file) {
        auto found = m_exists.find(file);
        if (found == m_exists.end())
            found = m_exists.emplace(file, access(file.c_str(), F_OK) == 0).first;
        return found->second;
    };
    if (!path.empty() && path[0] == '/') {
        if (exists(m_root + path))
            return m_root + path;
    }
    if (exists(path))
        return path;
    return std::nullopt;
}

std::optional<std::string> Linker::framework(const std::string& path)
{
    if (std::optional<std::string> found = existing(path))
        return found;
    const size_t slash = path.rfind('/');
    if (slash == std::string::npos)
        return std::nullopt;
    const std::string name = path.substr(slash + 1);
    const std::string directory = path.substr(0, slash);
    if (name.empty() || !std::string_view(directory).ends_with("/" + name + ".framework"))
        return std::nullopt;
    if (std::optional<std::string> found = existing(directory + "/Versions/Current/" + name))
        return found;
    for (const char* version : {"A", "B", "C"}) {
        if (std::optional<std::string> found = existing(directory + "/Versions/" + version + "/" + name))
            return found;
    }
    return std::nullopt;
}

std::optional<std::string> Linker::resolveInstallName(Image* loader, const std::string& name, Image* client)
{
    if (name.compare(0, 7, "@rpath/") != 0)
        return framework(expand(client, name));
    const std::string rest = name.substr(6);
    // The images that load this one, then the program.
    std::vector<Image*> chain;
    for (Image* image = loader; image; image = nullptr)
        chain.push_back(image);
    if (m_main && std::find(chain.begin(), chain.end(), m_main) == chain.end())
        chain.push_back(m_main);
    for (Image* image : chain) {
        for (const std::string& rpath : image->rpaths) {
            std::string base = rpath;
            if (rpath.compare(0, 13, "@loader_path/") == 0)
                base = directoryOf(image->path) + rpath.substr(12);
            else
                base = expand(client, rpath);
            if (std::optional<std::string> found = framework(base + rest))
                return found;
        }
    }
    return std::nullopt;
}

bool Linker::loadDependencies(Image* image)
{
    for (Dylib& dylib : image->dylibs) {
        const std::optional<std::string> path = resolveInstallName(image, dylib.path, image);
        if (!path) {
            if (dylib.kind == DylibKind::Weak)
                continue;
            if (m_stubMode != StubMode::None) {
                fprintf(stderr, "reef: stubbing missing library %s\n  referenced from %s\n", dylib.path.c_str(), image->path.c_str());
                continue;
            }
            report("library not loaded: %s\n  referenced from %s\n", dylib.path.c_str(), image->path.c_str());
            return false;
        }
        std::string error;
        dylib.image = load(*path, error);
        if (!dylib.image && dylib.kind != DylibKind::Weak) {
            report("%s\n  referenced from %s\n", error.c_str(), image->path.c_str());
            return false;
        }
    }
    return true;
}

std::optional<uint64_t> Linker::exported(Image* image, const std::string& name, int depth)
{
    if (depth > 32)
        return std::nullopt;
    if (std::optional<Export> found = image->findExport(name)) {
        switch (found->kind) {
        case ExportKind::Reexport: {
            if (found->ordinal <= 0 || size_t(found->ordinal) > image->dylibs.size() || !image->dylibs[found->ordinal - 1].image)
                return std::nullopt;
            return exported(image->dylibs[found->ordinal - 1].image, found->name.empty() ? name : found->name, depth + 1);
        }
        case ExportKind::Resolver:
            if (!image->linked) {
                report("%s in %s needs its resolver before the image is bound\n", name.c_str(), image->path.c_str());
                return std::nullopt;
            }
            m_usedResolvers = true;
            return reinterpret_cast<uint64_t (*)()>(found->resolver + image->header)();
        case ExportKind::Absolute:
            m_absolute = true;
            return found->address;
        default:
            return found->address + image->header;
        }
    }
    auto& cache = m_reexported[image];
    if (m_bindCache != BindCache::Record) {
        const auto cached = cache.find(name);
        if (cached != cache.end())
            return cached->second;
    }
    for (const Dylib& dylib : image->dylibs) {
        if (dylib.kind == DylibKind::Reexport && dylib.image) {
            if (const std::optional<uint64_t> value = exported(dylib.image, name, depth + 1)) {
                cache.emplace(name, *value);
                return value;
            }
        }
    }
    return std::nullopt;
}

std::optional<uint64_t> Linker::lookup(const std::string& name)
{
    std::optional<uint64_t> value;
    for (Image* image : m_images) {
        if (image->findExport(name)) {
            value = exported(image, name, 0);
            if (value)
                break;
        }
    }
    m_flat.emplace(name, value);
    return value;
}

std::optional<uint64_t> Linker::cachedLookup(std::string_view name)
{
    if (m_bindCache == BindCache::Off)
        return lookup(std::string(name));
    const uint32_t check = hash(name);
    if (m_bindCache == BindCache::Replay) {
        if (m_cacheIndex < m_cachedAddresses.size() && m_cachedHashes[m_cacheIndex] == check) {
            const uint64_t value = m_cachedAddresses[m_cacheIndex++];
            switch (value >> 62) {
            case 2:
                return std::nullopt;
            case 1:
                return value & 0xfffffffffffff;
            case 0:
                if ((value >> 52) < m_images.size())
                    return (value & 0xfffffffffffff) + m_images[value >> 52]->reservation;
                break;
            default:
                break;
            }
        }
        m_cacheInvalid = true;
        m_bindCache = BindCache::Off;
        return lookup(std::string(name));
    }
    m_usedResolvers = false;
    m_absolute = false;
    const std::optional<uint64_t> value = lookup(std::string(name));
    m_cachedAddresses.push_back(value ? encode(*value) : 0x8000000000000000);
    m_cachedHashes.push_back(check);
    return value;
}

bool Linker::bindWeak(Image& image)
{
    const auto [start, end] = image.linkedit(image.weakBindOffset, image.weakBindSize);
    const uint8_t* at = start;
    std::string_view name;
    size_t segment = 0;
    uint64_t offset = 0;
    int64_t addend = 0;
    const auto inside = [&](uint64_t where) { return segment < image.segments.size() && where + 8 <= image.segments[segment].size; };
    const auto bind = [&](uint64_t where) {
        if (const std::optional<uint64_t> value = cachedLookup(name))
            *reinterpret_cast<uint64_t*>(image.slide + image.segments[segment].address + where) = addend + *value;
    };
    while (at < end) {
        const uint8_t immediate = *at & 0xf;
        const uint8_t opcode = *at & 0xf0;
        at++;
        switch (opcode) {
        case 0x00:
            return true;
        case 0x40: {
            const char* text = reinterpret_cast<const char*>(at);
            name = std::string_view(text, strnlen(text, size_t(end - at)));
            at += name.size() + 1;
            break;
        }
        case 0x50:
            break;
        case 0x60:
            addend = readSleb(at, end);
            break;
        case 0x70:
            segment = immediate;
            offset = readUleb(at, end);
            break;
        case 0x80:
            offset += readUleb(at, end);
            break;
        case 0x90:
            if (!inside(offset)) {
                report("%s: weak bind outside its segment\n", image.path.c_str());
                return false;
            }
            bind(offset);
            offset += 8;
            break;
        case 0xa0:
            if (!inside(offset)) {
                report("%s: weak bind outside its segment\n", image.path.c_str());
                return false;
            }
            bind(offset);
            offset += readUleb(at, end) + 8;
            break;
        case 0xb0:
            if (!inside(offset)) {
                report("%s: weak bind outside its segment\n", image.path.c_str());
                return false;
            }
            bind(offset);
            offset += (immediate + 1) * 8;
            break;
        case 0xc0: {
            const uint64_t count = readUleb(at, end);
            const uint64_t skip = readUleb(at, end);
            for (uint64_t i = 0; i < count; i++) {
                if (!inside(offset)) {
                    report("%s: weak bind outside its segment\n", image.path.c_str());
                    return false;
                }
                bind(offset);
                offset += skip + 8;
            }
            break;
        }
        default:
            report("%s: unsupported weak bind opcode 0x%x\n", image.path.c_str(), opcode);
            return false;
        }
    }
    return true;
}

} // namespace cutefish::reef
