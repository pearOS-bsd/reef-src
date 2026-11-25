// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What the programs' system library asks of the host.  The table is an interface with libSystem: its order and types are fixed.
// Errors are negative Darwin error numbers.

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <ctime>

struct iovec;

namespace cutefish::reef {

struct HostStat
{
    uint64_t device;
    uint64_t inode;
    uint64_t links;
    uint64_t special;
    int64_t size;
    int64_t blocks;
    uint32_t mode;
    uint32_t user;
    uint32_t group;
    uint32_t blockSize;
    int64_t accessed[2];
    int64_t modified[2];
    int64_t changed[2];
    int64_t born[2];
    uint32_t flags;
};

struct HostFileSystem
{
    uint64_t blocks;
    uint64_t free;
    uint64_t available;
    uint64_t files;
    uint64_t freeFiles;
    uint32_t blockSize;
    uint32_t transferSize;
    uint32_t flags;
    int32_t id[2];
    char type[16];
    char mount[1024];
    char source[1024];
};

struct HostEntry
{
    uint64_t inode;
    int64_t offset;
    uint8_t type;
    char name[1024];
};

struct HostTime
{
    int64_t seconds;
    int64_t nanoseconds;
};

struct HostLock
{
    int64_t start;
    int64_t length;
    int32_t process;
    int16_t type;
    int16_t whence;
};

// Where an address is: its image, the image's base, the symbol and its address.
struct HostSymbol
{
    const char* path;
    const void* base;
    const char* name;
    const void* address;
};

struct HostServices
{
    int argumentCount;
    char** arguments;
    char** environment;
    char* executablePath;
    const char* programName;
    void (*exit)(int status);
    void (*abort)(const char* message);
    int (*getpid)();
    void (*random)(void* buffer, size_t size);
    void* (*malloc)(size_t size);
    void* (*calloc)(size_t count, size_t size);
    void* (*realloc)(void* memory, size_t size);
    void (*free)(void* memory);
    size_t (*allocationSize)(void* memory);
    int (*posixMemalign)(void** memory, size_t alignment, size_t size);
    void* (*memcpy)(void* to, const void* from, size_t size);
    void* (*memmove)(void* to, const void* from, size_t size);
    void* (*memset)(void* memory, int value, size_t size);
    int (*memcmp)(const void* a, const void* b, size_t size);
    const void* (*memchr)(const void* memory, int value, size_t size);
    size_t (*strlen)(const char* text);
    int (*strcmp)(const char* a, const char* b);
    int (*strncmp)(const char* a, const char* b, size_t size);
    const char* (*strchr)(const char* text, int value);
    const char* (*strrchr)(const char* text, int value);
    const char* (*strstr)(const char* text, const char* part);
    int (*strcasecmp)(const char* a, const char* b);
    int (*strncasecmp)(const char* a, const char* b, size_t size);
    double (*strtod)(const char* text, char** end);
    int (*formatDouble)(char* buffer, size_t size, const char* format, double value);
    long (*read)(int fd, void* buffer, size_t size);
    long (*write)(int fd, const void* buffer, size_t size);
    int (*open)(const char* path, int flags, uint32_t mode);
    int (*close)(int fd);
    int64_t (*lseek)(int fd, int64_t offset, int origin);
    int (*isatty)(int fd);
    int (*clock)(int clock, int64_t* seconds, int64_t* nanoseconds);
    uint64_t (*monotonic)();
    int (*nanosleep)(int64_t seconds, int64_t nanoseconds);
    int (*createThread)(uintptr_t* thread, void* (*start)(void*), void* argument, size_t stackSize);
    int (*joinThread)(uintptr_t thread, void** value);
    int (*detachThread)(uintptr_t thread);
    uintptr_t (*currentThread)();
    long (*threadId)();
    void (*exitThread)(void* value);
    int (*killThread)(uintptr_t thread, int signal);
    int (*setThreadName)(const char* name);
    int (*threadName)(uintptr_t thread, char* name, size_t size);
    int (*isMainThread)();
    int (*yield)();
    int (*createKey)(uintptr_t* key, void (*destructor)(void*));
    int (*deleteKey)(uintptr_t key);
    void* (*getSpecific)(uintptr_t key);
    int (*setSpecific)(uintptr_t key, const void* value);
    void* (*createMutex)(int recursive);
    void (*destroyMutex)(void* mutex);
    int (*lockMutex)(void* mutex);
    int (*tryLockMutex)(void* mutex);
    int (*unlockMutex)(void* mutex);
    void* (*createCondition)();
    void (*destroyCondition)(void* condition);
    int (*waitCondition)(void* condition, void* mutex);
    int (*waitConditionUntil)(void* condition, void* mutex, int64_t seconds, int64_t nanoseconds);
    int (*signalCondition)(void* condition);
    int (*broadcastCondition)(void* condition);
    int (*imageCount)();
    const void* (*image)(uint32_t index);
    void* (*imageSymbol)(void* handle, const char* name);
    void (*observeImages)(void (*mapped)(size_t count, const char* const* paths, const void* const* headers),
                          void (*initialized)(const char* path, const void* header), void* unmapped);
    void* (*openImage)(const char* path, uint32_t mode, char* error, size_t errorSize);
    bool (*findSymbol)(const void* address, HostSymbol* symbol);
    const void* (*imageContaining)(const void* address);
    void* (*readPreferences)(const char* name, size_t* size);
    void (*writePreferences)(const char* name, const void* data, size_t size);
    void* (*hostSymbol)(const char* libraries, const char* name);
    long threadValueOffset;
    void (*setThreadValue)(void* value);
    void* (*reserve)(size_t size);
    void* (*mmap)(void* address, size_t size, int protection, int flags, int fd, int64_t offset, int* error);
    int (*munmap)(void* address, size_t size);
    int (*mprotect)(void* address, size_t size, int protection);
    int (*msync)(void* address, size_t size, int flags);
    int (*madvise)(void* address, size_t size, int advice);
    int (*stat)(int directory, const char* path, uint32_t flags, HostStat* out);
    int (*statfs)(const char* path, int fd, HostFileSystem* out);
    int (*access)(int directory, const char* path, int mode, uint32_t flags);
    int (*mkdir)(int directory, const char* path, uint32_t mode);
    int (*unlink)(int directory, const char* path, uint32_t flags);
    int (*rename)(int directory, const char* from, int toDirectory, const char* to);
    long (*readlink)(int directory, const char* path, char* buffer, size_t size);
    int (*symlink)(const char* target, int directory, const char* path);
    int (*chmod)(int directory, const char* path, uint32_t mode, uint32_t flags);
    int (*link)(int directory, const char* from, int toDirectory, const char* to, uint32_t flags);
    int (*chown)(int directory, const char* path, uint32_t user, uint32_t group, uint32_t flags);
    int (*utimes)(int directory, const char* path, const HostTime* times, uint32_t flags);
    int (*getcwd)(char* buffer, size_t size);
    int (*chdir)(const char* path, int fd);
    int (*realpath)(const char* path, char* out);
    int (*openat)(int directory, const char* path, int flags, int mode);
    int (*fcntl)(int fd, int command, uint64_t argument);
    int (*dup)(int fd);
    int (*dup2)(int fd, int other);
    int (*pipe)(int* fds);
    int (*ftruncate)(int fd, int64_t size);
    int (*fsync)(int fd);
    long (*pread)(int fd, void* buffer, size_t size, int64_t offset);
    long (*pwrite)(int fd, const void* buffer, size_t size, int64_t offset);
    void* (*opendir)(const char* path, int fd, int* error);
    int (*readdir)(void* stream, HostEntry* entry);
    int (*closedir)(void* stream);
    void (*seekdir)(void* stream, long position);
    long (*telldir)(void* stream);
    int (*dirfd)(void* stream);
    void* unknown388[44];
    void (*log)(const char* identity, int priority, const char* message);
    void* unknown4f0[6];
    long (*getxattr)(const char* path, int fd, const char* name, void* value, size_t size, uint32_t options);
    int (*setxattr)(const char* path, int fd, const char* name, const void* value, size_t size, uint32_t options);
    int (*removexattr)(const char* path, int fd, const char* name, uint32_t options);
    long (*listxattr)(const char* path, int fd, char* names, size_t size, uint32_t options);
    void* unknown540[2];
    int (*renamex)(int directory, const char* from, int toDirectory, const char* to, uint32_t flags);
    int (*inotifyInit)(int flags);
    int (*inotifyAddWatch)(int fd, const char* path, uint32_t mask);
    int (*inotifyRemoveWatch)(int fd, int watch);
    void* unknown570[2];
    const char* reefPath;
    uint64_t* launchItems;
    void* unknown590[2];
    int (*darwinError)(int error);
    int (*createThreadOnStack)(uintptr_t* thread, void* (*start)(void*), void* argument, void* stack, size_t stackSize);
    int (*threadStack)(uintptr_t thread, void** stack, size_t* size);
    int (*mincore)(void* address, size_t size, unsigned char* out);
    void (*allocated)(size_t* used, size_t* total);
    bool (*readable)(uintptr_t address, size_t size);
    void (*fail)(const char* message);
    long (*pathconf)(const char* path, int fd, int name);
    void* unknown5e0;
    int (*getdirentries)(int fd, HostEntry* entries, int count);
    const char* activationToken;
    void* unknown5f8;
    void* bonjourRegister;
    void* bonjourSocket;
    void* bonjourNext;
    void* bonjourDestroy;
    void* bonjourBrowse;
    void* bonjourResolve;
    void* bonjourQuery;
    void (*trim)();
    void (*manyAllocations)(int more);
    void* unknown648[3];
    int (*memoryFile)(const char* name);
    void* unknown668[3];
    size_t (*pointerSize)(const void* memory);
    void* unknown688;
    void* taskFunctions[4];
};

// host.cpp
HostServices* hostServices(int argumentCount, char* executablePath, const char* programName);
int darwinError(int error);
int linuxError(int error);
int hostOpenFlags(int flags);
int darwinOpenFlags(int flags);
bool hostClock(int clock, clockid_t* host);
void tuneAllocator();
// The thread's values of the keys, and the register that points at them.
void setUpThread();
// The host's path of a path of the program; `follow` also for the last component.
std::string hostPath(const char* path, bool follow = true);

// sandbox.cpp
void allowSandboxSignal();
// The error of a call that the host refused: what the sandbox refuses is EPERM.
int sandboxError(int error, int directory, const char* path, int access);

// sockets.cpp
void forgetSocket(int fd);
bool socketNoSignal(int fd);
void setSocketNoSignal(int fd, bool on);
long withoutSignal(const std::function<long()>& call);

// signals.cpp
void setUpSignalStack();
void setUpCrashHandler();

// unsupported.cpp
void showUnsupported(bool running, const std::string& reason);
// The ports given with --exception-ports=, for the exception server.
void setExceptionPorts(const char* ports);
const char* exceptionPorts();
// The program that the dialog about unsupported programs names.
void setProgramPath(const std::string& path);
// reef --mach-broker: the server of Mach ports of the session.
int machBroker();

// task.cpp
void setUpTask();
int taskFunction0();
int taskFunction1();
int taskFunction2();
int taskFunction3();

void addFileServices(HostServices& services);
void addSignalServices(HostServices& services);
void addSystemServices(HostServices& services);
void addSocketServices(HostServices& services);
void addBonjourServices(HostServices& services);
void addProcessServices(HostServices& services);
void addMachServices(HostServices& services);
void addSandboxServices(HostServices& services);
void addRealtimeServices(HostServices& services);

} // namespace cutefish::reef
