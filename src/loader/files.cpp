// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Files and memory.

#include "host.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <sys/syscall.h>
#include <sys/sysmacros.h>
#include <sys/uio.h>
#include <sys/xattr.h>
#include <unistd.h>

namespace cutefish::reef {

static int failure() { return -darwinError(errno); }

// AT_FDCWD is another number there.
static int hostDirectory(int directory) { return directory == -2 ? AT_FDCWD : directory; }

static int hostAtFlags(uint32_t flags)
{
    int host = 0;
    if (flags & 0x10)
        host |= AT_EACCESS;
    if (flags & 0x20)
        host |= AT_SYMLINK_NOFOLLOW;
    if (flags & 0x40)
        host |= AT_SYMLINK_FOLLOW;
    if (flags & 0x80)
        host |= AT_REMOVEDIR;
    return host;
}

static uint64_t darwinDevice(uint64_t major, uint64_t minor) { return ((major & 0xff) << 24) | (minor & 0xffffff); }

static void* hostMmap(void* address, size_t size, int protection, int flags, int fd, int64_t offset, int* error)
{
    int host = flags & (MAP_SHARED | MAP_PRIVATE | MAP_FIXED);
    if (flags & 0x40)
        host |= MAP_NORESERVE;
    if (flags & 0x1000) {
        host |= MAP_ANONYMOUS;
        fd = -1;
    }
    void* memory = mmap(address, size, protection & 7, host, fd, offset);
    if (memory == MAP_FAILED) {
        *error = darwinError(errno);
        return nullptr;
    }
    return memory;
}

static int hostMunmap(void* address, size_t size) { return munmap(address, size) < 0 ? failure() : 0; }
static int hostMprotect(void* address, size_t size, int protection) { return mprotect(address, size, protection & 7) < 0 ? failure() : 0; }

static int hostMsync(void* address, size_t size, int flags)
{
    int host = flags & 3;
    if (flags & 0x10)
        host |= MS_SYNC;
    return msync(address, size, host) < 0 ? failure() : 0;
}

static int hostMadvise(void* address, size_t size, int advice)
{
    int host;
    switch (advice) {
    case 0:
        host = MADV_NORMAL;
        break;
    case 1:
        host = MADV_RANDOM;
        break;
    case 2:
        host = MADV_SEQUENTIAL;
        break;
    case 3:
        host = MADV_WILLNEED;
        break;
    case 4:
        host = MADV_COLD;
        break;
    case 5:
        host = MADV_FREE;
        break;
    case 7:
        host = MADV_DONTNEED;
        break;
    default:
        return 0;
    }
    return madvise(address, size, host) < 0 ? failure() : 0;
}

static int hostMincore(void* address, size_t size, unsigned char* out) { return mincore(address, size, out) < 0 ? failure() : 0; }

// Whether the memory can be read.
static bool hostReadable(uintptr_t address, size_t size)
{
    if (size == 0)
        return true;
    const uintptr_t page = uintptr_t(sysconf(_SC_PAGESIZE));
    const uintptr_t last = address + size - 1;
    if (last < address)
        return false;
    constexpr int Count = 256;
    char bytes[Count];
    iovec local[Count], remote[Count];
    for (;;) {
        for (int i = 0; i < Count; i++) {
            remote[i] = {reinterpret_cast<void*>(address), 1};
            local[i] = {&bytes[i], 1};
            address = (address & -page) + page;
            if (address - 1 >= last) {
                const int count = i + 1;
                if (process_vm_readv(getpid(), local, count, remote, count, 0) != count)
                    return false;
                return true;
            }
        }
        if (process_vm_readv(getpid(), local, Count, remote, Count, 0) != Count)
            return false;
    }
}

static int hostStat(int directory, const char* path, uint32_t flags, HostStat* out)
{
    const std::string host = path ? hostPath(path) : std::string();
    int at = hostAtFlags(flags) & AT_SYMLINK_NOFOLLOW;
    if (!path)
        at |= AT_EMPTY_PATH;
    struct statx status;
    if (statx(hostDirectory(directory), path ? host.c_str() : "", at, STATX_ALL, &status) < 0)
        return failure();
    out->device = darwinDevice(status.stx_dev_major, status.stx_dev_minor);
    out->inode = status.stx_ino;
    out->links = status.stx_nlink;
    out->special = darwinDevice(status.stx_rdev_major, status.stx_rdev_minor);
    out->size = status.stx_size;
    out->blocks = status.stx_blocks;
    out->mode = status.stx_mode;
    out->user = status.stx_uid;
    out->group = status.stx_gid;
    out->blockSize = status.stx_blksize;
    out->accessed[0] = status.stx_atime.tv_sec;
    out->accessed[1] = status.stx_atime.tv_nsec;
    out->modified[0] = status.stx_mtime.tv_sec;
    out->modified[1] = status.stx_mtime.tv_nsec;
    out->changed[0] = status.stx_ctime.tv_sec;
    out->changed[1] = status.stx_ctime.tv_nsec;
    const statx_timestamp& born = status.stx_mask & STATX_BTIME ? status.stx_btime : status.stx_ctime;
    out->born[0] = born.tv_sec;
    out->born[1] = born.tv_nsec;
    const uint64_t attributes = status.stx_attributes & status.stx_attributes_mask;
    out->flags = (attributes & STATX_ATTR_NODUMP ? 1 : 0) | (attributes & STATX_ATTR_COMPRESSED ? 0x20 : 0) |
                 (attributes & STATX_ATTR_IMMUTABLE ? 0x20000 : 0) | (attributes & STATX_ATTR_APPEND ? 0x40000 : 0);
    return 0;
}

static int hostStatfs(const char* path, int fd, HostFileSystem* out)
{
    struct statvfs vfs;
    struct statfs fs;
    std::string full;
    if (path) {
        const std::string host = hostPath(path);
        if (statvfs(host.c_str(), &vfs) < 0 || statfs(host.c_str(), &fs) < 0)
            return failure();
        char resolved[PATH_MAX];
        full = realpath(host.c_str(), resolved) ? resolved : host;
    } else {
        if (fstatvfs(fd, &vfs) < 0 || fstatfs(fd, &fs) < 0)
            return failure();
        char link[64];
        snprintf(link, sizeof link, "/proc/self/fd/%d", fd);
        char target[PATH_MAX];
        const ssize_t size = readlink(link, target, sizeof target - 1);
        full = size > 0 ? std::string(target, size_t(size)) : std::string("/");
    }
    memset(out, 0, sizeof *out);
    out->blocks = vfs.f_blocks;
    out->free = vfs.f_bfree;
    out->available = vfs.f_bavail;
    out->files = vfs.f_files;
    out->freeFiles = vfs.f_ffree;
    out->blockSize = uint32_t(vfs.f_frsize ? vfs.f_frsize : vfs.f_bsize);
    out->transferSize = uint32_t(vfs.f_bsize);
    struct stat status;
    if ((path ? stat(hostPath(path).c_str(), &status) : fstat(fd, &status)) == 0) {
        out->id[0] = int32_t(darwinDevice(major(status.st_dev), minor(status.st_dev)));
        out->id[1] = int32_t(fs.f_type);
    } else {
        memcpy(out->id, &fs.f_fsid, sizeof out->id);
    }
    if (FILE* mounts = fopen("/proc/self/mountinfo", "re")) {
        char* line = nullptr;
        size_t capacity = 0;
        size_t longest = 0;
        while (getline(&line, &capacity, mounts) > 0) {
            char mount[1024];
            if (sscanf(line, "%*s %*s %*s %*s %1023s", mount) != 1)
                continue;
            const size_t length = strlen(mount);
            if (full.compare(0, length, mount) != 0 || (length != 1 && length != full.size() && full[length] != '/') || length < longest)
                continue;
            const char* rest = strstr(line, " - ");
            char type[64];
            char source[1024];
            if (!rest || sscanf(rest + 3, "%63s %1023s", type, source) != 2)
                continue;
            longest = length;
            snprintf(out->type, sizeof out->type, "%.15s", type);
            snprintf(out->mount, sizeof out->mount, "%s", mount);
            snprintf(out->source, sizeof out->source, "%s", source);
        }
        free(line);
        fclose(mounts);
    }
    if (vfs.f_flag & ST_RDONLY)
        out->flags |= 1;
    if (vfs.f_flag & ST_NOEXEC)
        out->flags |= 4;
    if (vfs.f_flag & ST_NOSUID)
        out->flags |= 8;
    if (vfs.f_flag & ST_NODEV)
        out->flags |= 0x10;
    const std::string type = out->type;
    if (type != "nfs" && type != "nfs4" && type != "cifs" && type != "smb3" && type != "9p" && !type.starts_with("fuse."))
        out->flags |= 0x1000;
    return 0;
}

static long hostPathconf(const char* path, int fd, int name)
{
    const std::string host = path ? hostPath(path) : std::string();
    int hostName;
    switch (name) {
    case 1:
        hostName = _PC_LINK_MAX;
        break;
    case 2:
        hostName = _PC_MAX_CANON;
        break;
    case 3:
        hostName = _PC_MAX_INPUT;
        break;
    case 4:
    case 10:
        hostName = _PC_NAME_MAX;
        break;
    case 5:
        hostName = _PC_PATH_MAX;
        break;
    case 6:
        hostName = _PC_PIPE_BUF;
        break;
    case 7:
        hostName = _PC_CHOWN_RESTRICTED;
        break;
    case 8:
        hostName = _PC_NO_TRUNC;
        break;
    case 9:
        hostName = _PC_VDISABLE;
        break;
    case 11:
    case 12: {
        struct statfs fs;
        if ((path ? statfs(host.c_str(), &fs) : fstatfs(fd, &fs)) < 0)
            return failure();
        // FAT and exFAT ignore the case of names.
        const bool caseless = fs.f_type == 0x4d44 || fs.f_type == 0x2011bab0;
        return name == 11 ? !caseless : 1;
    }
    case 13:
    case 14: {
        struct stat status;
        return (path ? stat(host.c_str(), &status) : fstat(fd, &status)) < 0 ? failure() : 0;
    }
    case 15:
        hostName = _PC_2_SYMLINKS;
        break;
    case 16:
        hostName = _PC_ALLOC_SIZE_MIN;
        break;
    case 17:
        hostName = _PC_ASYNC_IO;
        break;
    case 18:
        hostName = _PC_FILESIZEBITS;
        break;
    case 19:
        hostName = _PC_PRIO_IO;
        break;
    case 20:
        hostName = _PC_REC_INCR_XFER_SIZE;
        break;
    case 21:
        hostName = _PC_REC_MAX_XFER_SIZE;
        break;
    case 22:
        hostName = _PC_REC_MIN_XFER_SIZE;
        break;
    case 23:
        hostName = _PC_REC_XFER_ALIGN;
        break;
    case 24:
        hostName = _PC_SYMLINK_MAX;
        break;
    case 25:
        hostName = _PC_SYNC_IO;
        break;
    default:
        return -22;
    }
    errno = 0;
    const long value = path ? pathconf(host.c_str(), hostName) : fpathconf(fd, hostName);
    if (value < 0)
        return errno ? failure() : LONG_MIN;
    return value;
}

static int hostAccess(int directory, const char* path, int mode, uint32_t flags)
{
    return faccessat(hostDirectory(directory), hostPath(path).c_str(), mode, hostAtFlags(flags)) < 0 ? failure() : 0;
}

static int hostMkdir(int directory, const char* path, uint32_t mode)
{
    const std::string host = hostPath(path);
    if (mkdirat(hostDirectory(directory), host.c_str(), mode) == 0)
        return 0;
    return -darwinError(sandboxError(errno, hostDirectory(directory), host.c_str(), 2));
}

static int hostUnlink(int directory, const char* path, uint32_t flags)
{
    const std::string host = hostPath(path);
    if (unlinkat(hostDirectory(directory), host.c_str(), hostAtFlags(flags) & AT_REMOVEDIR) == 0)
        return 0;
    return -darwinError(sandboxError(errno, hostDirectory(directory), host.c_str(), 2));
}

static int hostRename(int directory, const char* from, int toDirectory, const char* to)
{
    const std::string host = hostPath(to, false);
    if (renameat(hostDirectory(directory), hostPath(from).c_str(), hostDirectory(toDirectory), host.c_str()) == 0)
        return 0;
    return -darwinError(sandboxError(errno, hostDirectory(toDirectory), host.c_str(), 2));
}

static int hostRenamex(int directory, const char* from, int toDirectory, const char* to, uint32_t flags)
{
    unsigned host = 0;
    if (flags & 2)
        host |= RENAME_EXCHANGE;
    if (flags & 4)
        host |= RENAME_NOREPLACE;
    return renameat2(hostDirectory(directory), hostPath(from).c_str(), hostDirectory(toDirectory), hostPath(to, false).c_str(), host) < 0 ? failure() : 0;
}

static long hostReadlink(int directory, const char* path, char* buffer, size_t size)
{
    const ssize_t count = readlinkat(hostDirectory(directory), hostPath(path).c_str(), buffer, size);
    return count < 0 ? failure() : count;
}

static int hostSymlink(const char* target, int directory, const char* path)
{
    const std::string host = hostPath(path);
    if (symlinkat(target, hostDirectory(directory), host.c_str()) == 0)
        return 0;
    return -darwinError(sandboxError(errno, hostDirectory(directory), host.c_str(), 2));
}

static int hostChmod(int directory, const char* path, uint32_t mode, uint32_t flags)
{
    if (!path)
        return fchmod(directory, mode) < 0 ? failure() : 0;
    return fchmodat(hostDirectory(directory), hostPath(path).c_str(), mode, hostAtFlags(flags) & AT_SYMLINK_NOFOLLOW) < 0 ? failure() : 0;
}

static int hostLink(int directory, const char* from, int toDirectory, const char* to, uint32_t flags)
{
    return linkat(hostDirectory(directory), hostPath(from).c_str(), hostDirectory(toDirectory), hostPath(to, false).c_str(), hostAtFlags(flags)) < 0 ? failure() : 0;
}

static int hostChown(int directory, const char* path, uint32_t user, uint32_t group, uint32_t flags)
{
    if (!path)
        return fchown(directory, user, group) < 0 ? failure() : 0;
    return fchownat(hostDirectory(directory), hostPath(path).c_str(), user, group, hostAtFlags(flags)) < 0 ? failure() : 0;
}

static int hostUtimes(int directory, const char* path, const HostTime* times, uint32_t flags)
{
    timespec host[2];
    const timespec* set = nullptr;
    for (int i = 0; times && i < 2; ++i) {
        host[i].tv_sec = times[i].seconds;
        host[i].tv_nsec = times[i].nanoseconds == -1 ? UTIME_NOW : times[i].nanoseconds == -2 ? UTIME_OMIT : times[i].nanoseconds;
        set = host;
    }
    const int r = path ? utimensat(hostDirectory(directory), hostPath(path).c_str(), set, hostAtFlags(flags)) : futimens(directory, set);
    return r < 0 ? failure() : 0;
}

static int hostGetcwd(char* buffer, size_t size) { return getcwd(buffer, size) ? 0 : failure(); }

static int hostChdir(const char* path, int fd)
{
    if (!path)
        return fchdir(fd) < 0 ? failure() : 0;
    return chdir(hostPath(path).c_str()) < 0 ? failure() : 0;
}

static int hostRealpath(const char* path, char* out)
{
    char resolved[PATH_MAX];
    if (!realpath(hostPath(path).c_str(), resolved))
        return failure();
    const size_t length = strlen(resolved);
    if (length >= 1024)
        return -63;
    memcpy(out, resolved, length + 1);
    return 0;
}

static int hostOpenat(int directory, const char* path, int flags, int mode)
{
    const std::string host = hostPath(path);
    const int hostFlags = hostOpenFlags(flags);
    const int fd = openat(hostDirectory(directory), host.c_str(), hostFlags, mode);
    if (fd < 0) {
        const int access = (hostFlags & O_ACCMODE) == O_RDONLY ? 4 : (hostFlags & O_ACCMODE) == O_WRONLY ? 2 : 6;
        return -darwinError(sandboxError(errno, hostDirectory(directory), host.c_str(), access));
    }
    return fd;
}

static int hostFcntl(int fd, int command, uint64_t argument)
{
    int value;
    switch (command) {
    case 0:
        value = fcntl(fd, F_DUPFD, int(argument));
        break;
    case 1:
        value = fcntl(fd, F_GETFD);
        break;
    case 2:
        value = fcntl(fd, F_SETFD, int(argument));
        break;
    case 3:
        value = fcntl(fd, F_GETFL);
        if (value >= 0)
            value = darwinOpenFlags(value);
        break;
    case 4:
        value = fcntl(fd, F_SETFL, hostOpenFlags(int(argument)));
        break;
    case 5:
        value = fcntl(fd, F_GETOWN);
        break;
    case 6:
        value = fcntl(fd, F_SETOWN, int(argument));
        break;
    case 7:
    case 8:
    case 9: {
        HostLock* lock = reinterpret_cast<HostLock*>(argument);
        struct flock host = {};
        host.l_type = lock->type == 1 ? F_RDLCK : lock->type == 3 ? F_WRLCK : F_UNLCK;
        host.l_whence = lock->whence;
        host.l_start = lock->start;
        host.l_len = lock->length;
        host.l_pid = lock->process;
        value = fcntl(fd, command == 7 ? F_GETLK : command == 8 ? F_SETLK : F_SETLKW, &host);
        if (value >= 0 && command == 7) {
            lock->type = host.l_type == F_RDLCK ? 1 : host.l_type == F_WRLCK ? 3 : 2;
            lock->whence = host.l_whence;
            lock->start = host.l_start;
            lock->length = host.l_len;
            lock->process = host.l_pid;
        }
        break;
    }
    case 42:
    case 44:
    case 48:
        return 0;
    case 50: {
        char link[64];
        snprintf(link, sizeof link, "/proc/self/fd/%d", fd);
        char* path = reinterpret_cast<char*>(argument);
        const ssize_t size = readlink(link, path, 1023);
        if (size < 0)
            return failure();
        path[size] = '\0';
        return 0;
    }
    case 51:
    case 85:
        value = fsync(fd);
        break;
    case 67:
        value = fcntl(fd, F_DUPFD_CLOEXEC, int(argument));
        break;
    case 73:
        value = fcntl(fd, F_GETFD);
        if (value >= 0) {
            setSocketNoSignal(fd, argument != 0);
            value = 0;
        }
        break;
    case 74:
        value = fcntl(fd, F_GETFD);
        if (value >= 0)
            value = socketNoSignal(fd);
        break;
    case 99: {
        const int64_t* hole = reinterpret_cast<const int64_t*>(argument);
        value = fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, hole[1], hole[2]);
        break;
    }
    default:
        return -22;
    }
    return value < 0 ? failure() : value;
}

static int hostDup(int fd)
{
    const int copy = dup(fd);
    return copy < 0 ? failure() : copy;
}

static int hostDup2(int fd, int other)
{
    const int copy = dup2(fd, other);
    if (copy >= 0 && fd != other)
        forgetSocket(other);
    return copy < 0 ? failure() : copy;
}

static int hostPipe(int* fds) { return pipe(fds) < 0 ? failure() : 0; }
static int hostFtruncate(int fd, int64_t size) { return ftruncate(fd, size) < 0 ? failure() : 0; }
static int hostFsync(int fd) { return fsync(fd) < 0 ? failure() : 0; }

static long hostPread(int fd, void* buffer, size_t size, int64_t offset)
{
    const ssize_t count = pread(fd, buffer, size, offset);
    return count < 0 ? failure() : count;
}

static long hostPwrite(int fd, const void* buffer, size_t size, int64_t offset)
{
    const ssize_t count = pwrite(fd, buffer, size, offset);
    return count < 0 ? failure() : count;
}

static void* hostOpendir(const char* path, int fd, int* error)
{
    const std::string host = path ? hostPath(path) : std::string();
    if (DIR* stream = path ? opendir(host.c_str()) : fdopendir(fd))
        return stream;
    *error = darwinError(path ? sandboxError(errno, AT_FDCWD, host.c_str(), 4) : errno);
    return nullptr;
}

static int hostReaddir(void* stream, HostEntry* entry)
{
    errno = 0;
    const dirent* next = readdir(static_cast<DIR*>(stream));
    if (!next)
        return errno ? failure() : 0;
    entry->inode = next->d_ino;
    entry->offset = next->d_off;
    entry->type = next->d_type;
    snprintf(entry->name, sizeof entry->name, "%s", next->d_name);
    return 1;
}

static int hostClosedir(void* stream) { return closedir(static_cast<DIR*>(stream)); }
static void hostSeekdir(void* stream, long position) { seekdir(static_cast<DIR*>(stream), position); }
static long hostTelldir(void* stream) { return telldir(static_cast<DIR*>(stream)); }
static int hostDirfd(void* stream) { return dirfd(static_cast<DIR*>(stream)); }

static int hostGetdirentries(int fd, HostEntry* entries, int count)
{
    if (count <= 0)
        return 0;
    char buffer[32768];
    const long size = syscall(SYS_getdents64, fd, buffer, sizeof buffer);
    if (size < 0)
        return failure();
    int used = 0;
    for (long at = 0; at < size;) {
        const dirent64* next = reinterpret_cast<const dirent64*>(buffer + at);
        // The rest is read again.
        if (used == count)
            return lseek(fd, entries[used - 1].offset, SEEK_SET) < 0 ? failure() : used;
        HostEntry& entry = entries[used++];
        entry.inode = next->d_ino;
        entry.offset = next->d_off;
        entry.type = next->d_type;
        snprintf(entry.name, sizeof entry.name, "%s", next->d_name);
        at += next->d_reclen;
    }
    return used;
}

// The programs' attributes are the host's user attributes.
static long hostGetxattr(const char* path, int fd, const char* name, void* value, size_t size, uint32_t options)
{
    const std::string attribute = std::string("user.") + name;
    ssize_t count;
    if (!path)
        count = fgetxattr(fd, attribute.c_str(), value, size);
    else if (options & 1)
        count = lgetxattr(hostPath(path).c_str(), attribute.c_str(), value, size);
    else
        count = getxattr(hostPath(path).c_str(), attribute.c_str(), value, size);
    if (count < 0)
        return errno == ENODATA ? -93 : failure();
    return count;
}

static int hostSetxattr(const char* path, int fd, const char* name, const void* value, size_t size, uint32_t options)
{
    const std::string attribute = std::string("user.") + name;
    int flags = 0;
    if (options & 2)
        flags |= XATTR_CREATE;
    if (options & 4)
        flags |= XATTR_REPLACE;
    int r;
    if (!path)
        r = fsetxattr(fd, attribute.c_str(), value, size, flags);
    else if (options & 1)
        r = lsetxattr(hostPath(path).c_str(), attribute.c_str(), value, size, flags);
    else
        r = setxattr(hostPath(path).c_str(), attribute.c_str(), value, size, flags);
    if (r < 0)
        return errno == ENODATA ? -93 : failure();
    return 0;
}

static int hostRemovexattr(const char* path, int fd, const char* name, uint32_t options)
{
    const std::string attribute = std::string("user.") + name;
    int r;
    if (!path)
        r = fremovexattr(fd, attribute.c_str());
    else if (options & 1)
        r = lremovexattr(hostPath(path).c_str(), attribute.c_str());
    else
        r = removexattr(hostPath(path).c_str(), attribute.c_str());
    if (r < 0)
        return errno == ENODATA ? -93 : failure();
    return 0;
}

static long hostListxattr(const char* path, int fd, char* names, size_t size, uint32_t options)
{
    std::string all(65536, '\0');
    ssize_t count;
    if (!path)
        count = flistxattr(fd, all.data(), all.size());
    else if (options & 1)
        count = llistxattr(hostPath(path).c_str(), all.data(), all.size());
    else
        count = listxattr(hostPath(path).c_str(), all.data(), all.size());
    if (count < 0)
        return errno == ENODATA ? -93 : failure();
    std::string own;
    for (size_t at = 0; at < size_t(count);) {
        const char* name = all.c_str() + at;
        const size_t length = strlen(name);
        if (strncmp(name, "user.", 5) == 0)
            own.append(name + 5, length - 4);
        at += length + 1;
    }
    if (!names)
        return long(own.size());
    if (own.size() > size)
        return -34;
    memcpy(names, own.data(), own.size());
    return long(own.size());
}

static int hostInotifyInit(int flags)
{
    const int fd = inotify_init1(flags);
    return fd < 0 ? failure() : fd;
}

static int hostInotifyAddWatch(int fd, const char* path, uint32_t mask)
{
    const int watch = inotify_add_watch(fd, hostPath(path).c_str(), mask);
    return watch < 0 ? failure() : watch;
}

static int hostInotifyRemoveWatch(int fd, int watch) { return inotify_rm_watch(fd, watch) < 0 ? failure() : 0; }

void addFileServices(HostServices& services)
{
    services.mincore = hostMincore;
    services.readable = hostReadable;
    services.pathconf = hostPathconf;
    services.renamex = hostRenamex;
    services.inotifyInit = hostInotifyInit;
    services.inotifyAddWatch = hostInotifyAddWatch;
    services.inotifyRemoveWatch = hostInotifyRemoveWatch;
    services.mmap = hostMmap;
    services.munmap = hostMunmap;
    services.mprotect = hostMprotect;
    services.msync = hostMsync;
    services.madvise = hostMadvise;
    services.stat = hostStat;
    services.statfs = hostStatfs;
    services.access = hostAccess;
    services.mkdir = hostMkdir;
    services.unlink = hostUnlink;
    services.rename = hostRename;
    services.readlink = hostReadlink;
    services.symlink = hostSymlink;
    services.chmod = hostChmod;
    services.link = hostLink;
    services.chown = hostChown;
    services.utimes = hostUtimes;
    services.getcwd = hostGetcwd;
    services.chdir = hostChdir;
    services.realpath = hostRealpath;
    services.openat = hostOpenat;
    services.fcntl = hostFcntl;
    services.dup = hostDup;
    services.dup2 = hostDup2;
    services.pipe = hostPipe;
    services.ftruncate = hostFtruncate;
    services.fsync = hostFsync;
    services.pread = hostPread;
    services.pwrite = hostPwrite;
    services.opendir = hostOpendir;
    services.readdir = hostReaddir;
    services.closedir = hostClosedir;
    services.seekdir = hostSeekdir;
    services.telldir = hostTelldir;
    services.dirfd = hostDirfd;
    services.getdirentries = hostGetdirentries;
    services.getxattr = hostGetxattr;
    services.setxattr = hostSetxattr;
    services.removexattr = hostRemovexattr;
    services.listxattr = hostListxattr;
}

} // namespace cutefish::reef
