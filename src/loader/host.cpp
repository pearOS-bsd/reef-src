// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "host.h"
#include "linker.h"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <malloc.h>
#include <map>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <sched.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <syslog.h>
#include <unistd.h>

namespace cutefish::reef {

static constexpr int KeyCount = 768;
// The keys before it are the system library's own.
static constexpr int FirstKey = 256;

static HostServices services;
static std::once_flag threadOnce;
static pthread_key_t threadKey;
static void (*destructors[KeyCount])(void*);
static bool keyUsed[KeyCount];
static std::mutex keyMutex;

static thread_local int isMain = -1;
static thread_local void* threadValue;
static thread_local void* slots[KeyCount];
static thread_local uintptr_t stackEnd;
static thread_local uintptr_t stackStart;

int darwinError(int error)
{
    switch (error) {
    case EAGAIN:
        return 35;
    case EDEADLK:
        return 11;
    case ENAMETOOLONG:
        return 63;
    case ENOLCK:
        return 77;
    case ENOSYS:
        return 78;
    case ENOTEMPTY:
        return 66;
    case ELOOP:
        return 62;
    case ENOMSG:
        return 91;
    case EIDRM:
        return 90;
    case ENOSTR:
        return 99;
    case ENODATA:
        return 96;
    case ETIME:
        return 101;
    case ENOSR:
        return 98;
    case EREMOTE:
        return 71;
    case ENOLINK:
        return 97;
    case EPROTO:
        return 100;
    case EMULTIHOP:
        return 95;
    case EBADMSG:
        return 94;
    case EOVERFLOW:
        return 84;
    case EILSEQ:
        return 92;
    case EUSERS:
        return 68;
    case ENOTSOCK:
        return 38;
    case EDESTADDRREQ:
        return 39;
    case EMSGSIZE:
        return 40;
    case EPROTOTYPE:
        return 41;
    case ENOPROTOOPT:
        return 42;
    case EPROTONOSUPPORT:
        return 43;
    case ESOCKTNOSUPPORT:
        return 44;
    case EOPNOTSUPP:
        return 45;
    case EPFNOSUPPORT:
        return 46;
    case EAFNOSUPPORT:
        return 47;
    case EADDRINUSE:
        return 48;
    case EADDRNOTAVAIL:
        return 49;
    case ENETDOWN:
        return 50;
    case ENETUNREACH:
        return 51;
    case ENETRESET:
        return 52;
    case ECONNABORTED:
        return 53;
    case ECONNRESET:
        return 54;
    case ENOBUFS:
        return 55;
    case EISCONN:
        return 56;
    case ENOTCONN:
        return 57;
    case ESHUTDOWN:
        return 58;
    case ETOOMANYREFS:
        return 59;
    case ETIMEDOUT:
        return 60;
    case ECONNREFUSED:
        return 61;
    case EHOSTDOWN:
        return 64;
    case EHOSTUNREACH:
        return 65;
    case EALREADY:
        return 37;
    case EINPROGRESS:
        return 36;
    case ESTALE:
        return 70;
    case EDQUOT:
        return 69;
    case ECANCELED:
        return 89;
    case EOWNERDEAD:
        return 105;
    case ENOTRECOVERABLE:
        return 104;
    }
    return error > 0 && error < 35 ? error : 5;
}

int linuxError(int error)
{
    if (error == 11)
        return EDEADLK;
    if (error >= 1 && error <= 34)
        return error;
    for (int host = 1; host < 134; ++host) {
        if (darwinError(host) == error && host != EIO)
            return host;
    }
    return EIO;
}

static int failure() { return -darwinError(errno); }

int hostOpenFlags(int flags)
{
    int host = flags & O_ACCMODE;
    if (flags & 4)
        host |= O_NONBLOCK;
    if (flags & 8)
        host |= O_APPEND;
    if (flags & 0x80)
        host |= O_SYNC;
    if (flags & 0x100)
        host |= O_NOFOLLOW;
    if (flags & 0x200)
        host |= O_CREAT;
    if (flags & 0x400)
        host |= O_TRUNC;
    if (flags & 0x800)
        host |= O_EXCL;
    if (flags & 0x20000)
        host |= O_NOCTTY;
    if (flags & 0x100000)
        host |= O_DIRECTORY;
    if (flags & 0x400000)
        host |= O_DSYNC;
    if (flags & 0x1000000)
        host |= O_CLOEXEC;
    return host;
}

int darwinOpenFlags(int flags)
{
    int own = flags & O_ACCMODE;
    if (flags & O_NONBLOCK)
        own |= 4;
    if (flags & O_APPEND)
        own |= 8;
    if ((flags & O_SYNC) == O_SYNC)
        own |= 0x80;
    else if (flags & O_DSYNC)
        own |= 0x400000;
    if (flags & O_NOFOLLOW)
        own |= 0x100;
    if (flags & O_CREAT)
        own |= 0x200;
    if (flags & O_TRUNC)
        own |= 0x400;
    if (flags & O_EXCL)
        own |= 0x800;
    if (flags & O_NOCTTY)
        own |= 0x20000;
    if (flags & O_DIRECTORY)
        own |= 0x100000;
    if (flags & O_CLOEXEC)
        own |= 0x1000000;
    return own;
}

void tuneAllocator()
{
    mallopt(M_MMAP_THRESHOLD, 128 * 1024);
    mallopt(M_ARENA_MAX, 4);
}

bool hostClock(int clock, clockid_t* host)
{
    switch (clock) {
    case 0:
        *host = CLOCK_REALTIME;
        return true;
    case 4:
    case 5:
    case 6:
        *host = CLOCK_BOOTTIME;
        return true;
    case 8:
    case 9:
        *host = CLOCK_MONOTONIC;
        return true;
    case 12:
        *host = CLOCK_PROCESS_CPUTIME_ID;
        return true;
    case 16:
        *host = CLOCK_THREAD_CPUTIME_ID;
        return true;
    }
    return false;
}

static void destroyThread(void*)
{
    // Destructors may set values again.
    for (int round = 0; round < 4; ++round) {
        bool again = false;
        for (int key = FirstKey; key < KeyCount; ++key) {
            void* value = slots[key];
            if (!value)
                continue;
            void (*destructor)(void*) = nullptr;
            {
                std::lock_guard lock(keyMutex);
                if (keyUsed[key])
                    destructor = destructors[key];
            }
            slots[key] = nullptr;
            if (destructor) {
                destructor(value);
                again = true;
            }
        }
        if (!again)
            return;
    }
}

void setUpThread()
{
    std::call_once(threadOnce, [] { pthread_key_create(&threadKey, destroyThread); });
    pthread_setspecific(threadKey, slots);
    slots[0] = reinterpret_cast<void*>(pthread_self());
    syscall(SYS_arch_prctl, 0x1001, slots);
}

struct ThreadStart
{
    void* (*start)(void*);
    void* argument;
};

static void* startThread(void* argument)
{
    const std::unique_ptr<ThreadStart> owned(static_cast<ThreadStart*>(argument));
    const ThreadStart start = *owned;
    allowSandboxSignal();
    setUpThread();
    return start.start(start.argument);
}

static void* startOwnThread(void* argument)
{
    const ThreadStart start = *static_cast<ThreadStart*>(argument);
    delete static_cast<ThreadStart*>(argument);
    allowSandboxSignal();
    setUpSignalStack();
    setUpThread();
    return start.start(start.argument);
}

// Paths.

// The name of the last component, in the case the folder has it.
static std::string existingPath(const char* path, bool follow)
{
    struct stat status;
    if (*path != '/' || lstat(path, &status) == 0 || errno != ENOENT)
        return path;
    std::string found;
    const char* at = path;
    while (*at) {
        if (*at == '/') {
            ++at;
            continue;
        }
        const char* end = strchrnul(at, '/');
        const std::string name(at, size_t(end - at));
        const bool last = *end == '\0' || end[strspn(end, "/")] == '\0';
        std::string full = found + '/' + name;
        if (name == "." || name == "..") {
            found = std::move(full);
        } else if (lstat(full.c_str(), &status) == 0) {
            found = std::move(full);
        } else if (errno != ENOENT) {
            return path;
        } else {
            std::string match;
            if (!last || follow) {
                if (DIR* directory = opendir(found.empty() ? "/" : found.c_str())) {
                    while (const dirent* entry = readdir(directory)) {
                        if (strcasecmp(entry->d_name, name.c_str()) == 0) {
                            match = entry->d_name;
                            break;
                        }
                    }
                    closedir(directory);
                }
            }
            if (!match.empty())
                found += '/' + match;
            else if (last)
                found = std::move(full);
            else
                return path;
        }
        at = end;
    }
    if (path[strlen(path) - 1] == '/')
        found += '/';
    return found.empty() ? path : found;
}

static constexpr const char* CommandFolders[] = {"/usr/bin/", "/usr/sbin/", "/bin/", "/sbin/"};

std::string hostPath(const char* path, bool follow)
{
    const Linker* linker = Linker::current;
    if (strncmp(path, "/System/", 8) == 0 || strncmp(path, "/usr/lib/", 9) == 0) {
        std::string own = linker->root() + path;
        if (access(own.c_str(), F_OK) == 0)
            return own;
    }
    for (const char* command : {"/usr/bin/tar", "/bin/tar", "/usr/bin/login", "/usr/bin/profiles"}) {
        if (strcmp(path, command) != 0)
            continue;
        std::string own = linker->root() + "/usr/bin" + strrchr(command, '/');
        if (access(own.c_str(), X_OK) == 0)
            return own;
    }
    for (const char* folder : CommandFolders) {
        const size_t length = strlen(folder);
        if (strncmp(path, folder, length) != 0 || strchr(path + length, '/') || access(path, F_OK) == 0)
            continue;
        for (const char* other : CommandFolders) {
            std::string command = std::string(other) + (path + length);
            if (access(command.c_str(), F_OK) == 0)
                return command;
        }
        std::string own = linker->root() + path;
        if (access(own.c_str(), F_OK) == 0)
            return own;
        break;
    }
    return existingPath(path, follow);
}

// Processes and memory.

static void hostExit(int status)
{
    fflush(nullptr);
    _exit(status);
}

static void hostAbort(const char* message)
{
    if (message)
        fprintf(stderr, "%s\n", message);
    abort();
}

static void hostFail(const char* message)
{
    fprintf(stderr, "reef: %s\n", message);
    showUnsupported(true, message);
    abort();
}

static int hostGetpid() { return getpid(); }

static void hostRandom(void* buffer, size_t size)
{
    char* at = static_cast<char*>(buffer);
    while (size > 0) {
        const ssize_t count = getrandom(at, size, 0);
        if (count < 0) {
            if (errno == EINTR)
                continue;
            abort();
        }
        at += count;
        size -= size_t(count);
    }
}

static uintptr_t heapStart;

static void findHeap()
{
    FILE* maps = fopen("/proc/self/maps", "re");
    if (!maps)
        return;
    char line[512];
    while (fgets(line, sizeof line, maps)) {
        unsigned long start = 0;
        if (strstr(line, "[heap]") && sscanf(line, "%lx-", &start) == 1) {
            heapStart = start;
            break;
        }
    }
    fclose(maps);
}

static size_t hostAllocationSize(void* memory) { return memory ? malloc_usable_size(memory) : 0; }
static int hostPosixMemalign(void** memory, size_t alignment, size_t size)
{
    const int error = posix_memalign(memory, alignment, size);
    return error ? darwinError(error) : 0;
}

static void hostTrim() { malloc_trim(0); }

static std::atomic<int> allocating;

static void hostManyAllocations(int more)
{
    if (more) {
        if (allocating.fetch_add(1) == 0) {
            mallopt(M_MMAP_THRESHOLD, 32 * 1024 * 1024);
            mallopt(M_TRIM_THRESHOLD, 64 * 1024 * 1024);
        }
    } else if (allocating.fetch_sub(1) == 1) {
        mallopt(M_MMAP_THRESHOLD, 128 * 1024);
        mallopt(M_TRIM_THRESHOLD, 128 * 1024);
    }
}

static void hostAllocated(size_t* used, size_t* total)
{
    const struct mallinfo2 info = mallinfo2();
    *used = info.uordblks + info.hblkhd;
    *total = info.arena + info.hblkhd;
}

// A word near the pointer, or wherever it is, without a fault.
static bool readWord(uintptr_t pointer, uintptr_t address, size_t* value)
{
    static const uintptr_t page = uintptr_t(sysconf(_SC_PAGESIZE));
    if (((pointer ^ address) & -page) == 0 ||
        (heapStart && address >= heapStart && address + sizeof(size_t) <= reinterpret_cast<uintptr_t>(sbrk(0)))) {
        *value = *reinterpret_cast<const size_t*>(address);
        return true;
    }
    const iovec local = {value, sizeof *value};
    const iovec remote = {reinterpret_cast<void*>(address), sizeof *value};
    return process_vm_readv(getpid(), &local, 1, &remote, 1, 0) == ssize_t(local.iov_len);
}

// The size of the allocation, 0 for what is not one.
static size_t hostPointerSize(const void* memory)
{
    const uintptr_t address = reinterpret_cast<uintptr_t>(memory);
    if (address == 0 || address & 15)
        return 0;
    if (!stackEnd) {
        pthread_attr_t attributes;
        if (pthread_getattr_np(pthread_self(), &attributes) == 0) {
            void* stack = nullptr;
            size_t size = 0;
            pthread_attr_getstack(&attributes, &stack, &size);
            stackStart = reinterpret_cast<uintptr_t>(stack);
            stackEnd = stackStart + size;
            pthread_attr_destroy(&attributes);
        }
    }
    if (address >= stackStart && address < stackEnd)
        return 0;
    HostSymbol symbol = {};
    if (Linker::current && Linker::current->contains(address) && Linker::current->findSymbol(memory, &symbol))
        return 0;
    // The size field of the allocator's chunk.
    size_t chunk;
    if (!readWord(address, address - sizeof(size_t), &chunk))
        return 0;
    const size_t size = chunk & ~size_t(7);
    if (size < 32 || chunk & 8)
        return 0;
    if (chunk & 2) {
        if ((address - 16) % size_t(sysconf(_SC_PAGESIZE)) != 0)
            return 0;
    } else {
        size_t next;
        if (!readWord(address, address - sizeof(size_t) + size, &next) || !(next & 1))
            return 0;
    }
    return malloc_usable_size(const_cast<void*>(memory));
}

static const void* hostMemchr(const void* memory, int value, size_t size) { return memchr(memory, value, size); }
static const char* hostStrchr(const char* text, int value) { return strchr(text, value); }
static const char* hostStrrchr(const char* text, int value) { return strrchr(text, value); }
static const char* hostStrstr(const char* text, const char* part) { return strstr(text, part); }
static int hostFormatDouble(char* buffer, size_t size, const char* format, double value) { return snprintf(buffer, size, format, value); }

// Files.

static long hostRead(int fd, void* buffer, size_t size)
{
    const ssize_t count = read(fd, buffer, size);
    return count < 0 ? failure() : count;
}

static long hostWrite(int fd, const void* buffer, size_t size)
{
    const ssize_t count = socketNoSignal(fd) ? withoutSignal([&] { return write(fd, buffer, size); }) : write(fd, buffer, size);
    return count < 0 ? failure() : count;
}

static int hostOpen(const char* path, int flags, uint32_t mode)
{
    const std::string host = hostPath(path);
    const int hostFlags = hostOpenFlags(flags);
    const int fd = open(host.c_str(), hostFlags, mode);
    if (fd < 0) {
        const int access = (hostFlags & O_ACCMODE) == O_RDONLY ? 4 : (hostFlags & O_ACCMODE) == O_WRONLY ? 2 : 6;
        return -darwinError(sandboxError(errno, AT_FDCWD, host.c_str(), access));
    }
    return fd;
}

static int hostClose(int fd)
{
    forgetSocket(fd);
    return close(fd) < 0 ? failure() : 0;
}

static int64_t hostLseek(int fd, int64_t offset, int origin)
{
    const off_t position = lseek(fd, offset, origin);
    return position < 0 ? failure() : position;
}

static int hostIsatty(int fd) { return isatty(fd); }

static int hostMemoryFile(const char* name)
{
    const long fd = syscall(SYS_memfd_create, name ? name : "reef", 1);
    return fd < 0 ? failure() : int(fd);
}

// Time.

static int hostClockGettime(int clock, int64_t* seconds, int64_t* nanoseconds)
{
    clockid_t host;
    if (!hostClock(clock, &host))
        return -22;
    timespec time;
    if (clock_gettime(host, &time) < 0)
        return failure();
    *seconds = time.tv_sec;
    *nanoseconds = time.tv_nsec;
    return 0;
}

static uint64_t hostMonotonic()
{
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return uint64_t(time.tv_sec) * 1000000000 + uint64_t(time.tv_nsec);
}

static int hostNanosleep(int64_t seconds, int64_t nanoseconds)
{
    const timespec time = {seconds, nanoseconds};
    return nanosleep(&time, nullptr) < 0 ? failure() : 0;
}

// Threads.

static int hostCreateThread(uintptr_t* thread, void* (*start)(void*), void* argument, size_t stackSize)
{
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    if (stackSize)
        pthread_attr_setstacksize(&attributes, std::max(stackSize, size_t(PTHREAD_STACK_MIN)));
    auto* own = new ThreadStart{start, argument};
    pthread_t created;
    const int error = pthread_create(&created, &attributes, startOwnThread, own);
    pthread_attr_destroy(&attributes);
    if (error) {
        delete own;
        return darwinError(error);
    }
    *thread = created;
    return 0;
}

static int hostCreateThreadOnStack(uintptr_t* thread, void* (*start)(void*), void* argument, void* stack, size_t stackSize)
{
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    int error = pthread_attr_setstack(&attributes, stack, stackSize);
    if (error) {
        pthread_attr_destroy(&attributes);
        return darwinError(error);
    }
    auto* own = new ThreadStart{start, argument};
    pthread_t created;
    error = pthread_create(&created, &attributes, startOwnThread, own);
    pthread_attr_destroy(&attributes);
    if (error) {
        delete own;
        return darwinError(error);
    }
    *thread = created;
    return 0;
}

static int hostJoinThread(uintptr_t thread, void** value)
{
    const int error = pthread_join(thread, value);
    return error ? darwinError(error) : 0;
}

static int hostDetachThread(uintptr_t thread)
{
    const int error = pthread_detach(thread);
    return error ? darwinError(error) : 0;
}

static uintptr_t hostCurrentThread() { return pthread_self(); }
static long hostThreadId() { return gettid(); }
static void hostExitThread(void* value) { pthread_exit(value); }

static int hostSetThreadName(const char* name)
{
    char own[16];
    snprintf(own, sizeof own, "%s", name);
    const int error = pthread_setname_np(pthread_self(), own);
    return error ? darwinError(error) : 0;
}

static int hostThreadName(uintptr_t thread, char* name, size_t size)
{
    const int error = pthread_getname_np(thread, name, size);
    return error ? darwinError(error) : 0;
}

static int hostThreadStack(uintptr_t thread, void** stack, size_t* size)
{
    pthread_attr_t attributes;
    int error = pthread_getattr_np(thread, &attributes);
    if (error)
        return darwinError(error);
    error = pthread_attr_getstack(&attributes, stack, size);
    pthread_attr_destroy(&attributes);
    return error ? darwinError(error) : 0;
}

static int hostIsMainThread()
{
    if (isMain < 0)
        isMain = gettid() == getpid();
    return isMain;
}

static int hostYield() { return sched_yield(); }

static int hostCreateKey(uintptr_t* key, void (*destructor)(void*))
{
    std::lock_guard lock(keyMutex);
    for (int i = FirstKey; i < KeyCount; ++i) {
        if (!keyUsed[i]) {
            keyUsed[i] = true;
            destructors[i] = destructor;
            *key = uintptr_t(i);
            return 0;
        }
    }
    return 35;
}

static int hostDeleteKey(uintptr_t key)
{
    std::lock_guard lock(keyMutex);
    if (key < FirstKey || key >= KeyCount || !keyUsed[key])
        return 22;
    keyUsed[key] = false;
    destructors[key] = nullptr;
    return 0;
}

static void* hostGetSpecific(uintptr_t key) { return key < KeyCount ? slots[key] : nullptr; }

static int hostSetSpecific(uintptr_t key, const void* value)
{
    if (key >= KeyCount)
        return 22;
    slots[key] = const_cast<void*>(value);
    return 0;
}

static void* hostCreateMutex(int recursive)
{
    auto* mutex = static_cast<pthread_mutex_t*>(malloc(sizeof(pthread_mutex_t)));
    pthread_mutexattr_t attributes;
    pthread_mutexattr_init(&attributes);
    if (recursive)
        pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &attributes);
    pthread_mutexattr_destroy(&attributes);
    return mutex;
}

static void hostDestroyMutex(void* mutex)
{
    pthread_mutex_destroy(static_cast<pthread_mutex_t*>(mutex));
    free(mutex);
}

static int hostLockMutex(void* mutex)
{
    const int error = pthread_mutex_lock(static_cast<pthread_mutex_t*>(mutex));
    return error ? darwinError(error) : 0;
}

static int hostTryLockMutex(void* mutex)
{
    const int error = pthread_mutex_trylock(static_cast<pthread_mutex_t*>(mutex));
    return error ? darwinError(error) : 0;
}

static int hostUnlockMutex(void* mutex)
{
    const int error = pthread_mutex_unlock(static_cast<pthread_mutex_t*>(mutex));
    return error ? darwinError(error) : 0;
}

static void* hostCreateCondition()
{
    auto* condition = static_cast<pthread_cond_t*>(malloc(sizeof(pthread_cond_t)));
    pthread_cond_init(condition, nullptr);
    return condition;
}

static void hostDestroyCondition(void* condition)
{
    pthread_cond_destroy(static_cast<pthread_cond_t*>(condition));
    free(condition);
}

static int hostWaitCondition(void* condition, void* mutex)
{
    const int error = pthread_cond_wait(static_cast<pthread_cond_t*>(condition), static_cast<pthread_mutex_t*>(mutex));
    return error ? darwinError(error) : 0;
}

static int hostWaitConditionUntil(void* condition, void* mutex, int64_t seconds, int64_t nanoseconds)
{
    const timespec time = {seconds, nanoseconds};
    const int error = pthread_cond_timedwait(static_cast<pthread_cond_t*>(condition), static_cast<pthread_mutex_t*>(mutex), &time);
    return error ? darwinError(error) : 0;
}

static int hostSignalCondition(void* condition)
{
    const int error = pthread_cond_signal(static_cast<pthread_cond_t*>(condition));
    return error ? darwinError(error) : 0;
}

static int hostBroadcastCondition(void* condition)
{
    const int error = pthread_cond_broadcast(static_cast<pthread_cond_t*>(condition));
    return error ? darwinError(error) : 0;
}

static void hostSetThreadValue(void* value) { threadValue = value; }

// Memory that stays.
static void* hostReserve(size_t size)
{
    static std::mutex mutex;
    static uintptr_t next;
    static uintptr_t end;
    size = (size + 15) & ~size_t(15);
    std::lock_guard lock(mutex);
    if (!next || end - next < size) {
        const size_t block = std::max((size + 0xfffff) & ~size_t(0xfffff), size_t(0x100000));
        void* memory = reserveMemory(block);
        if (memory == reinterpret_cast<void*>(-1))
            return nullptr;
        next = reinterpret_cast<uintptr_t>(memory);
        end = next + block;
    }
    const uintptr_t address = next;
    next += size;
    return reinterpret_cast<void*>(address);
}

// Images.

static int hostImageCount() { return int(Linker::current->infos().size()); }
static const void* hostImage(uint32_t index) { return Linker::current->info(index); }
static void* hostImageSymbol(void* handle, const char* name) { return Linker::current->symbol(handle, name); }

static void hostObserveImages(void (*mapped)(size_t count, const char* const* paths, const void* const* headers),
                              void (*initialized)(const char* path, const void* header), void* unmapped)
{
    Linker::current->observe(mapped, initialized, unmapped);
}

static void* hostOpenImage(const char* path, uint32_t mode, char* error, size_t errorSize)
{
    std::string message;
    void* handle = Linker::current->open(path ? path : "", mode, message);
    if (!handle && errorSize)
        snprintf(error, errorSize, "%s", message.c_str());
    return handle;
}

static bool hostFindSymbol(const void* address, HostSymbol* symbol) { return Linker::current->findSymbol(address, symbol); }
static const void* hostImageContaining(const void* address) { return Linker::current->imageContaining(address); }
static void* hostReadPreferences(const char* name, size_t* size) { return Linker::current->readPreferences(name, size); }
static void hostWritePreferences(const char* name, const void* data, size_t size) { Linker::current->writePreferences(name, data, size); }

// `libraries` has colons between the names; "@root/" is where our own files are.
static void* hostSymbol(const char* libraries, const char* name)
{
    static std::map<std::string, void*> handles;
    static std::mutex mutex;
    std::lock_guard lock(mutex);
    const std::string list = libraries;
    for (size_t start = 0; start <= list.size();) {
        const size_t end = std::min(list.find(':', start), list.size());
        const std::string library(list, start, end - start);
        start = end + 1;
        if (library.empty())
            continue;
        auto handle = handles.find(library);
        if (handle == handles.end()) {
            void* opened = library.rfind("@root/", 0) == 0 ? dlopen((Linker::current->root() + library.substr(5)).c_str(), RTLD_NOW)
                                                           : dlopen(library.c_str(), RTLD_NOW);
            handle = handles.emplace(library, opened).first;
        }
        if (handle->second) {
            if (void* symbol = dlsym(handle->second, name))
                return symbol;
        }
    }
    return nullptr;
}

static void hostLog(const char* identity, int priority, const char* message)
{
    static std::string name;
    static std::once_flag once;
    std::call_once(once, [&] {
        name = identity ? identity : "reef";
        openlog(name.c_str(), LOG_PID, LOG_USER);
    });
    syslog(priority & 7, "%s", message);
}

HostServices* hostServices(int argumentCount, char** arguments, char** environment)
{
    static std::string token;
    if (const char* value = getenv("XDG_ACTIVATION_TOKEN")) {
        token = value;
        unsetenv("XDG_ACTIVATION_TOKEN");
        services.activationToken = token.c_str();
    }
    services.argumentCount = argumentCount;
    services.arguments = arguments;
    services.environment = environment;
    services.exit = hostExit;
    services.abort = hostAbort;
    services.fail = hostFail;
    services.getpid = hostGetpid;
    services.random = hostRandom;
    services.malloc = malloc;
    services.calloc = calloc;
    services.realloc = realloc;
    services.free = free;
    services.allocationSize = hostAllocationSize;
    services.posixMemalign = hostPosixMemalign;
    findHeap();
    services.pointerSize = hostPointerSize;
    services.allocated = hostAllocated;
    services.memoryFile = hostMemoryFile;
    services.trim = hostTrim;
    services.manyAllocations = hostManyAllocations;
    services.createThreadOnStack = hostCreateThreadOnStack;
    services.threadStack = hostThreadStack;
    services.memcpy = memcpy;
    services.memmove = memmove;
    services.memset = memset;
    services.memcmp = memcmp;
    services.memchr = hostMemchr;
    services.strlen = strlen;
    services.strcmp = strcmp;
    services.strncmp = strncmp;
    services.strchr = hostStrchr;
    services.strrchr = hostStrrchr;
    services.strstr = hostStrstr;
    services.strcasecmp = strcasecmp;
    services.strncasecmp = strncasecmp;
    services.strtod = strtod;
    services.formatDouble = hostFormatDouble;
    services.read = hostRead;
    services.write = hostWrite;
    services.open = hostOpen;
    services.close = hostClose;
    services.lseek = hostLseek;
    services.isatty = hostIsatty;
    services.clock = hostClockGettime;
    services.monotonic = hostMonotonic;
    services.nanosleep = hostNanosleep;
    services.createThread = hostCreateThread;
    services.joinThread = hostJoinThread;
    services.detachThread = hostDetachThread;
    services.currentThread = hostCurrentThread;
    services.threadId = hostThreadId;
    services.exitThread = hostExitThread;
    services.setThreadName = hostSetThreadName;
    services.threadName = hostThreadName;
    services.isMainThread = hostIsMainThread;
    services.yield = hostYield;
    services.createKey = hostCreateKey;
    services.deleteKey = hostDeleteKey;
    services.getSpecific = hostGetSpecific;
    services.setSpecific = hostSetSpecific;
    services.createMutex = hostCreateMutex;
    services.destroyMutex = hostDestroyMutex;
    services.lockMutex = hostLockMutex;
    services.tryLockMutex = hostTryLockMutex;
    services.unlockMutex = hostUnlockMutex;
    services.createCondition = hostCreateCondition;
    services.destroyCondition = hostDestroyCondition;
    services.waitCondition = hostWaitCondition;
    services.waitConditionUntil = hostWaitConditionUntil;
    services.signalCondition = hostSignalCondition;
    services.broadcastCondition = hostBroadcastCondition;
    services.imageCount = hostImageCount;
    services.image = hostImage;
    services.imageSymbol = hostImageSymbol;
    services.observeImages = hostObserveImages;
    services.openImage = hostOpenImage;
    services.findSymbol = hostFindSymbol;
    services.imageContaining = hostImageContaining;
    services.readPreferences = hostReadPreferences;
    services.writePreferences = hostWritePreferences;
    services.hostSymbol = hostSymbol;
    services.threadValueOffset = reinterpret_cast<char*>(&threadValue) - static_cast<char*>(__builtin_thread_pointer());
    services.setThreadValue = hostSetThreadValue;
    services.reserve = hostReserve;
    addFileServices(services);
    addSignalServices(services);
    addSystemServices(services);
    addSocketServices(services);
    addBonjourServices(services);
    addProcessServices(services);
    addMachServices(services);
    addSandboxServices(services);
    addRealtimeServices(services);
    services.log = hostLog;
    setUpSignalStack();
    setUpThread();
    services.taskFunctions[0] = reinterpret_cast<void*>(taskFunction0);
    services.taskFunctions[1] = reinterpret_cast<void*>(taskFunction1);
    services.taskFunctions[2] = reinterpret_cast<void*>(taskFunction2);
    services.taskFunctions[3] = reinterpret_cast<void*>(taskFunction3);
    setUpTask();
    setUpCrashHandler();
    allowSandboxSignal();
    return &services;
}

} // namespace cutefish::reef

// Threads of the host's libraries get the same set-up.
extern "C" int pthread_create(pthread_t* thread, const pthread_attr_t* attributes, void* (*start)(void*), void* argument) noexcept
{
    using namespace cutefish::reef;
    static const auto real = reinterpret_cast<decltype(&pthread_create)>(dlsym(RTLD_NEXT, "pthread_create"));
    auto own = std::make_unique<ThreadStart>(ThreadStart{start, argument});
    const int error = real(thread, attributes, startThread, own.get());
    if (error == 0)
        own.release();
    return error;
}
