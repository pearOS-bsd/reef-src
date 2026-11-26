// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The command line of hdiutil: makehybrid, create, convert and verify, with the options of Apple's hdiutil that matter here.

#include "hdiutil.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <set>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

struct Arguments
{
    std::vector<std::string> positional;
    std::vector<std::pair<std::string, std::string>> values;
    std::set<std::string> flags;
};

// The options of Apple's hdiutil that take a value; the others are flags.
static const std::set<std::string> valued = {
    "-o", "-format", "-imagekey", "-encryption", "-certificate", "-pubkey", "-srcfolder", "-srcdir", "-srcdevice", "-volname", "-fs",
    "-fsargs", "-size", "-sectors", "-megabytes", "-layout", "-type", "-uid", "-gid", "-mode", "-segmentSize", "-tasks", "-shadow",
    "-cacert", "-default-volume-name", "-hfs-volume-name", "-iso-volume-name", "-joliet-volume-name", "-udf-volume-name", "-hide-hfs",
    "-hide-iso", "-hide-joliet", "-hide-udf", "-only-hfs", "-only-iso", "-only-joliet", "-only-udf", "-abstract-file",
    "-bibliography-file", "-copyright-file", "-application", "-preparer", "-publisher", "-system-id", "-eltorito-boot",
    "-eltorito-platform", "-boot-load-size", "-hfs-blessed-directory", "-hfs-openfolder", "-hfs-startupfile-size", "-udf-version",
    "-alternatemount",
};

inline std::string value(const Arguments& arguments, const std::string& name, const std::string& fallback = {})
{
    for (const auto& [option, given] : arguments.values) {
        if (option == name)
            return given;
    }
    return fallback;
}

inline std::vector<std::string> values(const Arguments& arguments, const std::string& name)
{
    std::vector<std::string> found;
    for (const auto& [option, given] : arguments.values) {
        if (option == name)
            found.push_back(given);
    }
    return found;
}

static bool has(const Arguments& arguments, const std::string& flag)
{
    return arguments.flags.count(flag) != 0;
}

// The name of a volume: its own option, then the general ones, then the name of the folder.
static std::string volumeName(const Arguments& arguments, const char* option, const Entry& root)
{
    std::string name = value(arguments, option);
    if (name.empty()) {
        name = value(arguments, "-default-volume-name");
        if (name.empty())
            name = value(arguments, "-volname");
    }
    return name.empty() ? root.name : name;
}

// -puppetstrings: the progress as Finder reads it.
static void startProgress(const Arguments& arguments)
{
    if (has(arguments, "-puppetstrings")) {
        printf("PERCENT:0.000000\n");
        fflush(stdout);
    }
}

static void printCreated(const std::string& path)
{
    char resolved[PATH_MAX];
    printf("created: %s\n", realpath(path.c_str(), resolved) ? resolved : path.c_str());
}

static void reportCreated(const Arguments& arguments, const std::string& path)
{
    if (has(arguments, "-quiet"))
        return;
    printCreated(path);
}

static Progress progressFor(const Arguments& arguments)
{
    if (!has(arguments, "-puppetstrings"))
        return {};
    auto last = std::make_shared<int>(-1);
    return [last](uint64_t done, uint64_t total) {
        const int permille = total ? int(done * 1000 / total) : 1000;
        if (*last == permille)
            return;
        *last = permille;
        printf("PERCENT:%f\n", permille / 10.0);
        fflush(stdout);
    };
}

// The path with the extension of the format, unless it has it already.
static std::string withExtension(const std::string& path, const char* extension)
{
    const size_t slash = path.rfind('/');
    const std::string name = slash == std::string::npos ? path : path.substr(slash + 1);
    const size_t size = strlen(extension);
    if (size < name.size() && strcasecmp(name.c_str() + name.size() - size, extension) == 0)
        return path;
    return path + extension;
}

static bool mayWrite(const Arguments& arguments, const char* path, std::string* error)
{
    bool exists;
    {
        struct stat status;
        exists = lstat(path, &status) == 0;
    }
    if (exists && !has(arguments, "-ov")) {
        *error = "File exists";
        return false;
    }
    return true;
}

// The image is made beside its place, then renamed.
static std::string temporaryFor(const std::string& path, std::string* error)
{
    const std::string pattern = path + ".hdiutil-XXXXXX";
    std::vector<char> name(pattern.begin(), pattern.end());
    name.push_back(0);
    const int fd = mkstemp(name.data());
    if (fd < 0) {
        *error = path + ": " + strerror(errno);
        return {};
    }
    close(fd);
    return name.data();
}

static bool finish(const std::string& temporary, const std::string& path, std::string* error)
{
    const mode_t mask = umask(0);
    umask(mask);
    chmod(temporary.c_str(), ~mask & 0666);
    if (rename(temporary.c_str(), path.c_str()) != 0) {
        *error = path + ": " + strerror(errno);
        unlink(temporary.c_str());
        return false;
    }
    return true;
}

static bool convertOptions(const Arguments& arguments, diskimage::ConvertOptions* options, std::string* error)
{
    const std::string format = value(arguments, "-format");
    options->format = diskimage::UDZO;
    if (!format.empty() && !diskimage::parseFormat(format, options)) {
        *error = "image format " + format + " is not supported";
        return false;
    }
    for (const std::string& key : values(arguments, "-imagekey")) {
        if (key.starts_with("zlib-level="))
            options->level = strtol(key.c_str() + 11, nullptr, 10);
        if (key.starts_with("bzip2-level="))
            options->level = strtol(key.c_str() + 12, nullptr, 10);
        if (key.starts_with("lzma-level="))
            options->level = strtol(key.c_str() + 11, nullptr, 10);
        if (key.starts_with("lzfse-level="))
            options->level = strtol(key.c_str() + 12, nullptr, 10);
    }
    if (!value(arguments, "-encryption", "").empty() || has(arguments, "-encryption")) {
        *error = "encrypted images are not supported";
        return false;
    }
    return true;
}

static int fail(const char* command, const std::string& error)
{
    fprintf(stderr, "hdiutil: %s failed - %s\n", command, error.c_str());
    return 1;
}

static int makehybrid(const Arguments& arguments)
{
    const std::string output = value(arguments, "-o");
    if (output.empty() || arguments.positional.size() != 1)
        return fail("makehybrid", "usage: hdiutil makehybrid -o image [options] source");
    Entry root;
    std::string error;
    if (!scanFolder(arguments.positional[0], root, &error))
        return fail("makehybrid", error);
    const bool hfs = has(arguments, "-hfs");
    HybridOptions options;
    options.iso = has(arguments, "-iso");
    options.joliet = has(arguments, "-joliet");
    options.udf = has(arguments, "-udf");
    if (!hfs && !options.iso && !options.joliet && !options.udf)
        options.iso = options.joliet = options.udf = true;
    if (!mayWrite(arguments, output.c_str(), &error))
        return fail("makehybrid", error);
    const std::string temporary = temporaryFor(output, &error);
    if (temporary.empty())
        return fail("makehybrid", error);
    if (!has(arguments, "-quiet") && !has(arguments, "-puppetstrings"))
        printf("Creating hybrid image...\n");
    startProgress(arguments);
    bool made;
    if (hfs && !options.iso && !options.joliet && !options.udf) {
        made = makeHfs(root, volumeName(arguments, "-hfs-volume-name", root), temporary, progressFor(arguments), &error);
    } else {
        if (hfs)
            fprintf(stderr, "hdiutil: makehybrid: an HFS+ volume beside ISO 9660 or UDF is not made\n");
        options.volumeName = volumeName(arguments, options.udf ? "-udf-volume-name" : "-iso-volume-name", root);
        made = makeHybrid(root, options, temporary, progressFor(arguments), &error);
    }
    if (!made) {
        unlink(temporary.c_str());
        return fail("makehybrid", error);
    }
    if (!finish(temporary, output, &error))
        return fail("makehybrid", error);
    return 0;
}

static int create(const Arguments& arguments)
{
    std::vector<std::string> sources = values(arguments, "-srcfolder");
    for (const std::string& source : values(arguments, "-srcdir"))
        sources.push_back(source);
    if (arguments.positional.size() != 1)
        return fail("create", "usage: hdiutil create -srcfolder folder [-volname name] [-format format] image");
    if (sources.size() != 1)
        return fail("create", "only images made from one folder are supported");
    const std::string fileSystem = value(arguments, "-fs", "HFS+");
    if (fileSystem.compare(0, 3, "HFS") != 0 && fileSystem != "Journaled HFS+")
        return fail("create", "file system " + fileSystem + " is not supported");
    std::string error;
    diskimage::ConvertOptions options;
    if (!convertOptions(arguments, &options, &error))
        return fail("create", error);
    const std::string path = withExtension(arguments.positional[0], diskimage::extension(options.format));
    if (!mayWrite(arguments, path.c_str(), &error))
        return fail("create", error);
    Entry root;
    if (!scanFolder(sources[0], root, &error))
        return fail("create", error);
    std::string name = value(arguments, "-volname");
    if (name.empty())
        name = root.name;
    const std::string raw = temporaryFor(path, &error);
    if (raw.empty())
        return fail("create", error);
    const std::string temporary = temporaryFor(path, &error);
    if (temporary.empty()) {
        unlink(raw.c_str());
        return fail("create", error);
    }
    startProgress(arguments);
    const Progress progress = progressFor(arguments);
    // The file system is the first half of the work, its conversion the second.
    const Progress first = progress ? Progress([&progress](uint64_t done, uint64_t total) { progress(done, total * 2); }) : Progress();
    if (!makeHfs(root, name, raw, first, &error)) {
        unlink(raw.c_str());
        unlink(temporary.c_str());
        return fail("create", error);
    }
    const Progress second =
        progress ? Progress([&progress](uint64_t done, uint64_t total) { progress(total + done, total * 2); }) : Progress();
    const bool converted = diskimage::convert(raw, temporary, options, second, &error);
    unlink(raw.c_str());
    if (!converted) {
        unlink(temporary.c_str());
        return fail("create", error);
    }
    if (!finish(temporary, path, &error))
        return fail("create", error);
    reportCreated(arguments, path);
    return 0;
}

static int convert(const Arguments& arguments)
{
    std::string output = value(arguments, "-o");
    if (output.empty() || arguments.positional.size() != 1)
        return fail("convert", "usage: hdiutil convert image -format format -o output");
    std::string error;
    diskimage::ConvertOptions options;
    if (!convertOptions(arguments, &options, &error))
        return fail("convert", error);
    output = withExtension(output, diskimage::extension(options.format));
    if (!mayWrite(arguments, output.c_str(), &error))
        return fail("convert", error);
    const std::string temporary = temporaryFor(output, &error);
    if (temporary.empty())
        return fail("convert", error);
    startProgress(arguments);
    if (!diskimage::convert(arguments.positional[0], temporary, options, progressFor(arguments), &error)) {
        unlink(temporary.c_str());
        return fail("convert", error);
    }
    if (!finish(temporary, output, &error))
        return fail("convert", error);
    reportCreated(arguments, output);
    return 0;
}

static int verify(const Arguments& arguments)
{
    if (arguments.positional.size() != 1)
        return fail("verify", "usage: hdiutil verify image");
    std::string report;
    std::string error;
    startProgress(arguments);
    const bool valid = diskimage::verify(arguments.positional[0], progressFor(arguments), &report, &error);
    if (!has(arguments, "-quiet") && !has(arguments, "-puppetstrings"))
        fputs(report.c_str(), stdout);
    if (!valid)
        return fail("verify", error);
    if (!has(arguments, "-quiet"))
        printf("hdiutil: verify: checksum of \"%s\" is VALID\n", arguments.positional[0].c_str());
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: hdiutil makehybrid|create|convert|verify ...\n");
        return 1;
    }
    const std::string command = argv[1];
    Arguments arguments;
    for (int i = 2; i < argc; i++) {
        const std::string argument = argv[i];
        if (argument.size() < 2 || argument[0] != '-')
            arguments.positional.push_back(argument);
        else if (valued.find(argument) != valued.end() && i + 1 < argc)
            arguments.values.emplace_back(argument, argv[++i]);
        else
            arguments.flags.insert(argument);
    }
    if (command == "makehybrid")
        return makehybrid(arguments);
    if (command == "create")
        return create(arguments);
    if (command == "convert")
        return convert(arguments);
    if (command == "verify")
        return verify(arguments);
    fprintf(stderr, "hdiutil: %s: not supported\n", command.c_str());
    return 1;
}
