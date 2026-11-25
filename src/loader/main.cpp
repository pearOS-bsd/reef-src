// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The loader of Mac programs: reef [options] <program or .app> [arguments...]

#include "bundle.h"
#include "host.h"
#include "launch.h"
#include "linker.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pwd.h>
#include <sys/auxv.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace cutefish::reef;

// Started again by the system for a window that is not shown: no question, no new process.
static bool headless(int argc, char** argv, int index)
{
    for (int i = index + 1; i + 1 < argc; i++) {
        if (strcmp(argv[i], "-ReefHeadless") == 0 && strcmp(argv[i + 1], "YES") == 0)
            return true;
    }
    return false;
}

// The name of the application of a program in Contents/MacOS; empty for another program.
static std::string applicationName(const std::string& program)
{
    const size_t slash = program.rfind('/');
    if (slash == std::string::npos || slash + 1 >= program.size())
        return {};
    static const std::string folder = ".app/Contents/MacOS";
    const std::string directory(program, 0, slash);
    if (!directory.ends_with(folder))
        return {};
    return program.substr(slash + 1);
}

static std::string absolutePath(const std::string& path)
{
    if (path.empty() || path[0] == '/')
        return path;
    char directory[4096];
    if (!getcwd(directory, sizeof directory))
        return {};
    return std::string(directory) + '/' + path;
}

// Where the frameworks and libraries of the layer are.
static std::string rootPath()
{
    char self[4096];
    const ssize_t length = readlink("/proc/self/exe", self, sizeof self - 1);
    if (length <= 0)
        return {};
    std::string path(self, size_t(length));
    path.erase(path.rfind('/'));
    return path + "/../lib/cutefish-reef";
}

// Variables of the session that an application does not change with LSEnvironment.
static bool sessionVariable(const std::string& name)
{
    return name == "PATH" || name == "HOME" || name == "DISPLAY" || name == "WAYLAND_DISPLAY" || name == "DBUS_SESSION_BUS_ADDRESS" ||
           name == "DBUS_SYSTEM_BUS_ADDRESS" || name == "XDG_RUNTIME_DIR";
}

// The home folder, as Mac programs see it.
static std::string homeFolder()
{
    const char* home = secure_getenv("CFFIXED_USER_HOME");
    if (home && *home == '/')
        return home;
    const passwd* user = getpwuid(getuid());
    if (user && user->pw_dir && *user->pw_dir)
        return user->pw_dir;
    return {};
}

static bool makeFolders(const std::string& folder)
{
    std::string partial;
    for (size_t at = 1; at <= folder.size(); at++) {
        at = folder.find('/', at);
        partial = folder.substr(0, at);
        if (mkdir(partial.c_str(), 0755) != 0 && errno != EEXIST)
            return false;
        if (at == std::string::npos)
            break;
    }
    return true;
}

// A link to the loader named after the application, so that the process has its name.
static bool makeLink(const std::string& folder, const std::string& link, const char* self)
{
    if (!makeFolders(folder))
        return false;
    const std::string temporary = link + ".new-" + std::to_string(getpid());
    unlink(temporary.c_str());
    if (symlink(self, temporary.c_str()) != 0)
        return false;
    if (rename(temporary.c_str(), link.c_str()) != 0) {
        unlink(temporary.c_str());
        return false;
    }
    return true;
}

// A document the application is asked to open: a file that exists or a URL.
static bool launchItem(const std::string& argument)
{
    if (argument.starts_with("file://"))
        return true;
    const size_t colon = argument.find(':');
    if (colon != std::string::npos && colon != 0 && argument[0] != '/') {
        const std::string scheme = argument.substr(0, colon);
        if (scheme != "file" && scheme.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+.-") == std::string::npos)
            return true;
    }
    struct stat status;
    return stat(argument.c_str(), &status) == 0;
}

// The documents among the arguments; options and their values stay.
static std::vector<std::string> takeLaunchItems(std::vector<std::string>& arguments)
{
    std::vector<std::string> items;
    std::vector<std::string> rest;
    for (size_t i = 0; i < arguments.size();) {
        const std::string& argument = arguments[i];
        if (!argument.empty() && argument[0] == '-') {
            rest.push_back(argument);
            if (argument.size() > 1 && i + 1 < arguments.size() && (arguments[i + 1].empty() || arguments[i + 1][0] != '-')) {
                rest.push_back(arguments[i + 1]);
                i += 2;
            } else {
                i += 1;
            }
            continue;
        }
        if (launchItem(argument))
            items.push_back(argument);
        else
            rest.push_back(argument);
        i++;
    }
    arguments = std::move(rest);
    return items;
}

int main(int argc, char** argv)
{
    tuneAllocator();
    prctl(PR_SET_THP_DISABLE, 1, 0, 0);
    if (argc == 2 && strcmp(argv[1], "--mach-broker") == 0)
        return machBroker();
    std::string imageMap;
    std::string argv0;
    bool list = false;
    bool json = false;
    bool launchItems = false;
    int stubMode = 0;
    int index = 1;
    for (; index < argc; index++) {
        const char* option = argv[index];
        if (option[0] != '-' || option[1] != '-')
            break;
        if (strncmp(option, "--image-map=", 12) == 0)
            imageMap = option + 12;
        else if (strncmp(option, "--argv0=", 8) == 0)
            argv0 = option + 8;
        else if (strncmp(option, "--exception-ports=", 18) == 0)
            setExceptionPorts(option + 18);
        else if (strcmp(option, "--stub-missing") == 0)
            stubMode = 1;
        else if (strcmp(option, "--stub-missing=zero") == 0)
            stubMode = 2;
        else if (strcmp(option, "--launch-items") == 0)
            launchItems = true;
        else if (strcmp(option, "--list-missing") == 0) {
            list = true;
            json = false;
        } else if (strcmp(option, "--list-missing=json") == 0) {
            list = true;
            json = true;
        } else
            break;
    }
    if (index >= argc) {
        fprintf(stderr, "Usage: reef [--image-map=<file>] [--stub-missing[=zero]] [--launch-items] <program or .app> [arguments...]\n"
                        "       reef --list-missing[=json] <program or bundle>\n");
        return 2;
    }
    if (list) {
        Linker linker(rootPath());
        return listMissing(linker, argv[index], json);
    }
    const std::string executable = bundleExecutable(argv[index]);
    if (executable.empty()) {
        struct stat status;
        if (stat(argv[index], &status) == 0 && S_ISDIR(status.st_mode)) {
            fprintf(stderr, "reef: %s: no main executable in the bundle\n", argv[index]);
            return 2;
        }
    } else {
        if (!headless(argc, argv, index)) {
            const std::string bundle = withoutTrailingSlashes(argv[index]);
            std::string identifier = infoString(bundle, "CFBundleIdentifier");
            if (identifier.empty())
                identifier = bundle.substr(bundle.rfind('/') + 1);
            launchThroughOpen(bundle, identifier, absolutePath(executable), argv);
        }
        launchItems = true;
        argv[index] = const_cast<char*>(executable.c_str());
    }
    const bool inApplication = !applicationName(argv[index]).empty();
    if (inApplication && launchItems) {
        const std::string program = argv[index];
        const std::string bundle(program, 0, program.rfind("/Contents/MacOS/"));
        for (const auto& [name, value] : infoDictionary(bundle, "LSEnvironment")) {
            const bool kept = name.empty() || name.find('=') != std::string::npos || sessionVariable(name);
            fprintf(stderr, "reef: LSEnvironment %s %s\n", name.c_str(), kept ? "kept from the session" : "set");
            if (!kept)
                setenv(name.c_str(), value.c_str(), 1);
        }
    }
    if (const std::string name = applicationName(argv[index]); !name.empty() && !headless(argc, argv, index)) {
        // The process takes the name of the application: it starts again through a link named after it.
        if (const std::string home = homeFolder(); !home.empty()) {
            const std::string folder = home + "/.local/share/cutefish-reef/bin";
            const std::string link = folder + "/" + name;
            const char* started = reinterpret_cast<const char*>(getauxval(AT_EXECFN));
            if (!started || link != started) {
                char self[4096];
                const ssize_t length = readlink("/proc/self/exe", self, sizeof self - 1);
                if (length > 0) {
                    self[length] = '\0';
                    char target[4096];
                    const ssize_t targetLength = readlink(link.c_str(), target, sizeof target - 1);
                    if ((targetLength >= 0 && std::string(target, size_t(targetLength)) == self) || makeLink(folder, link, self)) {
                        const std::string program = absolutePath(argv[index]);
                        if (!program.empty()) {
                            std::vector<char*> arguments(argv, argv + argc);
                            arguments[0] = const_cast<char*>(program.c_str());
                            if (!executable.empty())
                                arguments.insert(arguments.begin() + 1, const_cast<char*>("--launch-items"));
                            arguments.push_back(nullptr);
                            execv(link.c_str(), arguments.data());
                        }
                    }
                }
            }
        }
    }
    if (inApplication && !headless(argc, argv, index))
        setProgramPath(absolutePath(argv[index]));
    const char* program = argv[index];
    char shortName[16] = {};
    if (const char* slash = strrchr(program, '/'))
        program = slash + 1;
    if (*strncpy(shortName, program, 15))
        prctl(PR_SET_NAME, shortName);
    std::vector<std::string> arguments(argv + index + 1, argv + argc);
    Linker linker(rootPath());
    if (!imageMap.empty())
        linker.setImageMap(imageMap);
    linker.setStubMode(StubMode(stubMode));
    linker.setArgv0(argv0);
    if (launchItems)
        linker.setLaunchItems(takeLaunchItems(arguments));
    const int status = linker.run(argv[index], arguments, environ);
    std::string messages;
    for (const std::string& line : reports())
        messages += (messages.empty() ? "" : "\n") + line;
    if (!messages.empty())
        showUnsupported(false, messages);
    return status;
}
