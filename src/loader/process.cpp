// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Processes: fork, exec and posix_spawn (Mac programs are started through the loader), and waiting for them.

#include "host.h"
#include "macho.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include <dirent.h>
#include <fcntl.h>
#include <linux/close_range.h>
#include <pwd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

namespace cutefish::reef {

struct FileAction
{
    enum Kind { Open, Close, Duplicate, Inherit, ChangeDirectory, ChangeDirectoryTo };

    int kind;
    int fd;
    int newFd;
    int flags;
    uint32_t mode;
    const char* path;
};

struct SpawnAttributes
{
    enum Flags {
        ResetIds = 0x1,
        SetGroup = 0x2,
        SetDefault = 0x4,
        SetMask = 0x8,
        SetExec = 0x40,
        StartSuspended = 0x80,
        SetSession = 0x400,
        CloseByDefault = 0x4000,
    };

    uint32_t flags;
    int group;
    uint32_t mask;
    uint32_t defaults;
    uint64_t unknown10[2];
};

struct WaitResult
{
    int process;
    uint32_t user;
    int code;
    int status;
};

namespace {

// What is executed for a path: the program itself, or the loader for a Mac program.
struct Program
{
    std::string path;
    std::vector<std::string> arguments;
    std::vector<char*> argumentPointers;
    std::vector<char*> environment;
    std::vector<std::string> environmentStrings;
};

// What the child does before it becomes the program.
struct ChildSetup
{
    const FileAction* actions = nullptr;
    uint32_t actionCount = 0;
    const std::vector<std::string>* paths = nullptr;
    const std::vector<int>* kept = nullptr;
    SpawnAttributes attributes = {};
    sigset_t mask = {};
    int errorPipe = -1;
};

} // namespace

static int hostErrno()
{
    return darwinError(errno);
}

static void hostSetErrno(int error)
{
    errno = linuxError(error);
}

static int hostWaitid(int type, uint32_t id, int options, WaitResult* result)
{
    int host = options & (WNOHANG | WEXITED);
    if (options & 0x8)
        host |= WSTOPPED;
    if (options & 0x10)
        host |= WCONTINUED;
    if (options & 0x20)
        host |= WNOWAIT;
    siginfo_t information = {};
    if (waitid(idtype_t(type), id, &information, host) < 0)
        return -darwinError(errno);
    result->process = information.si_pid;
    result->user = information.si_uid;
    result->code = information.si_code;
    result->status = information.si_code == CLD_EXITED ? information.si_status : darwinSignal(information.si_status);
    return 0;
}

static int hostWait4(int process, int* status, int options, rusage* usage)
{
    int host = options & (WNOHANG | WUNTRACED);
    if (options & 0x10)
        host |= WCONTINUED;
    int hostStatus;
    rusage hostUsage = {};
    const pid_t found = wait4(process, &hostStatus, host, usage ? &hostUsage : nullptr);
    if (found < 0)
        return -darwinError(errno);
    if (status && found) {
        if (WIFSIGNALED(hostStatus))
            hostStatus = (hostStatus & ~0x7f) | darwinSignal(WTERMSIG(hostStatus));
        else if (WIFSTOPPED(hostStatus))
            hostStatus = W_STOPCODE(darwinSignal(WSTOPSIG(hostStatus)));
        else if (WIFCONTINUED(hostStatus))
            hostStatus = 0x137f;
        *status = hostStatus;
    }
    if (usage)
        *usage = hostUsage;
    return found;
}

// In the child: 0, or the error of what could not be done.
static int setUpChild(const ChildSetup& setup)
{
    const uint32_t flags = setup.attributes.flags;
    if ((flags & SpawnAttributes::SetSession) && setsid() < 0)
        return errno;
    if ((flags & SpawnAttributes::SetGroup) && setpgid(0, setup.attributes.group) < 0)
        return errno;
    if (flags & SpawnAttributes::ResetIds) {
        if (setgid(getgid()) < 0 || setuid(getuid()) < 0)
            return errno;
    }
    for (uint32_t i = 0; i < setup.actionCount; i++) {
        const FileAction& action = setup.actions[i];
        switch (action.kind) {
        case FileAction::Open: {
            const int fd = open((*setup.paths)[i].c_str(), hostOpenFlags(action.flags), action.mode);
            if (fd < 0)
                return errno;
            if (fd != action.fd) {
                if (dup2(fd, action.fd) < 0)
                    return errno;
                close(fd);
            }
            break;
        }
        case FileAction::Close:
            close(action.fd);
            break;
        case FileAction::Duplicate:
            if (action.fd != action.newFd) {
                if (dup2(action.fd, action.newFd) < 0)
                    return errno;
                break;
            }
            [[fallthrough]];
        case FileAction::Inherit: {
            const int current = fcntl(action.fd, F_GETFD);
            if (current < 0 || fcntl(action.fd, F_SETFD, current & ~FD_CLOEXEC) < 0)
                return errno;
            break;
        }
        case FileAction::ChangeDirectory:
            if (chdir((*setup.paths)[i].c_str()) < 0)
                return errno;
            break;
        case FileAction::ChangeDirectoryTo:
            if (fchdir(action.fd) < 0)
                return errno;
            break;
        }
    }
    if (flags & SpawnAttributes::CloseByDefault) {
        unsigned next = 0;
        for (const unsigned fd : *setup.kept) {
            if (fd > next)
                syscall(SYS_close_range, next, fd - 1, CLOSE_RANGE_CLOEXEC);
            next = fd + 1;
        }
        syscall(SYS_close_range, next, UINT_MAX, CLOSE_RANGE_CLOEXEC);
    }
    if (flags & SpawnAttributes::SetDefault) {
        sigset_t defaults;
        sigemptyset(&defaults);
        addDarwinSignals(setup.attributes.defaults, &defaults);
        struct sigaction action = {};
        for (int signal = 1; signal < 65; signal++) {
            if (sigismember(&defaults, signal) == 1)
                sigaction(signal, &action, nullptr);
        }
    }
    if (flags & SpawnAttributes::StartSuspended)
        kill(getpid(), SIGSTOP);
    pthread_sigmask(SIG_SETMASK, &setup.mask, nullptr);
    return 0;
}

[[noreturn]] static void failChild(int pipe, int error)
{
    const int darwin = darwinError(error);
    while (write(pipe, &darwin, sizeof darwin) < 0 && errno == EINTR) {
    }
    _exit(127);
}

// Whether a child has exited, without reaping it: 1 and its status, or 0.
static int hostWaitExited(int process, int* status)
{
    siginfo_t information = {};
    if (waitid(P_PID, id_t(process), &information, WEXITED | WNOHANG | WNOWAIT) < 0)
        return -darwinError(errno);
    if (information.si_pid == 0)
        return 0;
    if (information.si_code == CLD_EXITED)
        *status = (information.si_status & 0xff) << 8;
    else
        *status = darwinSignal(information.si_status) | (information.si_code == CLD_DUMPED ? 0x80 : 0);
    return 1;
}

static int hostFork()
{
    const pid_t process = fork();
    if (process == 0) {
        renewTask();
        return 0;
    }
    if (process < 0)
        return -darwinError(errno);
    return process;
}

static int hostSetsid()
{
    const pid_t session = setsid();
    if (session < 0)
        return -darwinError(errno);
    return session;
}

static int hostSetpgid(int process, int group)
{
    if (setpgid(process, group) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostPidfdOpen(int process, unsigned flags)
{
    const long fd = syscall(SYS_pidfd_open, process, flags);
    if (fd < 0)
        return -darwinError(errno);
    return int(fd);
}

static std::string selfPath()
{
    char self[PATH_MAX];
    const ssize_t length = readlink("/proc/self/exe", self, sizeof self - 1);
    if (length <= 0)
        return "/proc/self/exe";
    return std::string(self, size_t(length));
}

// The loader, whatever the process does later.
static const std::string& reefPath()
{
    static const std::string path = selfPath();
    return path;
}

static std::string homeFolder()
{
    const char* home = secure_getenv("CFFIXED_USER_HOME");
    if (!home || *home != '/') {
        const passwd* user = getpwuid(getuid());
        if (!user || !user->pw_dir || !*user->pw_dir)
            return {};
        home = user->pw_dir;
    }
    return home;
}

// A terminfo directory of Darwin for the programs of the host: its folders have hexadecimal names, theirs the letters.
static std::string hostTerminfo(const std::string& directory)
{
    DIR* stream = opendir(directory.c_str());
    if (!stream)
        return {};
    std::vector<std::pair<char, std::string>> letters;
    while (const dirent* entry = readdir(stream)) {
        const char* name = entry->d_name;
        if (strlen(name) != 2 || !isxdigit(static_cast<unsigned char>(name[0])) || !isxdigit(static_cast<unsigned char>(name[1])))
            continue;
        const long code = strtol(name, nullptr, 16);
        if (code > ' ' && code < 0x7f && code != '.' && code != '/')
            letters.emplace_back(char(code), directory + "/" + name);
    }
    closedir(stream);
    const std::string home = homeFolder();
    if (letters.empty() || home.empty())
        return {};
    char hash[17];
    snprintf(hash, sizeof hash, "%016zx", std::hash<std::string>()(directory));
    std::string path = home;
    for (const char* part : {".cache", "cutefish-reef", "terminfo", static_cast<const char*>(hash)}) {
        path += std::string("/") + part;
        if (mkdir(path.c_str(), 0700) != 0 && errno != EEXIST)
            return {};
    }
    for (const auto& [letter, target] : letters) {
        const std::string link = path + "/" + letter;
        char current[PATH_MAX];
        const ssize_t length = readlink(link.c_str(), current, sizeof current - 1);
        if (length >= 0 && std::string_view(current, size_t(length)) == target)
            continue;
        if (length >= 0)
            unlink(link.c_str());
        if (symlink(target.c_str(), link.c_str()) != 0 && errno != EEXIST)
            return {};
    }
    return path;
}

static Program prepare(const char* path, char* const* argv, char* const* environment, const char* exceptionPorts)
{
    Program program;
    const std::string file = hostPath(path, true);
    bool mac = false;
    const int fd = open(file.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd >= 0) {
        uint32_t magic = 0;
        const ssize_t size = read(fd, &magic, sizeof magic);
        close(fd);
        mac = size == sizeof magic && (magic == MH_MAGIC || magic == MH_MAGIC_64 || magic == FAT_CIGAM || magic == FAT_CIGAM_64);
    }
    if (mac) {
        program.path = reefPath();
        program.arguments.push_back(reefPath());
        if (argv && argv[0] && strcmp(argv[0], path) != 0)
            program.arguments.push_back("--argv0=" + std::string(argv[0]));
        if (exceptionPorts)
            program.arguments.push_back("--exception-ports=" + std::string(exceptionPorts));
        program.arguments.push_back(file);
        if (argv) {
            for (char* const* argument = argv + (argv[0] != nullptr); *argument; argument++)
                program.arguments.push_back(*argument);
        }
    } else {
        program.path = file;
        if (argv) {
            for (; *argv; argv++)
                program.arguments.push_back(*argv);
        }
    }
    for (std::string& argument : program.arguments)
        program.argumentPointers.push_back(argument.data());
    program.argumentPointers.push_back(nullptr);
    if (environment) {
        const char* terminfo = nullptr;
        size_t directories = std::string::npos;
        for (; *environment; environment++) {
            if (strncmp(*environment, "TERMINFO=", 9) == 0)
                terminfo = *environment + 9;
            else if (strncmp(*environment, "TERMINFO_DIRS=", 14) == 0)
                directories = program.environment.size();
            program.environment.push_back(*environment);
        }
        // The terminfo of a terminal of the Mac, for a program of the host.
        if (terminfo && *terminfo == '/' && !mac) {
            const std::string converted = hostTerminfo(terminfo);
            const char* old = directories == std::string::npos ? "" : program.environment[directories] + 14;
            if (!converted.empty() && strncmp(old, converted.c_str(), converted.size()) != 0) {
                if (directories != std::string::npos) {
                    program.environmentStrings.push_back("TERMINFO_DIRS=" + converted + ":" + old);
                    program.environment[directories] = program.environmentStrings.back().data();
                } else {
                    program.environmentStrings.push_back("TERMINFO_DIRS=" + converted);
                    program.environment.push_back(program.environmentStrings.back().data());
                }
            }
        }
    }
    program.environment.push_back(nullptr);
    return program;
}

static int hostExec(const char* path, char* const* argv, char* const* environment)
{
    const Program program = prepare(path, argv, environment, nullptr);
    execve(program.path.c_str(), program.argumentPointers.data(), program.environment.data());
    return -darwinError(errno);
}

static int hostSpawn(int* process, const char* path, char* const* argv, char* const* environment, const FileAction* actions, uint32_t actionCount,
                     const SpawnAttributes* attributes, const char* exceptionPorts)
{
    const Program program = prepare(path, argv, environment, exceptionPorts);
    std::vector<std::string> paths(actionCount);
    std::vector<int> kept;
    for (uint32_t i = 0; i < actionCount; i++) {
        if (actions[i].path)
            paths[i] = hostPath(actions[i].path, true);
        if (actions[i].kind == FileAction::Open || actions[i].kind == FileAction::Inherit)
            kept.push_back(actions[i].fd);
        else if (actions[i].kind == FileAction::Duplicate)
            kept.push_back(actions[i].newFd);
    }
    std::sort(kept.begin(), kept.end());
    ChildSetup setup;
    setup.actions = actions;
    setup.actionCount = actionCount;
    setup.paths = &paths;
    setup.kept = &kept;
    if (attributes)
        setup.attributes = *attributes;
    sigset_t all;
    sigset_t old;
    sigfillset(&all);
    pthread_sigmask(SIG_SETMASK, &all, &old);
    setup.mask = old;
    if (setup.attributes.flags & SpawnAttributes::SetMask) {
        sigemptyset(&setup.mask);
        addDarwinSignals(setup.attributes.mask, &setup.mask);
    }
    if (setup.attributes.flags & SpawnAttributes::SetExec) {
        int error = setUpChild(setup);
        if (error == 0) {
            execve(program.path.c_str(), program.argumentPointers.data(), program.environment.data());
            error = errno;
        }
        error = -darwinError(error);
        pthread_sigmask(SIG_SETMASK, &old, nullptr);
        return error;
    }
    int ends[2];
    if (pipe2(ends, O_CLOEXEC) < 0) {
        const int error = -darwinError(errno);
        pthread_sigmask(SIG_SETMASK, &old, nullptr);
        return error;
    }
    setup.errorPipe = ends[1];
    const pid_t child = fork();
    if (child == 0) {
        close(ends[0]);
        int error = setUpChild(setup);
        if (error == 0) {
            execve(program.path.c_str(), program.argumentPointers.data(), program.environment.data());
            error = errno;
        }
        failChild(ends[1], error);
    }
    const int error = errno;
    close(ends[1]);
    pthread_sigmask(SIG_SETMASK, &old, nullptr);
    if (child < 0) {
        close(ends[0]);
        return -darwinError(error);
    }
    int childError = 0;
    ssize_t size;
    do {
        size = read(ends[0], &childError, sizeof childError);
    } while (size < 0 && errno == EINTR);
    close(ends[0]);
    if (size == sizeof childError) {
        while (waitpid(child, nullptr, 0) < 0 && errno == EINTR) {
        }
        return -childError;
    }
    if (process)
        *process = child;
    return 0;
}

void addProcessServices(HostServices& services)
{
    // Read before a sandbox hides it.
    reefPath();
    services.spawn = hostSpawn;
    services.exec = hostExec;
    services.fork = hostFork;
    services.wait4 = hostWait4;
    services.setsid = hostSetsid;
    services.setpgid = hostSetpgid;
    services.pidfdOpen = hostPidfdOpen;
    services.waitExited = hostWaitExited;
    services.reefPath = reefPath().c_str();
    services.error = hostErrno;
    services.setError = hostSetErrno;
    services.darwinError = darwinError;
    services.waitid = hostWaitid;
}

} // namespace cutefish::reef
