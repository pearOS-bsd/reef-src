// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The dynamic linker: loads a program's images, binds them and runs it.

#pragma once

#include "host.h"
#include "image.h"

#include <deque>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace cutefish::reef {

// What the system library reads about each image (dyld_all_image_infos).
struct ImageInfo
{
    uintptr_t header;
    const char* path;
    intptr_t slide;
};

// Thread-local variables of an image.
struct ThreadLocal
{
    uint64_t key;
    uint64_t size;
    uint64_t end;
    uint64_t index;
};

struct ProgramVars
{
    const void* header;
    int* argumentCount;
    char*** arguments;
    char*** environment;
    const char** name;
};

// A path, what it resolved to, and the file then.
struct CanonicalPath
{
    std::string path;
    dev_t device;
    ino_t inode;
};

struct CanonicalRecord
{
    std::string path;
    std::string canonical;
    dev_t device;
    ino_t inode;
};

enum class StubMode { None, Report, Zero };
enum class BindCache { Off, Record, Replay };

using MappedCallback = void (*)(size_t count, const char* const* paths, const void* const* headers);
using InitializedCallback = void (*)(const char* path, const void* header);

class Linker
{
public:
    explicit Linker(const std::string& root);

    static Linker* current;

    const std::string& root() const { return m_root; }
    const std::deque<ImageInfo>& infos() const { return m_infos; }
    const std::vector<Image*>& images() const { return m_images; }
    void setImageMap(std::string path) { m_imageMap = std::move(path); }
    void setStubMode(StubMode mode) { m_stubMode = mode; }
    void setArgv0(std::string argv0) { m_argv0 = std::move(argv0); }
    void setLaunchItems(std::vector<std::string> items) { m_launchItems = std::move(items); }

    Image* load(const std::string& path, std::string& error);
    void* open(const std::string& path, uint32_t mode, std::string& error);
    void* symbol(void* handle, const char* name);
    const ImageInfo* info(uint32_t index) const;
    void observe(MappedCallback mapped, InitializedCallback initialized, void* unmapped);
    bool findSymbol(const void* address, HostSymbol* symbol);
    bool findSymbolUnlocked(const void* address, HostSymbol* symbol);
    const void* imageContaining(const void* address);
    bool contains(uintptr_t address) const;
    void* readPreferences(const char* name, size_t* size);
    void writePreferences(const char* name, const void* data, size_t size);
    Image* frameworkImage(const Image& image) const;
    int run(const std::string& path, const std::vector<std::string>& arguments, char** environment);

    std::optional<std::string> resolveInstallName(const std::string& name, Image* image);
    std::optional<uint64_t> exported(Image* image, const std::string& name, int depth);
    std::optional<uint64_t> lookup(const std::string& name);
    void* stub(Image* image, int ordinal, const std::string& name, bool report);
    bool loadDependencies(Image* image);
    void initialize(Image* image);
    void missingSymbol(const Image& from, const std::string& name);
    std::optional<uint64_t> resolve(Image* image, int ordinal, const std::string& name, bool weak);
    bool bind(Image* image, const uint8_t* at, const uint8_t* end, bool lazy);
    bool bindChained(Image& image);
    bool link(size_t first);

    std::recursive_mutex m_mutex;
    std::string m_root;
    std::string m_imageMap;
    StubMode m_stubMode = StubMode::None;
    std::optional<std::vector<std::string>> m_launchItems;
    std::string m_argv0;
    std::map<std::string, void*, std::less<>> m_stubs;
    std::vector<Image*> m_images;
    uintptr_t m_low = UINTPTR_MAX;
    uintptr_t m_high = 0;
    std::map<std::string, Image*, std::less<>> m_byPath;
    std::unordered_map<std::string, bool> m_exists;
    std::unordered_map<Image*, std::unordered_map<std::string, uint64_t>> m_reexported;
    Image* m_main = nullptr;
    Image* m_system = nullptr;
    void* m_tlvGetAddress = nullptr;
    std::vector<std::string> m_missingSymbols;
    std::map<std::string, std::optional<uint64_t>, std::less<>> m_flat;
    BindCache m_bindCache = BindCache::Off;
    std::vector<uint64_t> m_cachedAddresses;
    std::vector<uint32_t> m_cachedHashes;
    size_t m_cacheIndex = 0;
    bool m_cacheInvalid = false;
    bool m_usedResolvers = false;
    bool m_absolute = false;
    std::vector<std::pair<uintptr_t, size_t>> m_ranges;
    std::unordered_map<std::string, CanonicalPath> m_canonical;
    std::vector<CanonicalRecord> m_canonicalUsed;
    bool m_canonicalChanged = false;
    std::string m_canonicalFile;
    std::string m_statePath;
    std::string m_stateKey;
    std::deque<ImageInfo> m_infos;
    std::deque<ThreadLocal> m_threadLocals;
    std::map<Image*, int> m_initialized;
    MappedCallback m_mapped = nullptr;
    InitializedCallback m_initializedCallback = nullptr;
    void* m_unmapped = nullptr;
    int m_argumentCount = 0;
    char** m_arguments = nullptr;
    char** m_environment = nullptr;
    const char* m_name = nullptr;
    const char** m_apple = nullptr;
    std::string m_programName;
    ProgramVars m_vars = {};

private:
    std::string canonical(const std::string& path, bool* cached);
    void loadCanonicalPaths(const std::string& path);
    void saveCanonicalPaths(const std::string& path);
    std::optional<std::string> existing(const std::string& path);
    std::optional<std::string> framework(const std::string& path);
    std::string expand(Image* loader, const std::string& path);
    template<typename Resolve>
    std::optional<uint64_t> cached(std::string_view name, Resolve resolve);
    uint64_t encode(uint64_t address) const;
    bool rebase(Image& image);
    bool bindWeak(Image& image);
    bool registerThreadLocals(Image& image);
};

// Errors that are shown once the program has run.
std::vector<std::string>& reports();
// reef --list-missing[=json]: what a program or bundle needs that reef does not have.
int listMissing(Linker& linker, const std::string& path, bool json);
void report(const char* format, ...) __attribute__((format(printf, 1, 2)));
std::string directoryOf(const std::string& path);
// The name of the missing function a stub address stands for.
const char* missingFunction(uintptr_t address);
bool stubsReturnZero();
bool haveStubs();

} // namespace cutefish::reef
