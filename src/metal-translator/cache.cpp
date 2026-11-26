// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The disk cache of compiled Metal sources and translated shaders.

#include "cache.h"

#include <dirent.h>
#include <dlfcn.h>
#include <elf.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <mutex>
#include <thread>

#include <llvm-c/Core.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Support/SHA256.h>

namespace cutefish::reef::metal {

// The first bytes of a compiled source and of a shader.
static const char compiledMagic[8] = {'R', 'E', 'E', 'F', 'M', 'S', 'L', '1'};
static const char shaderMagic[8] = {'R', 'E', 'E', 'F', 'M', 'T', 'L', '1'};

namespace {

// The bytes of a cache file, read from the start. A read past the end fails.
struct Reader
{
    const std::vector<char> &bytes;
    size_t position = 0;
};

}

static inline std::string hexOf(llvm::ArrayRef<uint8_t> bytes)
{
    std::string hex;
    for (const uint8_t byte : bytes) {
        hex += "0123456789abcdef"[byte >> 4];
        hex += "0123456789abcdef"[byte & 15];
    }
    return hex;
}

template<typename T>
static void put(std::vector<char> &bytes, const T &value)
{
    const char *data = reinterpret_cast<const char *>(&value);
    bytes.insert(bytes.end(), data, data + sizeof value);
}

// The count of a vector of plain structures and their bytes.
template<typename T>
static void putArray(std::vector<char> &bytes, const std::vector<T> &values)
{
    put(bytes, uint64_t(values.size()));
    for (const T &value : values)
        put(bytes, value);
}

// A string of a compiled source: its length and its bytes.
static void putString(std::vector<char> &bytes, const std::string &value)
{
    put(bytes, uint64_t(value.size()));
    bytes.insert(bytes.end(), value.begin(), value.end());
}

// A string of a shader: its length, -1 for none, and its bytes.
static void putText(std::vector<char> &bytes, const char *text)
{
    const uint64_t length = text ? strlen(text) : uint64_t(-1);
    put(bytes, length);
    if (text)
        bytes.insert(bytes.end(), text, text + length);
}

template<typename T>
static bool get(Reader &reader, T &value)
{
    if (reader.bytes.size() - reader.position < sizeof value)
        return false;
    memcpy(&value, reader.bytes.data() + reader.position, sizeof value);
    reader.position += sizeof value;
    return true;
}

static bool getMagic(Reader &reader, const char (&magic)[8])
{
    if (reader.bytes.size() - reader.position < sizeof magic
        || memcmp(reader.bytes.data() + reader.position, magic, sizeof magic) != 0)
        return false;
    reader.position += sizeof magic;
    return true;
}

// The count can be wrong in a damaged file: it may not promise more elements than there are bytes.
template<typename T>
static bool getArray(Reader &reader, std::vector<T> &values)
{
    uint64_t count;
    if (!get(reader, count) || count > (reader.bytes.size() - reader.position) / sizeof(T))
        return false;
    values.resize(count);
    for (T &value : values) {
        if (!get(reader, value))
            return false;
    }
    return true;
}

static bool getString(Reader &reader, std::string &value)
{
    int64_t length;
    if (!get(reader, length) || length < 0 || reader.bytes.size() - reader.position < uint64_t(length))
        return false;
    value.assign(reader.bytes.data() + reader.position, length);
    reader.position += length;
    return true;
}

// The structures of a shader point into its strings.
static bool getText(Reader &reader, Shader &shader, const char *&text)
{
    int64_t length;
    if (!get(reader, length) || length < -1)
        return false;
    if (length == -1) {
        text = nullptr;
        return true;
    }
    if (reader.bytes.size() - reader.position < uint64_t(length))
        return false;
    shader.strings.push_back(std::string(reader.bytes.data() + reader.position, length));
    text = shader.strings.back().c_str();
    reader.position += length;
    return true;
}

// The file of the library that holds an address, empty when none does.
static std::string libraryOf(const void *address)
{
    Dl_info info = {};
    if (!dladdr(address, &info) || !info.dli_fname)
        return {};
    return info.dli_fname;
}

// The GNU build id of an ELF file in hex, empty when it has none.
static std::string buildId(const std::string &file)
{
    std::string id;
    const int fd = open(file.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return id;
    Elf64_Ehdr header;
    if (pread(fd, &header, sizeof header, 0) == sizeof header && memcmp(header.e_ident, ELFMAG, SELFMAG) == 0
        && header.e_ident[EI_CLASS] == ELFCLASS64 && header.e_phentsize == sizeof(Elf64_Phdr)) {
        for (unsigned i = 0; i < header.e_phnum && id.empty(); ++i) {
            Elf64_Phdr program;
            if (pread(fd, &program, sizeof program, header.e_phoff + i * sizeof program) != sizeof program
                || program.p_type != PT_NOTE || program.p_filesz > 0x100000)
                continue;
            std::vector<uint8_t> notes(program.p_filesz);
            if (pread(fd, notes.data(), notes.size(), program.p_offset) != ssize_t(notes.size()))
                continue;
            for (size_t offset = 0; offset + sizeof(Elf64_Nhdr) <= notes.size();) {
                Elf64_Nhdr note;
                memcpy(&note, notes.data() + offset, sizeof note);
                const size_t name = offset + sizeof note;
                const size_t description = name + ((note.n_namesz + 3) & ~3u);
                offset = description + ((note.n_descsz + 3) & ~3u);
                if (offset > notes.size())
                    break;
                if (note.n_type == NT_GNU_BUILD_ID && note.n_namesz == 4 && memcmp(notes.data() + name, "GNU", 4) == 0) {
                    id = hexOf(llvm::ArrayRef<uint8_t>(notes.data() + description, note.n_descsz));
                    break;
                }
            }
        }
    }
    close(fd);
    return id;
}

std::string buildIds(const std::vector<std::string> &files)
{
    std::string ids;
    for (const std::string &file : files) {
        std::string id = buildId(file);
        struct stat status = {};
        if (id.empty() && stat(file.c_str(), &status) == 0)
            id = std::to_string(status.st_size) + ":" + std::to_string(status.st_mtim.tv_sec) + "."
                + std::to_string(status.st_mtim.tv_nsec);
        ids += file.substr(file.rfind('/') + 1) + "=" + id + "\n";
    }
    return ids;
}

// What a cached translation depends on: the translator, LLVM and the compiler next to the translator. Empty when the
// translator or LLVM cannot tell where they are.
static const std::string &libraryIds()
{
    static const std::string ids = [] {
        const std::string translator = libraryOf(reinterpret_cast<const void *>(&libraryIds));
        const std::string llvm = libraryOf(reinterpret_cast<const void *>(&LLVMContextCreate));
        if (translator.empty() || llvm.empty())
            return std::string();
        std::vector<std::string> files = {translator, llvm};
        const std::string compiler = translator.substr(0, translator.rfind('/') + 1) + "libreef-metal-msl.so";
        if (access(compiler.c_str(), F_OK) == 0)
            files.push_back(compiler);
        return buildIds(files);
    }();
    return ids;
}

// Removes a file, or a directory and all it holds, from an open directory.
static void removeTree(int parent, const char *name)
{
    const int fd = openat(parent, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (fd >= 0) {
        if (DIR *dir = fdopendir(fd)) {
            while (const dirent *entry = readdir(dir)) {
                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                    continue;
                if (unlinkat(fd, entry->d_name, 0) != 0)
                    removeTree(fd, entry->d_name);
            }
            closedir(dir);
        } else {
            close(fd);
        }
    }
    unlinkat(parent, name, AT_REMOVEDIR);
}

void removeOthers(const std::string &root, const std::string &keep)
{
    DIR *dir = opendir(root.c_str());
    if (!dir)
        return;
    const int fd = dirfd(dir);
    // Removing while reading the directory could make readdir skip entries.
    std::vector<std::string> names;
    while (const dirent *entry = readdir(dir)) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") || keep == entry->d_name)
            continue;
        names.push_back(entry->d_name);
    }
    for (const std::string &name : names) {
        if (unlinkat(fd, name.c_str(), 0) != 0)
            removeTree(fd, name.c_str());
    }
    closedir(dir);
}

// CFFIXED_USER_HOME when it is a path from the root, else the home of the user; empty without one.
static std::string homeDirectory()
{
    const char *home = secure_getenv("CFFIXED_USER_HOME");
    if (home && home[0] == '/')
        return home;
    const passwd *user = getpwuid(getuid());
    if (user && user->pw_dir && user->pw_dir[0])
        return user->pw_dir;
    return {};
}

// Where the caches of all builds are.
static std::string cacheRoot()
{
    const std::string home = homeDirectory();
    if (home.empty())
        return {};
    return home + "/.cache/cutefish-reef/metal";
}

// The cache of this build. The first time, a thread removes the caches of the others.
static std::string directory()
{
    const std::string root = cacheRoot();
    if (root.empty() || libraryIds().empty())
        return {};
    const std::string name = hexOf(llvm::SHA256::hash(llvm::arrayRefFromStringRef(libraryIds()))).substr(0, 16);
    static std::once_flag cleaned;
    std::call_once(cleaned, [root, name] {
        std::thread([root, name] { removeOthers(root, name); }).detach();
    });
    return root + "/" + name;
}

static std::string pathOf(const std::string &key)
{
    const std::string folder = directory();
    if (folder.empty())
        return {};
    return folder + "/" + hexOf(llvm::SHA256::hash(llvm::arrayRefFromStringRef(key)));
}

static bool readFile(const std::string &key, std::vector<char> &bytes)
{
    const std::string path = pathOf(key);
    if (path.empty())
        return false;
    FILE *file = fopen(path.c_str(), "rb");
    if (!file)
        return false;
    char buffer[16384];
    while (const size_t count = fread(buffer, 1, sizeof buffer, file))
        bytes.insert(bytes.end(), buffer, buffer + count);
    fclose(file);
    return true;
}

// Readers see the whole file or none: it is written under a name of its own, then renamed.
static void writeFile(const std::string &key, const std::vector<char> &bytes)
{
    const std::string path = pathOf(key);
    if (path.empty())
        return;
    const std::string folder = path.substr(0, path.rfind('/'));
    for (size_t slash = folder.find('/', 1); slash != std::string::npos; slash = folder.find('/', slash + 1))
        mkdir(folder.substr(0, slash).c_str(), 0700);
    mkdir(folder.c_str(), 0700);
    const std::string temporary = path + "." + std::to_string(getpid()) + ".tmp";
    FILE *file = fopen(temporary.c_str(), "wb");
    if (!file)
        return;
    const bool written = fwrite(bytes.data(), 1, bytes.size(), file) == bytes.size();
    if (fclose(file) != 0 || !written || rename(temporary.c_str(), path.c_str()) != 0)
        remove(temporary.c_str());
}

bool loadCompiledSource(const std::string &key, CompiledSource &source)
{
    std::vector<char> bytes;
    if (!readFile(key, bytes))
        return false;
    Reader reader{bytes};
    uint64_t count;
    if (!getMagic(reader, compiledMagic) || !getString(reader, source.bitcode) || !get(reader, count) || count > bytes.size())
        return false;
    source.functions.resize(count);
    for (CompiledFunction &function : source.functions) {
        if (!getString(reader, function.name) || !get(reader, function.stage))
            return false;
    }
    return reader.position == bytes.size();
}

void storeCompiledSource(const std::string &key, const CompiledSource &source)
{
    std::vector<char> bytes(std::begin(compiledMagic), std::end(compiledMagic));
    putString(bytes, source.bitcode);
    put(bytes, uint64_t(source.functions.size()));
    for (const CompiledFunction &function : source.functions) {
        putString(bytes, function.name);
        put(bytes, function.stage);
    }
    writeFile(key, bytes);
}

std::shared_ptr<Shader> loadShader(const std::string &key)
{
    std::vector<char> bytes;
    if (!readFile(key, bytes))
        return nullptr;
    auto shader = std::make_shared<Shader>();
    Reader reader{bytes};
    const char *glsl = nullptr;
    if (!getMagic(reader, shaderMagic) || !get(reader, shader->stage) || !getText(reader, *shader, glsl) || !glsl
        || !get(reader, shader->flags) || !getArray(reader, shader->bindings) || !getArray(reader, shader->samplers)
        || !getArray(reader, shader->attributes) || !getArray(reader, shader->varyings)
        || !getArray(reader, shader->renderTargets) || !get(reader, shader->clipDistances)
        || !get(reader, shader->uniformLocations) || !get(reader, shader->threadgroupSize))
        return nullptr;
    shader->glsl = glsl;
    for (reef_mtl_binding &binding : shader->bindings) {
        if (!getText(reader, *shader, binding.name) || !getText(reader, *shader, binding.variable))
            return nullptr;
    }
    for (reef_mtl_attribute &attribute : shader->attributes) {
        if (!getText(reader, *shader, attribute.name))
            return nullptr;
    }
    for (reef_mtl_varying &varying : shader->varyings) {
        if (!getText(reader, *shader, varying.name) || !getText(reader, *shader, varying.variable))
            return nullptr;
    }
    if (reader.position != bytes.size())
        return nullptr;
    return shader;
}

void storeShader(const std::string &key, const Shader &shader)
{
    std::vector<char> bytes(std::begin(shaderMagic), std::end(shaderMagic));
    put(bytes, shader.stage);
    putText(bytes, shader.glsl.c_str());
    put(bytes, shader.flags);
    putArray(bytes, shader.bindings);
    putArray(bytes, shader.samplers);
    putArray(bytes, shader.attributes);
    putArray(bytes, shader.varyings);
    putArray(bytes, shader.renderTargets);
    put(bytes, shader.clipDistances);
    put(bytes, shader.uniformLocations);
    put(bytes, shader.threadgroupSize);
    for (const reef_mtl_binding &binding : shader.bindings) {
        putText(bytes, binding.name);
        putText(bytes, binding.variable);
    }
    for (const reef_mtl_attribute &attribute : shader.attributes)
        putText(bytes, attribute.name);
    for (const reef_mtl_varying &varying : shader.varyings) {
        putText(bytes, varying.name);
        putText(bytes, varying.variable);
    }
    writeFile(key, bytes);
}

}
