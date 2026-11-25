// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Sandboxes of Mac programs, with Landlock and seccomp: the profile as libsandbox gives it, the rules for the files, the network
// and the system calls; and the decisions of the broker about what the rules of Landlock cannot say.

#include "sandbox.h"

#include "host.h"
#include "linker.h"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cerrno>
#include <climits>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <linux/audit.h>
#include <linux/filter.h>
#include <linux/landlock.h>
#include <linux/openat2.h>
#include <linux/seccomp.h>
#include <pwd.h>
#include <set>
#include <sstream>
#include <sys/prctl.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace cutefish::reef {

namespace {

constexpr uint64_t readAccess = LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_READ_DIR;
constexpr uint64_t writeAccess = LANDLOCK_ACCESS_FS_WRITE_FILE | LANDLOCK_ACCESS_FS_REMOVE_DIR | LANDLOCK_ACCESS_FS_REMOVE_FILE |
                                 LANDLOCK_ACCESS_FS_MAKE_DIR | LANDLOCK_ACCESS_FS_MAKE_REG | LANDLOCK_ACCESS_FS_MAKE_SOCK |
                                 LANDLOCK_ACCESS_FS_MAKE_FIFO | LANDLOCK_ACCESS_FS_MAKE_SYM | LANDLOCK_ACCESS_FS_REFER | LANDLOCK_ACCESS_FS_TRUNCATE;
constexpr uint64_t executeAccess = LANDLOCK_ACCESS_FS_EXECUTE;
constexpr uint64_t controlAccess = 1 << 15; // LANDLOCK_ACCESS_FS_IOCTL_DEV
// What a rule for a file may have.
constexpr uint64_t fileAccess =
    LANDLOCK_ACCESS_FS_EXECUTE | LANDLOCK_ACCESS_FS_WRITE_FILE | LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_TRUNCATE | controlAccess;
constexpr uint64_t openAccess = readAccess | LANDLOCK_ACCESS_FS_WRITE_FILE;

constexpr int maximumThreads = 4096;

struct Ruleset
{
    int fd;
    uint64_t handled;

    void allow(const std::string& path, uint64_t access) const
    {
        const int file = open(path.c_str(), O_PATH | O_CLOEXEC);
        if (file < 0)
            return;
        struct stat status;
        if (fstat(file, &status) == 0 && !S_ISDIR(status.st_mode))
            access &= fileAccess;
        landlock_path_beneath_attr attribute;
        attribute.allowed_access = access & handled;
        attribute.parent_fd = file;
        if (attribute.allowed_access)
            syscall(SYS_landlock_add_rule, fd, LANDLOCK_RULE_PATH_BENEATH, &attribute, 0);
        close(file);
    }
};

struct RulesetAttribute
{
    uint64_t files;
    uint64_t network;
    uint64_t scoped;
};

struct Filter
{
    uint64_t access;
    bool deny;
    std::string kind;
    std::string path;
};

struct Profile
{
    bool denyDefault = false;
    bool allowOutbound = false;
    bool allowInbound = false;
    uint64_t denied = 0;
    uint64_t restricted = 0;
    bool confineOutbound = false;
    bool confineInbound = false;
    bool graphics = false;
    bool audio = false;
    std::vector<Filter> filters;
};

// A path that is refused inside of one that is allowed.
struct Hole
{
    std::string path;
    bool literal;
};

struct Expanded
{
    size_t order;
    const Filter* filter;
    std::string path;
};

struct Outcome
{
    std::string message;
    std::string policy;
};

std::atomic<int> currentRuleset = -1;
std::atomic<int> answers;
std::atomic<int> failures;
std::atomic<int> answered[maximumThreads];
bool active;

bool inside(const std::string& path, const std::string& directory)
{
    if (directory == "/")
        return path.size() > 1 && path[0] == '/';
    return path.size() > directory.size() && path.compare(0, directory.size(), directory) == 0 && path[directory.size()] == '/';
}

uint64_t accessOf(const std::string& letters)
{
    if (letters == "R")
        return readAccess;
    if (letters == "W")
        return writeAccess;
    if (letters == "X")
        return executeAccess;
    if (letters == "IOC")
        return controlAccess;
    return 0;
}

inline std::string parentOf(const std::string& path)
{
    const size_t slash = path.rfind('/');
    if (slash == 0)
        return "/";
    return path.substr(0, slash);
}

std::string nameOf(const std::string& path)
{
    return path.substr(path.rfind('/') + 1);
}

// The path on the host: /private is not there.
std::string canonical(const std::string& given)
{
    std::string path = given;
    struct stat status;
    if (path.rfind("/private/", 0) == 0 && stat("/private", &status) != 0)
        path = path.substr(8);
    while (path.size() > 1 && path.back() == '/')
        path.pop_back();
    return hostPath(path.c_str());
}

std::set<int> threads()
{
    std::set<int> found;
    DIR* directory = opendir("/proc/self/task");
    if (directory) {
        while (const dirent* entry = readdir(directory)) {
            if (entry->d_name[0] != '.')
                found.insert(atoi(entry->d_name));
        }
        closedir(directory);
    }
    return found;
}

// Opens a path without following links.
int openPlain(const char* path, uint64_t flags, uint64_t mode)
{
    open_how how = {};
    how.flags = flags;
    how.mode = mode;
    how.resolve = RESOLVE_NO_SYMLINKS | RESOLVE_NO_MAGICLINKS;
    return int(syscall(SYS_openat2, AT_FDCWD, path, &how, sizeof how));
}

void restrictThread(int)
{
    const int saved = errno;
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0 || syscall(SYS_landlock_restrict_self, currentRuleset.load(), 0) != 0)
        failures++;
    const int index = answers++;
    if (index < maximumThreads)
        answered[index] = int(syscall(SYS_gettid));
    errno = saved;
}

} // namespace

int suspendSignal()
{
    return SIGRTMAX - 2;
}

void allowSandboxSignal()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGRTMAX - 2);
    static const auto real = reinterpret_cast<decltype(&pthread_sigmask)>(dlsym(RTLD_NEXT, "pthread_sigmask"));
    real(SIG_UNBLOCK, &set, nullptr);
}

} // namespace cutefish::reef

// The signal of the sandbox stays open, whatever masks the program sets.
static const sigset_t* withoutSandboxSignal(int how, const sigset_t* set, sigset_t& copy)
{
    if (!set || how == SIG_UNBLOCK || !sigismember(set, SIGRTMAX - 2))
        return set;
    copy = *set;
    sigdelset(&copy, SIGRTMAX - 2);
    return &copy;
}

extern "C" int pthread_sigmask(int how, const sigset_t* set, sigset_t* old) noexcept
{
    static const auto real = reinterpret_cast<decltype(&pthread_sigmask)>(dlsym(RTLD_NEXT, "pthread_sigmask"));
    sigset_t copy;
    return real(how, withoutSandboxSignal(how, set, copy), old);
}

extern "C" int sigprocmask(int how, const sigset_t* set, sigset_t* old) noexcept
{
    static const auto real = reinterpret_cast<decltype(&sigprocmask)>(dlsym(RTLD_NEXT, "sigprocmask"));
    sigset_t copy;
    return real(how, withoutSandboxSignal(how, set, copy), old);
}

namespace cutefish::reef {

void SandboxPolicy::release(uint64_t id)
{
    extensions.erase(std::remove_if(extensions.begin(), extensions.end(), [id](const SandboxExtension& extension) { return extension.id == id; }),
                     extensions.end());
}

bool SandboxPolicy::allows(const std::string& path, uint32_t access) const
{
    for (const SandboxLayer& layer : layers) {
        if (!(layer.files & access))
            continue;
        uint32_t given = 0;
        for (const SandboxExtension& extension : extensions) {
            const auto found = layer.extensions.find(extension.name);
            if (found != layer.extensions.end() && (path == extension.path || inside(path, extension.path)))
                given |= found->second;
        }
        for (const SandboxGrant& grant : layer.grants) {
            if (path != grant.path && !inside(path, grant.path))
                continue;
            bool refused = false;
            for (const SandboxHole& hole : grant.holes)
                refused |= path == hole.path || (!hole.literal && inside(path, hole.path));
            if (!refused)
                given |= grant.access;
        }
        if (layer.files & access & ~given)
            return false;
    }
    return true;
}

int SandboxPolicy::open(const std::string& path, int flags, int mode, int* fd) const
{
    uint32_t access = (flags & O_ACCMODE) == O_WRONLY ? SandboxWrite : (flags & O_ACCMODE) == O_RDWR ? SandboxRead | SandboxWrite : SandboxRead;
    // O_CREAT, O_TRUNC
    if (flags & 0x600)
        access |= SandboxWrite;
    if (!allows(path, access))
        return EPERM;
    const int host = hostOpenFlags(flags) | O_CLOEXEC;
    const int opened = openPlain(path.c_str(), host, host & (O_CREAT | O_TMPFILE) ? mode & 07777 : 0);
    if (opened < 0) {
        if (errno == ELOOP)
            return EPERM;
        return darwinError(errno);
    }
    *fd = opened;
    return 0;
}

// Landlock refuses with EACCES what the mode of a file allows: Darwin's sandbox refuses with EPERM.
int sandboxError(int error, int directory, const char* path, int access)
{
    if (error != EACCES || !active || !path)
        return error;
    const auto permits = [directory](const char* name, int wanted) {
        struct stat status;
        if (fstatat(directory, name, &status, 0) != 0)
            return false;
        const unsigned shift = status.st_uid == geteuid() ? 6 : status.st_gid == getegid() ? 3 : 0;
        const unsigned mode = status.st_mode >> shift;
        return (!(wanted & 4) || (mode & 4)) && (!(wanted & 2) || (mode & 2)) && (!(wanted & 1) || (mode & 1));
    };
    struct stat status;
    if (fstatat(directory, path, &status, 0) == 0)
        return permits(path, access) ? EPERM : error;
    std::string parent = path;
    const size_t slash = parent.rfind('/');
    parent = slash == std::string::npos ? "." : slash == 0 ? "/" : parent.substr(0, slash);
    return permits(parent.c_str(), 3) ? EPERM : error;
}

bool SandboxPolicy::mayIssue(const std::string& name) const
{
    for (const SandboxLayer& layer : layers) {
        if (!layer.unconfined && !layer.issued.count(name) && !layer.issued.count("*"))
            return false;
    }
    return true;
}

int SandboxPolicy::change(int operation, const std::string& path, const std::string& to, int mode) const
{
    if (path.size() < 2 || path[0] != '/' || (operation == 3 && (to.size() < 2 || to[0] != '/')))
        return EINVAL;
    if (!allows(path, SandboxWrite) || (operation == 3 && !allows(to, SandboxWrite)))
        return EPERM;
    const int directory = openPlain(parentOf(path).c_str(), O_PATH | O_DIRECTORY | O_CLOEXEC, 0);
    if (directory < 0)
        return errno == ELOOP ? EPERM : darwinError(errno);
    int result = -1;
    int error;
    if (operation == 1) {
        result = mkdirat(directory, nameOf(path).c_str(), mode & 07777);
    } else if (operation == 2) {
        result = unlinkat(directory, nameOf(path).c_str(), mode ? AT_REMOVEDIR : 0);
    } else if (operation == 3) {
        const int other = openPlain(parentOf(to).c_str(), O_PATH | O_DIRECTORY | O_CLOEXEC, 0);
        if (other >= 0) {
            result = renameat(directory, nameOf(path).c_str(), other, nameOf(to).c_str());
            error = errno;
            close(other);
            errno = error;
        }
    } else {
        errno = EINVAL;
    }
    error = errno;
    close(directory);
    if (result == 0)
        return 0;
    return error == ELOOP ? EPERM : darwinError(error);
}

uint64_t SandboxPolicy::consume(const std::string& name, const std::string& path)
{
    extensions.push_back({nextExtension, name, path});
    return nextExtension++;
}

bool findSandboxToken(const SandboxTokens& tokens, const std::string& token, std::string& name, std::string& path)
{
    const auto found = tokens.find(token);
    if (found == tokens.end())
        return false;
    name = found->second.name;
    path = found->second.path;
    return true;
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

// What every program needs to run.
static void allowSystem(const Ruleset& ruleset, const Profile& profile)
{
    ruleset.allow(Linker::current->root(), readAccess);
    for (const char* path : {"/usr/lib", "/usr/lib64", "/lib", "/lib64", "/usr/local/lib", "/usr/share", "/etc", "/var/cache/fontconfig", "/proc/cpuinfo",
                             "/proc/meminfo", "/proc/stat", "/proc/sys/kernel", "/proc/sys/vm", "/sys/devices/system/cpu", "/dev/zero", "/dev/urandom",
                             "/dev/random"})
        ruleset.allow(path, readAccess);
    char executable[PATH_MAX];
    const ssize_t length = readlink("/proc/self/exe", executable, sizeof executable - 1);
    if (length > 0) {
        executable[length] = 0;
        ruleset.allow(executable, readAccess);
    }
    ruleset.allow("/proc/" + std::to_string(getpid()), readAccess);
    ruleset.allow("/dev/null", openAccess | LANDLOCK_ACCESS_FS_TRUNCATE);
    if (const passwd* user = getpwuid(getuid())) {
        const std::string home = user->pw_dir;
        for (const char* path : {"/.local/share/fonts", "/.fonts", "/.config/fontconfig", "/.cache/fontconfig"})
            ruleset.allow(home + path, readAccess);
        if (profile.audio)
            ruleset.allow(home + "/.config/pulse", readAccess);
    }
    if (profile.graphics) {
        ruleset.allow("/dev/dri", openAccess | controlAccess);
        for (const char* path : {"/sys/devices", "/sys/dev", "/sys/bus/pci"})
            ruleset.allow(path, readAccess);
        const std::string home = homeFolder();
        if (!home.empty()) {
            const std::string cache = home + "/.cache";
            mkdir(cache.c_str(), 0700);
            std::string directory = cache;
            directory += "/cutefish-reef";
            mkdir(directory.c_str(), 0700);
            directory += "/metal";
            mkdir(directory.c_str(), 0700);
            ruleset.allow(directory, readAccess | writeAccess);
            for (const char* name : {"/mesa_shader_cache_db", "/mesa_shader_cache"}) {
                const std::string path = cache + name;
                mkdir(path.c_str(), 0700);
                ruleset.allow(path, readAccess | writeAccess);
            }
        }
    }
    if (profile.audio)
        ruleset.allow("/dev/snd", openAccess | controlAccess);
}

// A rule of Landlock covers all that is inside: with holes, what is around them gets its own rules.
static void allowAround(const Ruleset& ruleset, const std::string& path, uint64_t access, const std::vector<Hole>& holes)
{
    bool split = false;
    for (const Hole& hole : holes) {
        if (hole.path == path) {
            if (!hole.literal)
                return;
            split = true;
        } else {
            split |= inside(hole.path, path);
        }
    }
    if (!split) {
        ruleset.allow(path, access);
        return;
    }
    DIR* directory = opendir(path.c_str());
    if (!directory)
        return;
    while (const dirent* entry = readdir(directory)) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        const std::string child = (path == "/" ? "" : path) + "/" + entry->d_name;
        struct stat status;
        if (lstat(child.c_str(), &status) != 0 || S_ISLNK(status.st_mode))
            continue;
        std::vector<Hole> inner;
        for (const Hole& hole : holes) {
            if (hole.path == child || inside(hole.path, child))
                inner.push_back(hole);
        }
        allowAround(ruleset, child, access, inner);
    }
    closedir(directory);
}

std::string issueSandboxToken(SandboxTokens& tokens, const std::string& name, const std::string& path)
{
    unsigned char bytes[16];
    if (getrandom(bytes, sizeof bytes, 0) != sizeof bytes)
        return {};
    std::string token;
    for (const unsigned char byte : bytes) {
        char text[3];
        snprintf(text, sizeof text, "%02x", byte);
        token += text;
    }
    token += ";" + name + ";" + path;
    tokens[token] = {name, path};
    return token;
}

bool SandboxPolicy::mayLookUp(const std::string& name) const
{
    for (const SandboxLayer& layer : layers) {
        if (layer.unconfined)
            continue;
        bool known = layer.names.count(name);
        for (size_t i = 0; !known && i < layer.regexes.size(); i++)
            known = std::regex_search(name, layer.regexes[i]);
        if (!known)
            return false;
    }
    return true;
}

// What the fixed start of a regular expression says: where to search what it matches.
static std::string fixedStart(const std::string& expression)
{
    std::string start;
    for (size_t i = !expression.empty() && expression[0] == '^'; i < expression.size(); i++) {
        const char c = expression[i];
        if (c == '\\' && i + 1 < expression.size() && !isalnum(static_cast<unsigned char>(expression[i + 1]))) {
            start.push_back(expression[++i]);
            continue;
        }
        if (strchr(".[]()*+?{}|^$\\", c))
            break;
        start += c;
    }
    return start;
}

static std::vector<std::string> matching(const std::string& text)
{
    const std::string start = fixedStart(text);
    const size_t slash = start.rfind('/');
    if (slash == std::string::npos || start[0] != '/')
        return {};
    const std::string base = slash ? start.substr(0, slash) : "/";
    std::regex expression;
    try {
        expression = std::regex(text, std::regex::extended | std::regex::nosubs);
    } catch (const std::regex_error&) {
        return {};
    }
    std::vector<std::pair<std::string, int>> pending = {{base, 0}};
    std::vector<std::string> found;
    size_t seen = 0;
    while (!pending.empty() && seen < 5000) {
        auto [directory, depth] = pending.back();
        pending.pop_back();
        const std::string host = canonical(directory);
        DIR* stream = opendir(host.c_str());
        if (!stream)
            continue;
        while (const dirent* entry = readdir(stream)) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || ++seen > 5000)
                continue;
            const std::string path = (directory == "/" ? "" : directory) + "/" + entry->d_name;
            if (std::regex_search(path, expression))
                found.push_back(host + "/" + entry->d_name);
            else if (entry->d_type == DT_DIR && depth < 8)
                pending.push_back({path, depth + 1});
        }
        closedir(stream);
    }
    return found;
}

// The paths of the host that a filter means.
static std::vector<std::string> pathsOf(const Filter& filter)
{
    std::vector<std::string> found;
    if (filter.kind == "extension")
        return found;
    if (filter.kind == "regex")
        return matching(filter.path);
    if (filter.kind == "prefix") {
        const size_t slash = filter.path.rfind('/');
        if (slash == std::string::npos)
            return found;
        const std::string directory = slash ? filter.path.substr(0, slash) : "/";
        const std::string name = filter.path.substr(slash + 1);
        const std::string host = canonical(directory);
        DIR* stream = opendir(host.c_str());
        if (!stream)
            return found;
        while (const dirent* entry = readdir(stream)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strncmp(entry->d_name, name.c_str(), name.size()) == 0)
                found.push_back(host + "/" + entry->d_name);
        }
        closedir(stream);
        return found;
    }
    found.push_back(canonical(filter.path));
    return found;
}

// The rules of the filters: a later refusal takes a part of what an earlier filter allows.
static void addFilters(const Ruleset& ruleset, const std::vector<Filter>& filters, std::string& policy)
{
    std::vector<Expanded> expanded;
    for (size_t i = 0; i < filters.size(); i++) {
        for (const std::string& path : pathsOf(filters[i])) {
            char real[PATH_MAX];
            if (realpath(path.c_str(), real))
                expanded.push_back({i, &filters[i], real});
        }
    }
    for (const Expanded& allowed : expanded) {
        if (allowed.filter->deny)
            continue;
        std::vector<Hole> holes;
        bool covered = false;
        for (const Expanded& refused : expanded) {
            if (!refused.filter->deny || refused.order < allowed.order || refused.filter->access != allowed.filter->access)
                continue;
            if (inside(allowed.path, refused.path)) {
                covered = true;
                continue;
            }
            const bool literal = refused.filter->kind == "literal";
            if (refused.path == allowed.path) {
                if (!literal) {
                    covered = true;
                    continue;
                }
            } else if (!inside(refused.path, allowed.path)) {
                continue;
            }
            holes.push_back({refused.path, literal});
        }
        if (covered)
            continue;
        allowAround(ruleset, allowed.path, allowed.filter->access, holes);
        char letter;
        if (allowed.filter->access == readAccess)
            letter = 'R';
        else if (allowed.filter->access == writeAccess)
            letter = 'W';
        else
            continue;
        if (holes.empty())
            continue;
        policy += std::string("grant ") + letter + " " + allowed.path + "\n";
        for (const Hole& hole : holes)
            policy += std::string("hole ") + (hole.literal ? "l " : "s ") + hole.path + "\n";
    }
}

static void addFilter(Profile& profile, const std::string& line)
{
    const size_t first = line.find(' ');
    if (first == std::string::npos)
        return;
    std::string letters(line, 0, first);
    const size_t second = line.find(' ', first + 1);
    if (second == std::string::npos)
        return;
    const std::string kind = line.substr(first + 1, second - first - 1);
    const std::string path = line.substr(second + 1);
    const bool deny = letters[0] == '-';
    if (deny)
        letters.erase(0, 1);
    const uint64_t access = accessOf(letters);
    if (!access) {
        if (letters == "I") {
            for (const char* name : {"IOAccelerator", "IOAccelerationUserClient", "AGX", "AGPMClient"})
                profile.graphics |= path.find(name) != std::string::npos;
        }
        if (letters == "M" && path.rfind("com.apple.audio.", 0) == 0)
            profile.audio = true;
        return;
    }
    profile.filters.push_back({access, deny, kind, path});
    if (deny)
        profile.restricted |= access;
}

static Profile parseProfile(const char* text, size_t size)
{
    Profile profile;
    std::istringstream stream(std::string(text, size));
    std::string line;
    while (std::getline(stream, line)) {
        if (line == "deny-default") {
            profile.denyDefault = true;
        } else if (line == "allow-default") {
            for (const uint64_t access : {readAccess, writeAccess, executeAccess, controlAccess})
                profile.filters.insert(profile.filters.begin(), {access, false, "subpath", "/"});
        } else if (line == "N out") {
            profile.allowOutbound = true;
        } else if (line == "N in") {
            profile.allowInbound = true;
        } else if (line.rfind("deny-all ", 0) == 0) {
            const std::string what = line.substr(9);
            const uint64_t access = accessOf(what);
            profile.denied |= access;
            if (access)
                profile.filters.push_back({access, true, "subpath", "/"});
            profile.confineOutbound |= what == "N out";
            profile.confineInbound |= what == "N in";
        } else {
            addFilter(profile, line);
        }
    }
    if (profile.denyDefault) {
        profile.confineOutbound = !profile.allowOutbound;
        profile.confineInbound = !profile.allowInbound;
    }
    return profile;
}

static std::string threadName(int thread)
{
    std::string name;
    char path[64];
    snprintf(path, sizeof path, "/proc/self/task/%d/comm", thread);
    if (FILE* file = fopen(path, "r")) {
        char text[64] = {};
        if (fgets(text, sizeof text, file))
            name = std::string(text, strcspn(text, "\n"));
        fclose(file);
    }
    return "thread " + std::to_string(thread) + " (" + name + ")";
}

// Landlock confines a thread and those it starts: the others get a signal, and confine themselves in its handler.
static int confineThreads(int ruleset, Outcome& outcome)
{
    const int signal = SIGRTMAX - 2;
    struct sigaction action = {};
    struct sigaction old = {};
    action.sa_handler = restrictThread;
    action.sa_flags = SA_RESTART;
    sigemptyset(&action.sa_mask);
    sigaction(signal, &action, &old);
    currentRuleset = ruleset;
    std::set<int> done = {int(syscall(SYS_gettid))};
    int result = 0;
    for (int round = 0; round < 50 && !result; round++) {
        answers = 0;
        std::vector<int> signalled;
        for (const int thread : threads()) {
            if (done.count(thread))
                continue;
            if (syscall(SYS_tgkill, getpid(), thread, signal) == 0)
                signalled.push_back(thread);
            done.insert(thread);
        }
        if (signalled.empty())
            break;
        for (int wait = 0; answers < int(signalled.size()); wait++) {
            const timespec pause = {0, 1000000};
            if (wait % 50 != 49) {
                nanosleep(&pause, nullptr);
                continue;
            }
            // Those that ended in the meantime do not answer.
            const std::set<int> alive = threads();
            int gone = 0;
            for (const int thread : signalled)
                gone += alive.find(thread) == alive.end();
            if (answers + gone >= int(signalled.size()))
                break;
            if (wait < 3000) {
                nanosleep(&pause, nullptr);
                continue;
            }
            std::set<int> confined;
            for (int i = 0; i < std::min(int(answers), maximumThreads); i++)
                confined.insert(answered[i]);
            outcome.message = "a thread did not take the sandbox";
            for (const int thread : signalled) {
                if (!confined.count(thread) && alive.count(thread))
                    outcome.message += ": " + threadName(thread);
            }
            result = -1;
            break;
        }
    }
    if (result) {
    } else if (failures) {
        outcome.message = "a thread could not take the sandbox";
        result = -1;
    } else if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0 || syscall(SYS_landlock_restrict_self, ruleset, 0) != 0) {
        outcome.message = std::string("landlock_restrict_self: ") + strerror(errno);
        result = -errno;
    }
    sigaction(signal, &old, nullptr);
    return result;
}

// What no sandboxed program calls: tracing, mounts, modules, namespaces, keys; and sockets when it has no network.
static int filterCalls(const Profile& profile, Outcome& outcome)
{
    std::vector<sock_filter> program;
    const auto refuse = [&program](uint32_t call, int error = EPERM) {
        program.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, call, 0, 1));
        program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | uint32_t(error)));
    };
    program.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 4));
    program.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0));
    program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS));
    program.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 0));
    program.push_back(BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, 0x40000000, 0, 1));
    program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | EPERM));
    refuse(SYS_iopl);
    refuse(SYS_ioperm);
    // A program may read its own memory.
    program.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_process_vm_readv, 0, 4));
    program.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 16));
    program.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, uint32_t(getpid()), 0, 1));
    program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW));
    program.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 0));
    for (const uint32_t call :
         {SYS_ptrace,          SYS_process_vm_readv,   SYS_process_vm_writev, SYS_perf_event_open, SYS_bpf,           SYS_userfaultfd,
          SYS_keyctl,          SYS_add_key,            SYS_request_key,       SYS_unshare,         SYS_setns,         SYS_mount,
          SYS_umount2,         SYS_pivot_root,         SYS_chroot,            SYS_kexec_load,      SYS_kexec_file_load, SYS_init_module,
          SYS_finit_module,    SYS_delete_module,      SYS_swapon,            SYS_swapoff,         SYS_reboot,        SYS_acct,
          SYS_quotactl,        SYS_open_by_handle_at,  SYS_name_to_handle_at, SYS_io_uring_setup,  SYS_io_uring_enter, SYS_io_uring_register,
          SYS_fanotify_init,   SYS_syslog,             SYS_vhangup,           SYS_fsopen,          SYS_fsmount,       SYS_fspick,
          SYS_move_mount,      SYS_open_tree,          SYS_mount_setattr})
        refuse(call);
    refuse(SYS_clone3, ENOSYS);
    program.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_clone, 0, 4));
    program.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 16));
    program.push_back(BPF_JUMP(BPF_JMP | BPF_JSET | BPF_K, 0x7e020000, 0, 1));
    program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | EPERM));
    program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW));
    if (profile.confineOutbound && profile.confineInbound) {
        if (profile.audio) {
            program.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_socket, 0, 4));
            program.push_back(BPF_STMT(BPF_LD | BPF_W | BPF_ABS, 16));
            program.push_back(BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 1, 0, 1));
            program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW));
            program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | EPERM));
        } else {
            refuse(SYS_socket);
        }
    }
    program.push_back(BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW));
    sock_fprog filter;
    filter.len = static_cast<unsigned short>(program.size());
    filter.filter = program.data();
    if (syscall(SYS_seccomp, SECCOMP_SET_MODE_FILTER, SECCOMP_FILTER_FLAG_TSYNC, &filter) != 0) {
        outcome.message = std::string("seccomp: ") + strerror(errno);
        return -errno;
    }
    return 0;
}

static int confineFiles(const Profile& profile, uint64_t handled, Outcome& outcome)
{
    const int version = int(syscall(SYS_landlock_create_ruleset, nullptr, 0, LANDLOCK_CREATE_RULESET_VERSION));
    if (version < 1) {
        fprintf(stderr, "reef: the host has no Landlock; the sandbox confines sockets and calls only\n");
        return 0;
    }
    RulesetAttribute attribute = {};
    attribute.files = handled & 0xffff;
    if (version < 2)
        attribute.files &= ~uint64_t(LANDLOCK_ACCESS_FS_REFER);
    if (version < 3)
        attribute.files &= ~uint64_t(LANDLOCK_ACCESS_FS_TRUNCATE);
    if (version < 5)
        attribute.files &= ~controlAccess;
    if (version >= 4) {
        if (profile.confineOutbound)
            attribute.network |= 2;
        if (profile.confineInbound)
            attribute.network |= 1;
    }
    if (version >= 6 && profile.denyDefault)
        attribute.scoped = 3;
    if (!attribute.files && !attribute.network && !attribute.scoped)
        return 0;
    const size_t size = version < 4 ? 8 : version < 6 ? 16 : 24;
    const int fd = int(syscall(SYS_landlock_create_ruleset, &attribute, size, 0));
    if (fd < 0) {
        outcome.message = std::string("landlock_create_ruleset: ") + strerror(errno);
        return -errno;
    }
    const Ruleset ruleset = {fd, attribute.files};
    addFilters(ruleset, profile.filters, outcome.policy);
    outcome.policy += std::string("files ") + (attribute.files & readAccess ? "R" : "") + (attribute.files & writeAccess ? "W" : "") + "\n";
    allowSystem(ruleset, profile);
    const int result = confineThreads(fd, outcome);
    close(fd);
    return result;
}

static int confine(const char* text, size_t size, Outcome& outcome)
{
    const Profile profile = parseProfile(text, size);
    uint64_t handled = ~uint64_t(0);
    if (!profile.denyDefault) {
        handled = profile.denied | profile.restricted;
        if (!handled && !profile.confineOutbound && !profile.confineInbound)
            return 0;
    }
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0) {
        outcome.message = std::string("no_new_privs: ") + strerror(errno);
        return -errno;
    }
    int result = confineFiles(profile, handled, outcome);
    if (result)
        return result;
    result = filterCalls(profile, outcome);
    if (result)
        return result;
    active = true;
    return 0;
}

static int hostSandbox(const char* profile, size_t size, char* message, size_t messageSize, char* policy, size_t policySize)
{
    Outcome outcome;
    const int result = confine(profile, size, outcome);
    if (message && messageSize)
        snprintf(message, messageSize, "%s", outcome.message.c_str());
    if (policy) {
        if (policySize)
            *policy = 0;
        if (outcome.policy.size() < policySize)
            memcpy(policy, outcome.policy.c_str(), outcome.policy.size() + 1);
    }
    return result;
}

void SandboxPolicy::add(const std::string& description)
{
    SandboxLayer layer;
    std::istringstream stream(description);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.rfind("name ", 0) == 0) {
            layer.names.insert(line.substr(5));
        } else if (line.rfind("regex ", 0) == 0) {
            try {
                layer.regexes.emplace_back(line.substr(6), std::regex::extended | std::regex::nosubs);
            } catch (const std::regex_error&) {
            }
        } else if (line.rfind("extension ", 0) == 0 && line.size() > 12 && line[11] == ' ') {
            const uint32_t access = line[10] == 'R' ? SandboxRead : line[10] == 'W' ? SandboxWrite : 0;
            layer.extensions[line.substr(12)] |= access;
        } else if (line.rfind("issue ", 0) == 0) {
            layer.issued.insert(line.substr(6));
        } else if (line == "unconfined") {
            layer.unconfined = true;
        } else if (line.rfind("files ", 0) == 0) {
            layer.files = (line.find('R', 6) != std::string::npos ? SandboxRead : 0) | (line.find('W', 6) != std::string::npos ? SandboxWrite : 0);
        } else if (line.rfind("grant ", 0) == 0 && line.size() > 8) {
            layer.grants.push_back({line[6] == 'R' ? SandboxRead : SandboxWrite, line.substr(8), {}});
        } else if (line.rfind("hole ", 0) == 0 && line.size() > 7 && !layer.grants.empty()) {
            layer.grants.back().holes.push_back({line[5] == 'l', line.substr(7)});
        }
    }
    layers.push_back(std::move(layer));
}

void addSandboxServices(HostServices& services)
{
    services.sandbox = hostSandbox;
}

} // namespace cutefish::reef
