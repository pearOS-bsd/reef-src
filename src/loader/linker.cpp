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

void Linker::missingSymbol(const Image& from, const std::string& name)
{
    const std::string message = name + " (referenced from " + from.path + ")";
    if (std::find(m_missingSymbols.begin(), m_missingSymbols.end(), message) == m_missingSymbols.end())
        m_missingSymbols.push_back(message);
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

// Binds go through the cache of the program when there is one: replayed in order, or recorded.
template<typename Resolve>
std::optional<uint64_t> Linker::cached(std::string_view name, Resolve resolve)
{
    if (m_bindCache == BindCache::Off)
        return resolve();
    const uint32_t check = hash(name);
    if (m_bindCache == BindCache::Replay) {
        if (m_cacheIndex < m_cachedAddresses.size() && m_cachedHashes[m_cacheIndex] == check) {
            const uint64_t value = m_cachedAddresses[m_cacheIndex++];
            switch (value >> 62) {
            case 2:
                return std::nullopt;
            case 3:
                break;
            case 1:
                return value & 0xfffffffffffff;
            default:
                if ((value >> 52) < m_images.size())
                    return (value & 0xfffffffffffff) + m_images[value >> 52]->reservation;
                break;
            }
            if ((value >> 62) != 3) {
                m_cacheInvalid = true;
                m_bindCache = BindCache::Off;
            }
            return resolve();
        }
        m_cacheInvalid = true;
        m_bindCache = BindCache::Off;
        return resolve();
    }
    m_usedResolvers = false;
    m_absolute = false;
    const std::optional<uint64_t> value = resolve();
    m_cachedAddresses.push_back(value ? encode(*value) : 0x8000000000000000);
    m_cachedHashes.push_back(check);
    return value;
}

std::optional<uint64_t> Linker::resolve(Image* image, int ordinal, const std::string& name, bool weak)
{
    std::optional<uint64_t> value;
    if (ordinal == 0) {
        value = exported(image, name, 0);
    } else if (ordinal == -1) {
        value = exported(m_main, name, 0);
    } else if (ordinal == -3) {
        value = lookup(name);
    } else if (ordinal == -2) {
        for (Image* other : m_images) {
            if (other->unknown10[0])
                continue;
            if ((value = exported(other, name, 0)))
                return value;
        }
    } else if (ordinal >= 1 && size_t(ordinal) <= image->dylibs.size() && image->dylibs[ordinal - 1].image) {
        Image* dependency = image->dylibs[ordinal - 1].image;
        value = exported(dependency, name, 0);
        if (!value) {
            if (Image* umbrella = frameworkImage(*dependency))
                value = exported(umbrella, name, 0);
        }
    }
    if (value)
        return value;
    if (m_stubMode == StubMode::None) {
        if (weak)
            return 0;
        missingSymbol(*image, name);
        return std::nullopt;
    }
    if (weak)
        return 0;
    // Classes stay nil, and data gets no stub.
    if (name.compare(0, 14, "_OBJC_CLASS_$_") == 0 || name.compare(0, 18, "_OBJC_METACLASS_$_") == 0)
        return 0;
    static const char* const dataSuffixes[] = {"Key", "Notification", "Domain", "Property", "Attribute", "Option", "Type", "Kind", "Style",
                                               "Mode", "Value", "Constant", "Version", "Selector", "Category", "Trigger", "Effect", "Size",
                                               "Resolution", "Status", "Name", "Identifier"};
    static const char* const swiftDataSuffixes[] = {"Mn", "Mp", "Mc", "MC", "Ma", "MV", "Mm", "MF", "MX", "N", "Wv", "WV", "Wvd", "MXX",
                                                    "MXM", "MXE", "Tq", "TQ", "Tl", "TL", "Wp", "WP", "MQ", "Mq", "Mf", "Ml", "ML", "Me", "MV"};
    bool data = false;
    if (name.compare(0, 3, "_$s") == 0 || name.compare(0, 3, "_$S") == 0) {
        for (const char* suffix : swiftDataSuffixes)
            data = data || std::string_view(name).ends_with(suffix);
    } else if (name.compare(0, 13, "_OBJC_IVAR_$_") == 0 || name.compare(0, 15, "_OBJC_EHTYPE_$_") == 0 ||
               (name.size() >= 3 && name[0] == '_' && name[1] == 'k' && isupper(static_cast<unsigned char>(name[2])))) {
        data = true;
    } else {
        for (const char* suffix : dataSuffixes)
            data = data || std::string_view(name).ends_with(suffix);
        data = data || name.compare(0, 6, "_gGuid") == 0 || name.compare(0, 9, "_CSSMOID_") == 0 || name.compare(0, 5, "__FE_") == 0 ||
               name.compare(0, 15, "_bootstrap_port") == 0;
    }
    if (data)
        return 0;
    return uint64_t(stub(image, ordinal, name, true));
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
        if (const std::optional<uint64_t> value = cached(name, [&] { return lookup(std::string(name)); }))
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

bool Linker::bind(Image* image, const uint8_t* at, const uint8_t* end, bool lazy)
{
    size_t segment = 0;
    uint64_t offset = 0;
    std::string_view name;
    int ordinal = 0;
    bool weak = false;
    int type = 1;
    int64_t addend = 0;
    bool good = true;
    const auto record = [&]() {
        if (segment >= image->segments.size() || offset + 8 > image->segments[segment].size) {
            report("%s: bind outside its segment\n", image->path.c_str());
            return false;
        }
        if (type != 1) {
            report("%s: unsupported bind type %d\n", image->path.c_str(), type);
            return false;
        }
        const std::optional<uint64_t> value = cached(name, [&] { return resolve(image, ordinal, std::string(name), weak); });
        if (value)
            *reinterpret_cast<uint64_t*>(image->slide + image->segments[segment].address + offset) = *value + addend;
        else
            good = false;
        return true;
    };
    while (at < end) {
        const uint8_t immediate = *at & 0xf;
        const uint8_t opcode = *at & 0xf0;
        at++;
        switch (opcode) {
        case 0x00:
            if (!lazy)
                return good;
            break;
        case 0x10:
            ordinal = immediate;
            break;
        case 0x20:
            ordinal = int(readUleb(at, end));
            break;
        case 0x30:
            ordinal = immediate ? int(int8_t(0xf0 | immediate)) : 0;
            break;
        case 0x40: {
            weak = immediate & 1;
            const char* text = reinterpret_cast<const char*>(at);
            name = std::string_view(text, strnlen(text, size_t(end - at)));
            at += name.size() + 1;
            break;
        }
        case 0x50:
            type = immediate;
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
            if (!record())
                return false;
            offset += 8;
            break;
        case 0xa0:
            if (!record())
                return false;
            offset += 8 + readUleb(at, end);
            break;
        case 0xb0:
            if (!record())
                return false;
            offset += (immediate + 1) * 8;
            break;
        case 0xc0: {
            const uint64_t count = readUleb(at, end);
            const uint64_t skip = readUleb(at, end);
            for (uint64_t i = 0; i < count; i++) {
                if (!record())
                    return false;
                offset += skip + 8;
            }
            break;
        }
        default:
            report("%s: unsupported bind opcode 0x%x\n", image->path.c_str(), *(at - 1));
            return false;
        }
    }
    return good;
}

bool Linker::bindChained(Image& image)
{
    const auto [start, end] = image.linkedit(image.chainedOffset, image.chainedSize);
    const uint64_t size = uint64_t(end - start);
    if (size < sizeof(dyld_chained_fixups_header)) {
        report("%s: truncated chained fixups\n", image.path.c_str());
        return false;
    }
    dyld_chained_fixups_header fixups;
    memcpy(&fixups, start, sizeof fixups);
    if (fixups.fixups_version != 0 || fixups.symbols_format != 0) {
        report("%s: unsupported chained fixups version\n", image.path.c_str());
        return false;
    }
    static constexpr uint64_t strides[] = {4, 8, 16};
    if (fixups.imports_format - 1 > 2 || fixups.imports_offset > size ||
        strides[fixups.imports_format - 1] * fixups.imports_count > size - fixups.imports_offset || fixups.symbols_offset > size ||
        fixups.starts_offset + 4 > size) {
        report("%s: malformed chained fixups\n", image.path.c_str());
        return false;
    }
    const uint64_t stride = strides[fixups.imports_format - 1];
    const uint64_t symbolsSize = size - fixups.symbols_offset;
    std::vector<uint64_t> targets(fixups.imports_count);
    bool good = true;
    const uint8_t* import = start + fixups.imports_offset;
    for (uint64_t& target : targets) {
        int ordinal;
        bool weak;
        uint64_t nameOffset;
        if (fixups.imports_format == DYLD_CHAINED_IMPORT_ADDEND64) {
            uint64_t value;
            memcpy(&value, import, 8);
            const uint16_t number = uint16_t(value);
            ordinal = number >= 0xfff0 ? int(int16_t(number)) : number;
            weak = value >> 16 & 1;
            nameOffset = value >> 32;
        } else {
            uint32_t value;
            memcpy(&value, import, 4);
            const uint8_t number = uint8_t(value);
            ordinal = number >= 0xf0 ? int(int8_t(number)) : number;
            weak = value >> 8 & 1;
            nameOffset = value >> 9;
        }
        if (nameOffset >= symbolsSize) {
            report("%s: chained import name out of range\n", image.path.c_str());
            return false;
        }
        const char* text = reinterpret_cast<const char*>(start + fixups.symbols_offset + nameOffset);
        const std::string_view name(text, strnlen(text, symbolsSize - nameOffset));
        const std::optional<uint64_t> value = cached(name, [&] { return resolve(&image, ordinal, std::string(name), weak); });
        if (value)
            target = *value;
        else
            good = false;
        import += stride;
    }
    if (!good)
        return false;
    uint32_t segmentCount;
    memcpy(&segmentCount, start + fixups.starts_offset, 4);
    for (uint32_t i = 0; i < segmentCount; i++) {
        uint32_t offset;
        memcpy(&offset, start + fixups.starts_offset + 4 + i * 4, 4);
        if (!offset)
            continue;
        const auto* starts = reinterpret_cast<const dyld_chained_starts_in_segment*>(start + fixups.starts_offset + offset);
        if ((starts->pointer_format & ~4) != DYLD_CHAINED_PTR_64) {
            report("%s: unsupported chained pointer format %u\n", image.path.c_str(), starts->pointer_format);
            return false;
        }
        const bool fromHeader = starts->pointer_format == DYLD_CHAINED_PTR_64_OFFSET;
        uint64_t page = starts->segment_offset;
        for (uint16_t j = 0; j < starts->page_count; j++, page += starts->page_size) {
            const uint16_t first = starts->page_start[j];
            if (first == 0xffff)
                continue;
            if (first & DYLD_CHAINED_PTR_START_NONE) {
                report("%s: multiple chain starts per page are not supported\n", image.path.c_str());
                return false;
            }
            uint64_t* at = reinterpret_cast<uint64_t*>(image.header + page + first);
            for (;;) {
                const uint64_t value = *at;
                const uint64_t next = value >> 51 & 0xfff;
                if (int64_t(value) < 0) {
                    const uint64_t index = value & 0xffffff;
                    if (index >= targets.size()) {
                        report("%s: chained bind ordinal out of range\n", image.path.c_str());
                        return false;
                    }
                    *at = targets[index];
                } else {
                    *at = ((value & 0xfffffffff) + (fromHeader ? image.header : uint64_t(image.slide))) | (value >> 36 & 0xff) << 56;
                }
                if (!next)
                    break;
                at = reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(at) + next * 4);
            }
        }
    }
    return true;
}

bool Linker::link(size_t first)
{
    bool good = true;
    for (size_t i = m_images.size(); i-- > first;) {
        Image* image = m_images[i];
        if (image->chainedSize) {
            image->linked = bindChained(*image);
            good = good && image->linked;
            continue;
        }
        const auto [bindStart, bindEnd] = image->linkedit(image->bindOffset, image->bindSize);
        const auto [lazyStart, lazyEnd] = image->linkedit(image->lazyBindOffset, image->lazyBindSize);
        image->linked = rebase(*image) && bind(image, bindStart, bindEnd, false) && bind(image, lazyStart, lazyEnd, true) && bindWeak(*image);
        if (!image->linked)
            good = false;
    }
    if (!m_missingSymbols.empty()) {
        for (const std::string& missing : m_missingSymbols)
            report("symbol not found: %s\n", missing.c_str());
        m_missingSymbols.clear();
        return false;
    }
    if (!good)
        return false;
    for (size_t i = first; i < m_images.size(); i++) {
        registerThreadLocals(*m_images[i]);
        m_images[i]->protect();
    }
    if (!m_mapped || first >= m_images.size())
        return true;
    std::vector<const char*> paths;
    std::vector<const void*> headers;
    for (size_t i = first; i < m_images.size(); i++) {
        if (m_images[i]->hasObjC()) {
            paths.push_back(m_images[i]->path.c_str());
            headers.push_back(reinterpret_cast<const void*>(m_images[i]->header));
        }
    }
    if (!paths.empty())
        m_mapped(paths.size(), paths.data(), headers.data());
    return true;
}

void* Linker::open(const std::string& path, uint32_t mode, std::string& error)
{
    std::lock_guard lock(m_mutex);
    Image* client = m_main;
    if (path.empty())
        return client ? reinterpret_cast<void*>(client->header) : nullptr;
    std::optional<std::string> resolved;
    if (path.find('/') == std::string::npos) {
        resolved = resolveInstallName(client, "/usr/lib/" + path, client);
        if (!resolved && access(path.c_str(), F_OK) == 0)
            resolved = path;
    } else {
        resolved = resolveInstallName(client, path, client);
    }
    if (!resolved) {
        error = "image not found: " + path;
        return nullptr;
    }
    std::error_code failure;
    const std::string canonical = std::filesystem::weakly_canonical(std::filesystem::path(*resolved), failure).string();
    const auto found = m_byPath.find(canonical);
    if (found != m_byPath.end())
        return reinterpret_cast<void*>(found->second->header);
    if (mode & 0x10) {
        error = "image not loaded: " + path;
        return nullptr;
    }
    const size_t first = m_images.size();
    Image* image = load(*resolved, error);
    if (!image)
        return nullptr;
    for (size_t i = first; i < m_images.size(); i++) {
        if (!loadDependencies(m_images[i])) {
            error = "a dependency of " + path + " could not be loaded";
            return nullptr;
        }
    }
    if (!link(first)) {
        error = path + " could not be linked";
        return nullptr;
    }
    initialize(image);
    return reinterpret_cast<void*>(image->header);
}

void* Linker::symbol(void* handle, const char* name)
{
    std::lock_guard lock(m_mutex);
    const std::string mangled = std::string("_") + name;
    if (!handle) {
        for (Image* image : m_images) {
            if (image->unknown10[0])
                continue;
            if (const std::optional<uint64_t> value = exported(image, mangled, 0))
                return reinterpret_cast<void*>(*value);
        }
        return nullptr;
    }
    std::vector<Image*> queue;
    for (Image* image : m_images) {
        if (image->header == uintptr_t(handle)) {
            queue.push_back(image);
            break;
        }
    }
    for (size_t i = 0; i < queue.size(); i++) {
        if (const std::optional<uint64_t> value = exported(queue[i], mangled, 0))
            return reinterpret_cast<void*>(*value);
        for (const Dylib& dylib : queue[i]->dylibs) {
            if (dylib.image && std::find(queue.begin(), queue.end(), dylib.image) == queue.end())
                queue.push_back(dylib.image);
        }
    }
    if (queue.empty() || m_stubMode == StubMode::None)
        return nullptr;
    return stub(queue.front(), 0, mangled, true);
}

// Starts a program whose entry is LC_UNIXTHREAD: the stack has argc, then the pointers.
[[noreturn]] static void startThread(uintptr_t entry, int argumentCount, const uint64_t* pointers, size_t count)
{
    uint64_t* stack = static_cast<uint64_t*>(__builtin_alloca((count + 1) * 8 + 16));
    stack = reinterpret_cast<uint64_t*>((reinterpret_cast<uintptr_t>(stack) + 15) & ~uintptr_t(15));
    stack[0] = uint64_t(argumentCount);
    memcpy(stack + 1, pointers, count * 8);
    asm volatile("mov %0, %%rsp\n\txor %%ebp, %%ebp\n\tjmp *%1" : : "r"(stack), "r"(entry) : "memory");
    __builtin_unreachable();
}

int Linker::run(const std::string& path, const std::vector<std::string>& arguments, char** environment)
{
    std::unique_lock lock(m_mutex);
    std::string error;
    if (path.find(".app/Contents/MacOS/") != std::string::npos) {
        const std::string cache = cachePath(path);
        if (!cache.empty()) {
            m_canonicalFile = cache + ".paths";
            loadCanonicalPaths(m_canonicalFile);
        }
    }
    m_main = load(path, error);
    if (!m_main) {
        report("%s\n", error.c_str());
        return 127;
    }
    if (m_main->fileType != MH_EXECUTE || !m_main->entry) {
        report("%s is not an executable with an entry point\n", path.c_str());
        return 127;
    }
    for (size_t i = 0; i < m_images.size(); i++) {
        if (!loadDependencies(m_images[i]))
            return 127;
    }
    for (Image* image : m_images) {
        if (image->installName == "/usr/lib/libSystem.B.dylib")
            m_system = image;
    }
    if (!m_system) {
        report("%s does not link %s\n", path.c_str(), "/usr/lib/libSystem.B.dylib");
        return 127;
    }
    static std::vector<std::string> strings;
    strings.push_back(m_argv0);
    strings.insert(strings.end(), arguments.begin(), arguments.end());
    m_argumentCount = int(strings.size());
    m_name = m_argv0.c_str() + (m_argv0.rfind('/') + 1);
    m_programName = "executable_path=" + m_main->path;
    HostServices* services = hostServices(m_argumentCount, const_cast<char*>(m_main->path.c_str()), m_name);
    static std::vector<uint64_t> launchItems;
    if (m_launchItems) {
        for (const std::string& item : m_launchItemPaths)
            launchItems.push_back(reinterpret_cast<uint64_t>(item.c_str()));
        launchItems.push_back(0);
        services->launchItems = launchItems.data();
    }
    static std::vector<uint64_t> pointers;
    for (const std::string& argument : strings)
        pointers.push_back(reinterpret_cast<uint64_t>(argument.c_str()));
    pointers.push_back(0);
    const size_t environmentStart = pointers.size();
    for (char** variable = environment; *variable; variable++)
        pointers.push_back(reinterpret_cast<uint64_t>(*variable));
    pointers.push_back(0);
    const size_t appleStart = pointers.size();
    pointers.push_back(reinterpret_cast<uint64_t>(m_programName.c_str()));
    pointers.push_back(0);
    m_arguments = reinterpret_cast<char**>(pointers.data());
    m_environment = reinterpret_cast<char**>(pointers.data() + environmentStart);
    m_apple = reinterpret_cast<const char**>(pointers.data() + appleStart);
    m_vars = {reinterpret_cast<const void*>(m_main->header), &m_argumentCount, &m_arguments, &m_environment, &m_name};
    services->arguments = m_arguments;
    services->environment = m_environment;
    const std::optional<uint64_t> host = exported(m_system, "_reef_host", 0);
    const std::optional<uint64_t> tlvOffset = exported(m_system, "_reef_tlv_offset", 0);
    const std::optional<uint64_t> tlvGetAddress = exported(m_system, "_reef_tlv_get_addr", 0);
    const std::optional<uint64_t> exitFunction = exported(m_system, "_exit", 0);
    if (!host || !tlvOffset || !tlvGetAddress || !exitFunction) {
        report("%s is not reef's libSystem\n", m_system->path.c_str());
        return 127;
    }
    *reinterpret_cast<HostServices**>(*host) = services;
    *reinterpret_cast<long*>(*tlvOffset) = services->threadValueOffset;
    m_tlvGetAddress = reinterpret_cast<void*>(*tlvGetAddress);
    std::string cache;
    std::string key;
    if (m_main->path.find(".app/Contents/MacOS/") != std::string::npos) {
        cache = cachePath(m_main->path);
        if (!cache.empty()) {
            key = cacheKey(m_images, m_root, int(m_stubMode));
            if (!key.empty()) {
                if (readBinds(cache, key, m_cachedAddresses, m_cachedHashes)) {
                    m_bindCache = BindCache::Replay;
                } else {
                    m_bindCache = BindCache::Record;
                    for (size_t i = 0; i < m_images.size(); i++)
                        m_ranges.emplace_back(m_images[i]->reservation, i);
                    std::sort(m_ranges.begin(), m_ranges.end());
                }
            }
        }
    }
    const bool good = link(0);
    if (m_bindCache == BindCache::Record) {
        if (good) {
            writeBinds(cache, key, m_cachedAddresses, m_cachedHashes);
            m_statePath = cache;
            m_stateKey = key;
        } else {
            unlink(cache.c_str());
        }
    } else {
        if (m_cacheInvalid || (m_bindCache == BindCache::Replay && m_cacheIndex != m_cachedAddresses.size()))
            unlink(cache.c_str());
        if (good) {
            m_statePath = cache;
            m_stateKey = key;
        }
    }
    m_bindCache = BindCache::Off;
    m_cachedAddresses.clear();
    m_cachedHashes.clear();
    if (!good)
        return 127;
    if (!m_canonicalFile.empty())
        saveCanonicalPaths(m_canonicalFile);
    m_canonicalFile.clear();
    m_canonical.clear();
    m_canonicalUsed.clear();
    initialize(m_system);
    initialize(m_main);
    lock.unlock();
    const uintptr_t entry = m_main->header + *m_main->entry;
    if (m_main->entryFromThread)
        startThread(entry, m_argumentCount, pointers.data(), pointers.size());
    using Main = int (*)(int, char**, char**, const char**);
    const int status = reinterpret_cast<Main>(entry)(m_argumentCount, m_arguments, m_environment, m_apple);
    reinterpret_cast<void (*)(int)>(*exitFunction)(status);
    return status;
}

} // namespace cutefish::reef
