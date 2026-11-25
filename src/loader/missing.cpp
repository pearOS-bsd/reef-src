// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// reef --list-missing[=json]: what a Mac program or bundle needs that reef does not have: libraries, symbols,
// Objective-C classes, and selectors that no class implements.

#include "bundle.h"
#include "image.h"
#include "linker.h"
#include "macho.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <deque>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace cutefish::reef {

// Reads the Objective-C metadata of a mapped image.
struct ObjCReader
{
    Image* image;
    // Pointers are chained fixups: a target, not an address.
    bool chained;

    // The value of a pointer of the image; 0 for a bind or what is outside the mapping.
    uint64_t pointer(uint64_t address) const
    {
        const uint64_t* at = reinterpret_cast<const uint64_t*>(image->slide + address);
        if (!address || uintptr_t(at) < image->reservation || image->reservationSize < 8 ||
            uintptr_t(at) > image->reservation + image->reservationSize - 8 || !at)
            return 0;
        uint64_t value = *at;
        if (chained) {
            if (int64_t(value) < 0)
                return 0;
            value &= 0xfffffffff;
            if (value < image->preferredBase)
                value += image->preferredBase;
        }
        return value;
    }

    const uint32_t* word(uint64_t address) const
    {
        const uint32_t* at = reinterpret_cast<const uint32_t*>(image->slide + address);
        if (!address || uintptr_t(at) < image->reservation || image->reservationSize < 4 ||
            uintptr_t(at) > image->reservation + image->reservationSize - 4 || !at)
            return nullptr;
        return at;
    }

    std::string_view string(uint64_t address) const
    {
        const char* text = reinterpret_cast<const char*>(image->slide + address);
        if (!address || uintptr_t(text) < image->reservation || !image->reservationSize ||
            uintptr_t(text) > image->reservation + image->reservationSize - 1 || !text)
            return {};
        const size_t limit = image->reservation + image->reservationSize - uintptr_t(text);
        const size_t length = strnlen(text, limit);
        if (length == limit)
            return {};
        return {text, length};
    }

    // The names of a method list: pointers to selectors, or relative offsets.
    void addMethods(uint64_t list, std::set<std::string>& names) const
    {
        if (!pointerAt(list))
            return;
        const uint32_t* flags = word(list);
        const uint32_t* count = word(list + 4);
        if (!flags || !count)
            return;
        const uint32_t size = *flags & 0xfffc;
        if (*flags & 0x80000000) {
            if (*count - 1 >= 0x100000 || size < 12)
                return;
            uint64_t method = list + 8;
            for (uint32_t i = 0; i < *count; i++, method += size) {
                const uint32_t* name = word(method);
                if (!name)
                    continue;
                uint64_t selector = method + int32_t(*name);
                if (!(*flags & 0x40000000)) {
                    if (!selector || !pointerAt(selector))
                        continue;
                    selector = pointer(selector);
                }
                if (const std::string_view text = string(selector); !text.empty())
                    names.emplace(text);
            }
        } else {
            if (*count - 1 >= 0x100000 || size < 24)
                return;
            uint64_t method = list + 8;
            for (uint32_t i = 0; i < *count; i++, method += size) {
                if (!pointerAt(method))
                    continue;
                if (const std::string_view text = string(pointer(method)); !text.empty())
                    names.emplace(text);
            }
        }
    }

    // The accessors of a property list: the getter and the setter, or the ones the attributes name.
    void addProperties(uint64_t list, std::set<std::string>& names) const
    {
        if (!pointerAt(list))
            return;
        const uint32_t* flags = word(list);
        const uint32_t* count = word(list + 4);
        if (!flags || !count)
            return;
        const uint32_t size = *flags & 0xfffc;
        if (*count - 1 >= 0x100000 || size < 16)
            return;
        uint64_t property = list + 8;
        for (uint32_t i = 0; i < *count; i++, property += size) {
            if (!pointerAt(property))
                continue;
            const std::string_view name = string(pointer(property));
            if (name.empty())
                continue;
            std::string getter(name);
            std::string setter = "set" + std::string(name) + ":";
            setter[3] = char(toupper(static_cast<unsigned char>(setter[3])));
            const std::string_view attributes = string(pointer(property + 8));
            if (!attributes.empty()) {
                for (size_t start = 0; start < attributes.size();) {
                    size_t end = attributes.find(',', start);
                    if (end == std::string_view::npos)
                        end = attributes.size();
                    const std::string_view attribute = attributes.substr(start, end - start);
                    if (attribute.size() > 1) {
                        if (attribute[0] == 'G')
                            getter = attribute.substr(1);
                        else if (attribute[0] == 'S')
                            setter = attribute.substr(1);
                    }
                    start = end + 1;
                }
            }
            names.insert(getter);
            names.insert(setter);
        }
    }

    // Each pointer of a section that is not null.
    template <typename Visit>
    void forEachPointer(std::string_view name, Visit visit) const
    {
        for (const Section& section : image->sections) {
            if (section.name != name)
                continue;
            for (uint64_t offset = 0; offset + 8 <= section.size; offset += 8) {
                if (const uint64_t value = pointer(section.address + offset))
                    visit(value);
            }
        }
    }

    bool pointerAt(uint64_t address) const
    {
        const uintptr_t at = image->slide + address;
        return address && at >= image->reservation && image->reservationSize >= 8 && at <= image->reservation + image->reservationSize - 8 && at;
    }
};

namespace {

// A program, library or bundle of what is listed.
struct Entry
{
    std::string path;
    Image* image = nullptr;
    std::string error;
};

struct AbsentLibrary
{
    std::string reason;
    // Only when every image links it weakly.
    bool weak = true;
    std::set<std::string> from;
};

struct Symbol
{
    bool strong = false;
    std::set<std::string> from;
    // How images use a class: classref, superref, subclass, category or other.
    std::set<std::string> uses;
};

struct Library
{
    bool absent = false;
    std::map<std::string, Symbol> symbols;
    std::set<std::string> from;
};

struct Class
{
    bool strong = false;
    std::set<std::string> uses;
    std::set<std::string> libraries;
    std::set<std::string> from;
};

} // namespace

// The C string at an address of the image.
static std::string_view stringAt(const Image& image, uint64_t address)
{
    const char* text = reinterpret_cast<const char*>(image.slide + address);
    if (!address || uintptr_t(text) < image.reservation || !image.reservationSize || uintptr_t(text) > image.reservation + image.reservationSize - 1 ||
        !text)
        return {};
    const size_t limit = image.reservation + image.reservationSize - uintptr_t(text);
    const size_t length = strnlen(text, limit);
    if (length == limit)
        return {};
    return {text, length};
}

static bool classSymbol(std::string_view name)
{
    return name.starts_with("_OBJC_CLASS_$_") || name.starts_with("_OBJC_METACLASS_$_");
}

static std::string_view className(std::string_view symbol)
{
    if (symbol.starts_with("_OBJC_METACLASS_$_"))
        return symbol.substr(18);
    if (symbol.starts_with("_OBJC_CLASS_$_"))
        return symbol.substr(14);
    return {};
}

// Where an image refers to a class: the section of the bind.
static std::string classUse(const Image& image, uint64_t address)
{
    if (address) {
        for (const Section& section : image.sections) {
            if (section.address <= address && address < section.address + section.size) {
                if (section.name == "__objc_classrefs")
                    return "classref";
                if (section.name == "__objc_superrefs")
                    return "superref";
                if (section.name == "__objc_data")
                    return "subclass";
                if (section.name == "__objc_const")
                    return "category";
                break;
            }
        }
    }
    return "other";
}

// Whether an image exports a symbol, itself or through a library it re-exports.
static bool defines(Image* image, std::string_view name, int depth)
{
    if (depth == 33)
        return false;
    if (const std::optional<Export> symbol = image->findExport(name)) {
        if (symbol->kind != ExportKind::Reexport)
            return true;
        if (symbol->ordinal < 1 || size_t(symbol->ordinal) > image->dylibs.size())
            return false;
        Image* library = image->dylibs[symbol->ordinal - 1].image;
        if (!library)
            return false;
        return defines(library, symbol->name.empty() ? name : std::string_view(symbol->name), depth + 1);
    }
    for (const Dylib& dylib : image->dylibs) {
        if (dylib.kind == DylibKind::Reexport && dylib.image && defines(dylib.image, name, depth + 1))
            return true;
    }
    return false;
}

static std::string json(std::string_view text)
{
    std::string quoted = "\"";
    for (const char c : text) {
        switch (c) {
        case '"':
            quoted += "\\\"";
            break;
        case '\\':
            quoted += "\\\\";
            break;
        case '\n':
            quoted += "\\n";
            break;
        case '\t':
            quoted += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) {
                char escaped[8];
                snprintf(escaped, sizeof escaped, "\\u%04x", c);
                quoted += escaped;
            } else {
                quoted += c;
            }
        }
    }
    quoted += '"';
    return quoted;
}

static std::string json(const std::set<std::string>& values)
{
    std::string array = "[";
    for (const std::string& value : values)
        array += (array.size() > 1 ? ", " : "") + json(value);
    array += ']';
    return array;
}

static std::string joined(const std::set<std::string>& values)
{
    std::string text;
    for (const std::string& value : values)
        text += (text.empty() ? "" : ", ") + value;
    return text;
}

// Whether a file starts like a Mach-O file of one architecture or a universal one.
static bool machO(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);
    uint32_t header[2];
    if (!file.read(reinterpret_cast<char*>(header), sizeof header))
        return false;
    if (header[0] == MH_MAGIC_64)
        return true;
    if ((header[0] & 0xfeffffff) == FAT_CIGAM)
        return __builtin_bswap32(header[1]) - 1 < 31;
    return false;
}

static const char* kind(const Entry& entry)
{
    if (!entry.image)
        return "failed";
    if (entry.image->fileType == MH_EXECUTE)
        return "executable";
    if (entry.image->fileType == MH_BUNDLE)
        return "bundle";
    return "dylib";
}

static constexpr const char* selectorsNote = "heuristic: selector references no class of the app or of reef implements; the app's property "
                                             "accessors and the methods of protocols it carries count as implemented";

int listMissing(Linker& linker, const std::string& path, bool json)
{
    std::error_code error;
    const std::filesystem::path target = std::filesystem::weakly_canonical(std::filesystem::absolute(path), error);
    if (error || !std::filesystem::exists(target)) {
        fprintf(stderr, "reef: %s: no such file or directory\n", path.c_str());
        return 2;
    }
    std::string root;
    std::vector<std::string> files;
    if (std::filesystem::is_directory(target)) {
        root = target.string();
        for (std::filesystem::recursive_directory_iterator it(target, std::filesystem::directory_options::skip_permission_denied, error);
             !error && it != std::filesystem::recursive_directory_iterator(); it.increment(error)) {
            if (it->is_regular_file(error) && !it->is_symlink(error) && machO(it->path()))
                files.push_back(it->path().string());
        }
        std::sort(files.begin(), files.end());
    } else {
        root = target.parent_path().string();
        files.push_back(target.string());
    }

    const auto relative = [&root](const std::string& file) {
        const std::string prefix = root + "/";
        if (!file.starts_with(prefix))
            return file;
        return file.substr(root.size() + 1);
    };

    std::vector<Entry> images;
    std::set<Image*> own;
    for (const std::string& file : files) {
        Entry entry;
        entry.path = file;
        entry.image = linker.load(file, entry.error);
        if (entry.image)
            own.insert(entry.image);
        images.push_back(std::move(entry));
    }

    // The main executable an image runs in: its own, or the one of the bundle around it.
    const auto mainExecutable = [&root, &linker](const Entry& entry) -> Image* {
        if (entry.image->fileType == MH_EXECUTE)
            return entry.image;
        std::filesystem::path directory = std::filesystem::path(entry.path).parent_path();
        while (true) {
            const std::string name = directory.string();
            if (name == root || name.ends_with(".app") || name.ends_with(".appex") || name.ends_with(".xpc")) {
                const std::string executable = bundleExecutable(name);
                const auto found = linker.m_byPath.find(executable);
                if (found != linker.m_byPath.end() && !executable.empty() && found->second->fileType == MH_EXECUTE)
                    return found->second;
            }
            if (name.size() <= root.size() || !directory.has_parent_path())
                return entry.image;
            directory = directory.parent_path();
        }
    };

    std::map<std::string, AbsentLibrary> absent;
    std::set<Image*> visited;
    // Loads what the image links, breadth first, and notes the libraries that are not there.
    const auto walk = [&visited, &linker, &absent, &relative](Image* first) {
        std::deque<Image*> queue{first};
        while (!queue.empty()) {
            Image* image = queue.front();
            queue.pop_front();
            if (!visited.insert(image).second)
                continue;
            for (Dylib& dylib : image->dylibs) {
                const std::optional<std::string> resolved = linker.resolveInstallName(dylib.path, image);
                std::string reason = "not found";
                if (resolved)
                    dylib.image = linker.load(*resolved, reason);
                if (dylib.image) {
                    queue.push_back(dylib.image);
                    continue;
                }
                AbsentLibrary& library = absent[dylib.path];
                if (library.reason.empty() || library.reason == "not found")
                    library.reason = std::move(reason);
                library.weak = library.weak && dylib.kind == DylibKind::Weak;
                library.from.insert(relative(image->path));
            }
        }
    };
    for (const bool executables : {true, false}) {
        for (const Entry& entry : images) {
            if (!entry.image || (entry.image->fileType == MH_EXECUTE) != executables || visited.contains(entry.image))
                continue;
            Image* main = mainExecutable(entry);
            linker.m_main = main;
            walk(main);
            walk(entry.image);
        }
    }

    // The symbols the images bind that their libraries do not export.
    std::map<std::string, Library> libraries;
    for (const Entry& entry : images) {
        Image* image = entry.image;
        if (!image)
            continue;
        const std::string from = relative(image->path);
        Image* main = mainExecutable(entry);
        for (const Bind& bind : image->binds()) {
            if (bind.name.empty())
                continue;
            std::string library;
            bool found = false;
            bool missingLibrary = false;
            if (bind.ordinal == 0) {
                library = "(the image itself)";
                found = defines(image, bind.name, 0);
            } else if (bind.ordinal == -1) {
                library = "(the main executable)";
                found = defines(main, bind.name, 0);
            } else if (bind.ordinal == -2 || bind.ordinal == -3) {
                library = bind.ordinal == -2 ? "(flat namespace)" : "(weak definitions)";
                for (const std::unique_ptr<Image>& other : linker.m_images) {
                    if (defines(other.get(), bind.name, 0)) {
                        found = true;
                        break;
                    }
                }
            } else if (bind.ordinal < 0 || size_t(bind.ordinal) > image->dylibs.size()) {
                library = "(ordinal " + std::to_string(bind.ordinal) + ")";
            } else {
                const Dylib& dylib = image->dylibs[bind.ordinal - 1];
                library = dylib.path;
                if (!dylib.image) {
                    missingLibrary = true;
                } else if (defines(dylib.image, bind.name, 0)) {
                    found = true;
                } else if (Image* umbrella = linker.frameworkImage(*dylib.image)) {
                    found = defines(umbrella, bind.name, 0);
                }
            }
            if (found)
                continue;
            Library& missing = libraries[library];
            missing.absent |= missingLibrary;
            missing.from.insert(from);
            Symbol& symbol = missing.symbols[std::string(bind.name)];
            symbol.strong = symbol.strong || !bind.weak;
            symbol.from.insert(from);
            if (classSymbol(bind.name))
                symbol.uses.insert(classUse(*image, bind.address));
        }
    }

    size_t strong = 0;
    size_t weak = 0;
    std::map<std::string, Class> classes;
    for (const auto& [libraryName, library] : libraries) {
        for (const auto& [name, symbol] : library.symbols) {
            if (symbol.strong)
                strong++;
            else
                weak++;
            if (!classSymbol(name))
                continue;
            const std::string_view base = className(name);
            if (base.empty())
                continue;
            Class& cls = classes[std::string(base)];
            cls.strong = cls.strong || symbol.strong;
            cls.uses.insert(symbol.uses.begin(), symbol.uses.end());
            cls.libraries.insert(libraryName);
            cls.from.insert(symbol.from.begin(), symbol.from.end());
        }
    }

    // The selectors the program references, and the ones that classes implement.
    std::set<std::string> implemented;
    std::map<std::string, std::set<std::string>> references;
    for (const std::unique_ptr<Image>& owned : linker.m_images) {
        Image* image = owned.get();
        if (!image->hasObjC())
            continue;
        const auto [fixups, fixupsEnd] = image->linkedit(image->chainedOffset, image->chainedSize);
        const ObjCReader reader{image, fixups != fixupsEnd};
        bool ownImage = own.contains(image);
        const auto addClass = [&reader, &implemented, &ownImage](uint64_t cls) {
            const uint64_t data = reader.pointer(cls + 0x20) & ~uint64_t(7);
            reader.addMethods(reader.pointer(data + 0x20), implemented);
            if (ownImage)
                reader.addProperties(reader.pointer(data + 0x40), implemented);
        };
        reader.forEachPointer("__objc_classlist", [&](uint64_t cls) {
            addClass(cls);
            if (const uint64_t metaclass = reader.pointer(cls))
                addClass(metaclass);
        });
        const auto addCategory = [&reader, &implemented, &ownImage](uint64_t category) {
            reader.addMethods(reader.pointer(category + 0x10), implemented);
            reader.addMethods(reader.pointer(category + 0x18), implemented);
            if (ownImage)
                reader.addProperties(reader.pointer(category + 0x28), implemented);
        };
        reader.forEachPointer("__objc_catlist", addCategory);
        reader.forEachPointer("__objc_catlist2", addCategory);
        if (ownImage) {
            reader.forEachPointer("__objc_protolist", [&](uint64_t protocol) {
                for (uint64_t list = protocol + 0x18; list != protocol + 0x38; list += 8)
                    reader.addMethods(reader.pointer(list), implemented);
                reader.addProperties(reader.pointer(protocol + 0x38), implemented);
            });
        }
        if (own.contains(image)) {
            std::set<std::string> selectors;
            for (const Section& section : image->sections) {
                if (section.name != std::string_view("__objc_selrefs"))
                    continue;
                for (uint64_t offset = 0; offset + 8 <= section.size; offset += 8) {
                    if (const uint64_t selector = reader.pointer(section.address + offset)) {
                        if (const std::string_view name = stringAt(*image, selector); name.data())
                            selectors.emplace(name);
                    }
                }
            }
            for (const std::string& selector : selectors)
                references[selector].insert(relative(image->path));
        }
    }
    std::map<std::string, std::set<std::string>> missingSelectors;
    for (const auto& [selector, from] : references) {
        if (!implemented.contains(selector))
            missingSelectors.try_emplace(selector, from);
    }

    // Libraries that export a symbol the program misses, among those reef loaded for it.
    const auto exportedBy = [&linker, &own](const std::string& name) {
        std::set<std::string> exporters;
        for (const std::unique_ptr<Image>& image : linker.m_images) {
            if (own.contains(image.get()))
                continue;
            if (!image->installName.empty() && image->findExport(name))
                exporters.insert(image->installName);
        }
        return exporters;
    };

    size_t failed = 0;
    for (const Entry& entry : images)
        failed += !entry.image;

    if (json) {
        printf("{\n  \"path\": %s,\n", reef::json(path).c_str());
        printf("  \"summary\": {\"images\": %zu, \"failedImages\": %zu, \"absentLibraries\": %zu, \"strongSymbols\": %zu, \"weakSymbols\": %zu, "
               "\"classes\": %zu, \"selectors\": %zu},\n",
               images.size(), failed, absent.size(), strong, weak, classes.size(), missingSelectors.size());
        printf("  \"images\": [");
        for (size_t i = 0; i < images.size(); i++) {
            const Entry& entry = images[i];
            printf("%s\n    {\"path\": %s, \"kind\": \"%s\"", i ? "," : "", reef::json(relative(entry.path)).c_str(), kind(entry));
            if (!entry.image)
                printf(", \"error\": %s", reef::json(entry.error).c_str());
            printf("}");
        }
        printf("\n  ],\n  \"absentLibraries\": [");
        bool first = true;
        for (const auto& [installName, library] : absent) {
            printf("%s\n    {\"installName\": %s, \"reason\": %s, \"weak\": %s, \"from\": %s}", first ? "" : ",", reef::json(installName).c_str(),
                   reef::json(library.reason).c_str(), library.weak ? "true" : "false", reef::json(library.from).c_str());
            first = false;
        }
        printf("\n  ],\n  \"symbols\": [");
        first = true;
        for (const auto& [name, library] : libraries) {
            printf("%s\n    {\"library\": %s, \"absent\": %s, \"from\": %s, \"symbols\": [", first ? "" : ",", reef::json(name).c_str(),
                   library.absent ? "true" : "false", reef::json(library.from).c_str());
            bool firstSymbol = true;
            for (const auto& [symbolName, symbol] : library.symbols) {
                printf("%s\n      {\"name\": %s, \"strong\": %s, \"from\": %s", firstSymbol ? "" : ",", reef::json(symbolName).c_str(),
                       symbol.strong ? "true" : "false", reef::json(symbol.from).c_str());
                if (!library.absent) {
                    const std::set<std::string> exporters = exportedBy(symbolName);
                    if (!exporters.empty())
                        printf(", \"exportedBy\": %s", reef::json(exporters).c_str());
                }
                printf("}");
                firstSymbol = false;
            }
            printf("\n    ]}");
            first = false;
        }
        printf("\n  ],\n  \"classes\": [");
        first = true;
        for (const auto& [name, cls] : classes) {
            printf("%s\n    {\"name\": %s, \"strong\": %s, \"uses\": %s, \"libraries\": %s, \"from\": %s}", first ? "" : ",", reef::json(name).c_str(),
                   cls.strong ? "true" : "false", reef::json(cls.uses).c_str(), reef::json(cls.libraries).c_str(), reef::json(cls.from).c_str());
            first = false;
        }
        printf("\n  ],\n  \"selectorsNote\": %s,\n  \"selectors\": [", reef::json(selectorsNote).c_str());
        first = true;
        for (const auto& [selector, from] : missingSelectors) {
            printf("%s\n    {\"name\": %s, \"from\": %s}", first ? "" : ",", reef::json(selector).c_str(), reef::json(from).c_str());
            first = false;
        }
        printf("\n  ]\n}\n");
    } else {
        printf("reef --list-missing %s\n", path.c_str());
        printf("summary: %zu images (%zu failed), %zu absent libraries, %zu strong symbols, %zu weak symbols, %zu classes, %zu selectors\n", images.size(),
               failed, absent.size(), strong, weak, classes.size(), missingSelectors.size());
        printf("\nimages: %zu\n", images.size());
        for (const Entry& entry : images) {
            if (!entry.image)
                printf("    failed %s: %s\n", relative(entry.path).c_str(), entry.error.c_str());
            else
                printf("    %s %s\n", kind(entry), relative(entry.path).c_str());
        }
        printf("\nabsent libraries: %zu\n", absent.size());
        for (const auto& [installName, library] : absent) {
            printf("    %s%s: %s\n", installName.c_str(), library.weak ? " (weak)" : "", library.reason.c_str());
            printf("        from %s\n", joined(library.from).c_str());
        }
        printf("\nsymbols: %zu strong, %zu weak\n", strong, weak);
        for (const auto& [name, library] : libraries) {
            size_t strongSymbols = 0;
            for (const auto& [symbolName, symbol] : library.symbols)
                strongSymbols += symbol.strong;
            const auto absentLibrary = absent.find(name);
            const char* weakLibrary = library.absent && absentLibrary != absent.end() && absentLibrary->second.weak ? " (weak)" : "";
            printf("%s%s%s: %zu strong, %zu weak\n", library.absent ? "ABSENT " : "", name.c_str(), weakLibrary, strongSymbols,
                   library.symbols.size() - strongSymbols);
            printf("    from %s\n", joined(library.from).c_str());
            for (const bool strongFirst : {true, false}) {
                for (const auto& [symbolName, symbol] : library.symbols) {
                    if (symbol.strong != strongFirst)
                        continue;
                    std::string line = (strongFirst ? "    strong " : "    weak ") + symbolName;
                    if (symbol.from.size() < library.from.size())
                        line += "  (" + joined(symbol.from) + ")";
                    if (!library.absent) {
                        const std::set<std::string> exporters = exportedBy(symbolName);
                        if (!exporters.empty())
                            line += "  [exported by " + joined(exporters) + "]";
                    }
                    printf("%s\n", line.c_str());
                }
            }
        }
        printf("\nclasses: %zu\n", classes.size());
        for (const auto& [name, cls] : classes) {
            printf("    %s  %s  %s  from %s\n", name.c_str(), cls.strong ? "strong" : "weak", joined(cls.uses).c_str(), joined(cls.libraries).c_str());
            printf("        in %s\n", joined(cls.from).c_str());
        }
        printf("\nselectors (%s): %zu\n", selectorsNote, missingSelectors.size());
        for (const auto& [selector, from] : missingSelectors)
            printf("    %s  %s\n", selector.c_str(), joined(from).c_str());
    }
    return 0;
}

} // namespace cutefish::reef
