// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What debuggers need of a process: its threads stop for them and leave their registers in shared memory.

#include "host.h"
#include "task.h"

#include <cerrno>
#include <csignal>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <unistd.h>

namespace cutefish::reef {

static const char* ports;
static Task* task;

// The signal that stops a thread for a debugger.
static void stopThread(int signal, siginfo_t* information, void* context)
{
    Task* shared = task;
    if (!shared)
        return;
    const int error = errno;
    const int thread = int(syscall(SYS_gettid));
    const uint32_t count = shared->threadCount;
    for (uint32_t i = 0; i < count && i < Task::MaxThreads; i++) {
        TaskThread& entry = shared->threads[i];
        if (entry.thread == thread) {
            saveThreadState(context, information, &entry.state, &entry.floats);
            entry.captures++;
            break;
        }
    }
    sigset_t mask;
    syscall(SYS_rt_sigprocmask, 0, nullptr, &mask, 8);
    sigdelset(&mask, signal);
    while (shared->suspended > 0)
        sigsuspend(&mask);
    errno = error;
}

static Task* createTask()
{
    const int fd = memfd_create("reef-task", MFD_CLOEXEC);
    if (fd < 0)
        return nullptr;
    if (ftruncate(fd, sizeof(Task)) != 0) {
        close(fd);
        return nullptr;
    }
    void* memory = mmap(nullptr, sizeof(Task), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (memory == MAP_FAILED)
        return nullptr;
    Task* shared = static_cast<Task*>(memory);
    shared->magic = Task::Magic;
    return shared;
}

void setUpTask()
{
    task = createTask();
    struct sigaction action = {};
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = stopThread;
    action.sa_flags = SA_SIGINFO | SA_RESTART | SA_ONSTACK;
    sigaction(suspendSignal(), &action, nullptr);
}

// After fork the memory is the parent's: the child gets its own in the same place.
void renewTask()
{
    if (!task)
        return;
    const uint64_t first = task->unknown8[0];
    const uint64_t second = task->unknown8[1];
    const int fd = memfd_create("reef-task", MFD_CLOEXEC);
    if (fd < 0)
        return;
    if (ftruncate(fd, sizeof(Task)) != 0) {
        close(fd);
        return;
    }
    void* memory = mmap(task, sizeof(Task), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    close(fd);
    if (memory == MAP_FAILED)
        return;
    Task* shared = static_cast<Task*>(memory);
    shared->magic = Task::Magic;
    shared->unknown8[0] = first;
    shared->unknown8[1] = second;
}

void* taskMemory()
{
    return task;
}

long accessMemory(int process, void* remote, void* local, size_t size, int write)
{
    const iovec here = {local, size};
    const iovec there = {remote, size};
    const ssize_t done = write ? process_vm_writev(process, &here, 1, &there, 1, 0) : process_vm_readv(process, &here, 1, &there, 1, 0);
    if (done < 0)
        return -darwinError(errno);
    return done;
}

void setExceptionPorts(const char* names)
{
    ports = names;
}

const char* exceptionPorts()
{
    return ports;
}

int signalThread(int process, int thread)
{
    if (syscall(SYS_tgkill, process, thread, suspendSignal()) == 0)
        return 0;
    return -darwinError(errno);
}

} // namespace cutefish::reef
