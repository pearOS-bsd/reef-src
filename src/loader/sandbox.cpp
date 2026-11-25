// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Sandboxes of Mac programs, with Landlock and seccomp.

#include "host.h"

#include <cerrno>
#include <csignal>
#include <dlfcn.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace cutefish::reef {

static bool active;

static int sandboxSignal() { return SIGRTMAX - 2; }

int suspendSignal()
{
    return SIGRTMAX - 2;
}

void allowSandboxSignal()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sandboxSignal());
    static const auto real = reinterpret_cast<decltype(&pthread_sigmask)>(dlsym(RTLD_NEXT, "pthread_sigmask"));
    real(SIG_UNBLOCK, &set, nullptr);
}

// Landlock refuses with EACCES what the mode of a file allows: Darwin's sandbox refuses with EPERM.
int sandboxError(int error, int directory, const char* path, int access)
{
    if (error != EACCES || !active || !path)
        return error;
    struct stat status;
    if (fstatat(directory, path, &status, 0) == 0) {
        struct stat file;
        if (fstatat(directory, path, &file, 0) == 0) {
            const int shift = file.st_uid == geteuid() ? 6 : file.st_gid == getegid() ? 3 : 0;
            const unsigned mode = file.st_mode >> shift;
            if ((!(access & 4) || (mode & 4)) && (!(access & 2) || (mode & 2)) && (!(access & 1) || (mode & 1)))
                return EPERM;
        }
        return error;
    }
    std::string parent = path;
    const size_t slash = parent.rfind('/');
    if (slash == std::string::npos)
        parent = ".";
    else if (slash == 0)
        parent = "/";
    else
        parent = parent.substr(0, slash);
    struct stat folder;
    if (fstatat(directory, parent.c_str(), &folder, 0) == 0) {
        const int shift = folder.st_uid == geteuid() ? 6 : folder.st_gid == getegid() ? 3 : 0;
        if ((folder.st_mode >> shift & 2) && (folder.st_mode >> shift & 1))
            return EPERM;
    }
    return error;
}

} // namespace cutefish::reef

// The signal of the sandbox stays open, whatever masks the program sets.
static const sigset_t* withoutSandboxSignal(int how, const sigset_t* set, sigset_t& copy)
{
    if (!set || how == SIG_UNBLOCK || sigismember(set, cutefish::reef::suspendSignal()) == 0)
        return set;
    copy = *set;
    sigdelset(&copy, cutefish::reef::suspendSignal());
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
