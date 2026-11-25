// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Signals with Darwin's numbers, structures and masks; the report about a crash, for the terminal and the journal; signalfd.

#include "host.h"
#include "image.h"
#include "linker.h"
#include "thread_state.h"

#include <atomic>
#include <cerrno>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <mutex>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <ucontext.h>
#include <unistd.h>
#include <vector>

namespace cutefish::reef {

// struct __siginfo of Darwin.
struct DarwinSiginfo
{
    int signal;
    int error;
    int code;
    int process;
    uint32_t user;
    int status;
    void* address;
    void* value;
    long band;
    unsigned long padding[7];
};

// struct __darwin_sigaltstack.
struct DarwinStack
{
    void* memory;
    size_t size;
    int flags;
};

// struct __darwin_ucontext.
struct DarwinContext
{
    int onStack;
    uint32_t mask;
    DarwinStack stack;
    DarwinContext* link;
    size_t size;
    MachineContext* context;
};

struct Action
{
    uint64_t handler;
    uint32_t mask;
    uint32_t flags;
};

struct SignalFd
{
    int fd;
    uint32_t mask;
};

// The stack the handlers of a thread run on.
struct SignalStack
{
    SignalStack()
    {
        memory = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
        if (memory == MAP_FAILED) {
            memory = nullptr;
            return;
        }
        stack_t stack = {};
        stack.ss_sp = memory;
        stack.ss_size = size;
        sigaltstack(&stack, nullptr);
    }

    ~SignalStack()
    {
        if (!memory)
            return;
        stack_t stack = {};
        stack.ss_flags = SS_DISABLE;
        sigaltstack(&stack, nullptr);
        munmap(memory, size);
    }

    static constexpr size_t size = 0x40000;
    void* memory = nullptr;
};

static constexpr int crashSignals[] = {SIGSEGV, SIGBUS, SIGILL, SIGFPE, SIGTRAP};

// The Linux number of each signal of Darwin; 0 for SIGEMT and SIGINFO, which Linux does not have.
static constexpr int hostSignals[32] = {
    0,       SIGHUP,  SIGINT,  SIGQUIT, SIGILL,  SIGTRAP, SIGABRT,  0,       SIGFPE,    SIGKILL, SIGBUS,
    SIGSEGV, SIGSYS,  SIGPIPE, SIGALRM, SIGTERM, SIGURG,  SIGSTOP,  SIGTSTP, SIGCONT,   SIGCHLD, SIGTTIN,
    SIGTTOU, SIGIO,   SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF, SIGWINCH, 0,     SIGUSR1,   SIGUSR2,
};

static constexpr int floatCodes[8] = {7, 8, 1, 2, 3, 4, 5, 6};

static std::atomic<ExceptionHandler> exceptionHandler;
static char message[0x20000];
static char modules[0x18000];
static size_t symbolsLength;
static char symbols[0x4000];
static std::atomic<uint32_t> signalFdMask;
static std::vector<SignalFd> signalFds;
static std::mutex signalFdMutex;
static Action actions[32];

static thread_local DarwinStack alternateStack = {nullptr, 0, 4};
static thread_local SignalStack signalStack;

static int hostRaise(int signal)
{
    if (signal < 1 || signal > 31)
        return -22;
    if (hostSignals[signal])
        return raise(hostSignals[signal]) == 0 ? 0 : -22;
    if (signal == 7 || signal == 29)
        return 0;
    return -22;
}

// A field of a journal entry: the name, the size and the bytes.
static void appendField(size_t& length, const char* name, const void* value, size_t size)
{
    const size_t nameLength = strlen(name);
    if (length + nameLength + size + 10 > sizeof message)
        return;
    memcpy(message + length, name, nameLength);
    length += nameLength;
    message[length++] = '\n';
    const uint64_t count = size;
    memcpy(message + length, &count, sizeof count);
    length += sizeof count;
    memcpy(message + length, value, size);
    length += size;
    message[length++] = '\n';
}

static void append(char* buffer, size_t capacity, size_t& length, const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    const int written = vsnprintf(buffer + length, capacity - length, format, arguments);
    va_end(arguments);
    if (written > 0 && length + size_t(written) < capacity)
        length += size_t(written);
    else
        buffer[length] = '\0';
}

static void printFrame(int index, uint64_t address)
{
    HostSymbol symbol = {};
    Dl_info host = {};
    const char* path = nullptr;
    const void* base = nullptr;
    const char* name = nullptr;
    const void* start = nullptr;
    if (Linker::current && Linker::current->findSymbolUnlocked(reinterpret_cast<const void*>(address), &symbol)) {
        path = symbol.path;
        base = symbol.base;
        name = symbol.name;
        start = symbol.name ? symbol.address : symbol.base;
    } else if (dladdr(reinterpret_cast<const void*>(address), &host) && host.dli_fname) {
        Dl_info self = {};
        path = dladdr(reinterpret_cast<const void*>(printFrame), &self) && self.dli_fbase == host.dli_fbase ? "reef" : host.dli_fname;
        base = host.dli_fbase;
        name = host.dli_sname;
        start = host.dli_sname ? host.dli_saddr : host.dli_fbase;
    }
    if (!base) {
        fprintf(stderr, "  #%-2d 0x%016lx ?\n", index, address);
        return;
    }
    const char* slash = strrchr(path, '/');
    fprintf(stderr, "  #%-2d 0x%016lx %s %s + %lu (image +0x%lx)\n", index, address, slash ? slash + 1 : path, name ? name : "",
            address - reinterpret_cast<uintptr_t>(start), address - reinterpret_cast<uintptr_t>(base));
}

static void setExceptionHandler(ExceptionHandler handler)
{
    exceptionHandler = handler;
}

static unsigned hostAlarm(unsigned seconds)
{
    return alarm(seconds);
}

static int hostKillThread(uintptr_t thread, int signal)
{
    int host = 0;
    if (signal != 0) {
        if (signal < 1 || signal > 31)
            return 22;
        host = hostSignals[signal];
        if (!host)
            return signal == 7 || signal == 29 ? 0 : 22;
    }
    const int error = pthread_kill(pthread_t(thread), host);
    if (error)
        return darwinError(error);
    return 0;
}

static int hostKill(int process, int signal)
{
    int host = 0;
    if (signal != 0) {
        if (signal < 1 || signal > 31)
            return -22;
        host = hostSignals[signal];
        if (!host) {
            if (signal == 7 || signal == 29)
                return 0;
            return -22;
        }
    }
    if (kill(process, host) < 0)
        return -darwinError(errno);
    return 0;
}

// A call of a function that is missing: with stubs that return zero, it returns.
static bool returnFromMissing(int signal, ucontext_t* context)
{
    if (signal != SIGSEGV)
        return false;
    greg_t* registers = context->uc_mcontext.gregs;
    const char* function = missingFunction(uintptr_t(registers[REG_RIP]));
    if (!function)
        return false;
    char text[512];
    if (stubsReturnZero()) {
        snprintf(text, sizeof text, "reef: called missing function %s, returning 0\n", function);
        (void)!write(2, text, strlen(text));
        greg_t* stack = reinterpret_cast<greg_t*>(registers[REG_RSP]);
        registers[REG_RSP] += 8;
        registers[REG_RIP] = *stack;
        registers[REG_RDX] = 0;
        registers[REG_RAX] = 0;
        return true;
    }
    snprintf(text, sizeof text, "reef: called missing function %s\n", function);
    (void)!write(2, text, strlen(text));
    return false;
}

int darwinSignal(int signal)
{
    for (int i = 1; i < 32; i++) {
        if (hostSignals[i] == signal)
            return i;
    }
    return 0;
}

static void readContext(MachineContext& state, const siginfo_t* information, const ucontext_t* context)
{
    const greg_t* registers = context->uc_mcontext.gregs;
    memset(&state, 0, sizeof state);
    state.exception.trap = uint16_t(registers[REG_TRAPNO]);
    state.exception.error = uint32_t(registers[REG_ERR]);
    state.exception.address = reinterpret_cast<uint64_t>(information->si_addr);
    state.thread.rax = registers[REG_RAX];
    state.thread.rbx = registers[REG_RBX];
    state.thread.rcx = registers[REG_RCX];
    state.thread.rdx = registers[REG_RDX];
    state.thread.rdi = registers[REG_RDI];
    state.thread.rsi = registers[REG_RSI];
    state.thread.rbp = registers[REG_RBP];
    state.thread.rsp = registers[REG_RSP];
    state.thread.r8 = registers[REG_R8];
    state.thread.r9 = registers[REG_R9];
    state.thread.r10 = registers[REG_R10];
    state.thread.r11 = registers[REG_R11];
    state.thread.r12 = registers[REG_R12];
    state.thread.r13 = registers[REG_R13];
    state.thread.r14 = registers[REG_R14];
    state.thread.r15 = registers[REG_R15];
    state.thread.rip = registers[REG_RIP];
    state.thread.rflags = registers[REG_EFL];
    state.thread.cs = uint16_t(registers[REG_CSGSFS]);
    if (context->uc_mcontext.fpregs)
        memcpy(state.floats.fpu, context->uc_mcontext.fpregs, sizeof state.floats.fpu);
}

static void writeContext(const MachineContext& state, ucontext_t* context)
{
    greg_t* registers = context->uc_mcontext.gregs;
    registers[REG_RAX] = state.thread.rax;
    registers[REG_RBX] = state.thread.rbx;
    registers[REG_RCX] = state.thread.rcx;
    registers[REG_RDX] = state.thread.rdx;
    registers[REG_RDI] = state.thread.rdi;
    registers[REG_RSI] = state.thread.rsi;
    registers[REG_RBP] = state.thread.rbp;
    registers[REG_RSP] = state.thread.rsp;
    registers[REG_R8] = state.thread.r8;
    registers[REG_R9] = state.thread.r9;
    registers[REG_R10] = state.thread.r10;
    registers[REG_R11] = state.thread.r11;
    registers[REG_R12] = state.thread.r12;
    registers[REG_R13] = state.thread.r13;
    registers[REG_R14] = state.thread.r14;
    registers[REG_R15] = state.thread.r15;
    registers[REG_RIP] = state.thread.rip;
    registers[REG_EFL] = state.thread.rflags;
    if (context->uc_mcontext.fpregs)
        memcpy(context->uc_mcontext.fpregs, state.floats.fpu, sizeof state.floats.fpu);
}

static bool raiseException(int signal, const siginfo_t* information, ucontext_t* context, bool second)
{
    const ExceptionHandler handler = exceptionHandler;
    if (!handler || !context)
        return false;
    MachineContext state;
    readContext(state, information, context);
    if (!handler(darwinSignal(signal), information->si_code, state.exception.address, &state.thread, second) || second)
        return false;
    writeContext(state, context);
    return true;
}

static int hostSigaltstack(const DarwinStack* stack, DarwinStack* old)
{
    if (!(alternateStack.flags & 4)) {
        stack_t current = {};
        if (sigaltstack(nullptr, &current) == 0 && (current.ss_flags & SS_ONSTACK)) {
            if (old) {
                *old = alternateStack;
                old->flags |= 1;
            }
            if (!stack)
                return 0;
            if (stack->flags & ~4)
                return -22;
            return -1;
        }
    }
    if (old)
        *old = alternateStack;
    if (stack) {
        if (stack->flags & ~4)
            return -22;
        stack_t host = {};
        if (stack->flags == 0) {
            if (stack->size < 0x8000)
                return -12;
            host.ss_sp = stack->memory;
            host.ss_size = stack->size;
        } else {
            host.ss_sp = signalStack.memory;
            host.ss_size = SignalStack::size;
        }
        if (host.ss_sp && sigaltstack(&host, nullptr) < 0)
            return -darwinError(errno);
        alternateStack = *stack;
    }
    return 0;
}

static uint32_t darwinMask(const sigset_t* set)
{
    uint32_t mask = 0;
    for (int signal = 1; signal < 32; signal++) {
        if (hostSignals[signal] && sigismember(set, hostSignals[signal]) == 1)
            mask |= 1u << (signal - 1);
    }
    return mask;
}

static int hostSigpending(uint32_t* set)
{
    sigset_t pending;
    if (sigpending(&pending) < 0)
        return -darwinError(errno);
    *set = darwinMask(&pending);
    return 0;
}

static const char* signalName(int signal)
{
    switch (signal) {
    case SIGSEGV:
        return "SIGSEGV";
    case SIGBUS:
        return "SIGBUS";
    case SIGILL:
        return "SIGILL";
    case SIGFPE:
        return "SIGFPE";
    }
    return "SIGTRAP";
}

static void appendSymbol(uint64_t address)
{
    HostSymbol symbol = {};
    if (Linker::current && Linker::current->findSymbolUnlocked(reinterpret_cast<const void*>(address), &symbol) && symbol.name &&
        symbol.address != symbol.base)
        append(symbols, sizeof symbols, symbolsLength, "%lx %s\n", address, symbol.name);
}

static bool readMemory(uint64_t address, void* buffer, size_t size)
{
    iovec local = {buffer, size};
    iovec remote = {reinterpret_cast<void*>(address), size};
    return process_vm_readv(getpid(), &local, 1, &remote, 1, 0) == ssize_t(size);
}

// What is known about a crash: on the terminal, and as an entry of the journal that the crash dialog reads.
static void reportCrash(int signal, const void* address, const ucontext_t* context)
{
    const greg_t* registers = context->uc_mcontext.gregs;
    uint64_t frame = uint64_t(registers[REG_RBP]);
    const uint64_t instruction = uint64_t(registers[REG_RIP]);
    fprintf(stderr, "reef: crashed with %s at address 0x%lx\n", signalName(signal), reinterpret_cast<unsigned long>(address));
    symbolsLength = 0;
    symbols[0] = '\0';
    printFrame(0, instruction);
    appendSymbol(instruction);
    uint64_t low = 0;
    uint64_t high = 0;
    pthread_attr_t attributes;
    if (pthread_getattr_np(pthread_self(), &attributes) == 0) {
        void* memory = nullptr;
        size_t size = 0;
        pthread_attr_getstack(&attributes, &memory, &size);
        low = reinterpret_cast<uint64_t>(memory);
        high = low + size;
        pthread_attr_destroy(&attributes);
    }
    for (int index = 1; index < 64 && frame >= low && frame + 16 <= high && (frame & 7) == 0; index++) {
        const uint64_t* words = reinterpret_cast<const uint64_t*>(frame);
        if (!words[1])
            break;
        printFrame(index, words[1]);
        appendSymbol(words[1]);
        if (words[0] <= frame)
            break;
        frame = words[0];
    }
    size_t modulesLength = 0;
    if (Linker::current) {
        for (const std::unique_ptr<Image>& image : Linker::current->m_images)
            append(modules, sizeof modules, modulesLength, "%lx %lx %s\n", image->reservation, image->reservation + image->reservationSize,
                   image->path.c_str());
    }
    char text[64];
    snprintf(text, sizeof text, "reef: crashed with %s", signalName(signal));
    size_t length = 0;
    appendField(length, "MESSAGE_ID", "af9beb45ed294df2905ec4ec78887192", 32);
    appendField(length, "PRIORITY", "4", 1);
    appendField(length, "SYSLOG_IDENTIFIER", "reef", 4);
    appendField(length, "MESSAGE", text, strlen(text));
    appendField(length, "CUTEFISH_CRASH_SYMBOLS", symbols, symbolsLength);
    appendField(length, "CUTEFISH_CRASH_MODULES", modules, modulesLength);
    const int journal = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (journal >= 0) {
        sockaddr_un target = {};
        target.sun_family = AF_UNIX;
        strcpy(target.sun_path, "/run/systemd/journal/socket");
        sendto(journal, message, length, MSG_NOSIGNAL, reinterpret_cast<const sockaddr*>(&target), sizeof target);
        close(journal);
    }
    if (!haveStubs())
        return;
    // A program that runs with missing functions: more to look at.
    static const char* const names[] = {"r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15", "rdi",
                                        "rsi", "rbp", "rbx", "rdx", "rax", "rcx", "rsp", "rip"};
    for (unsigned i = 0; i < 17; i++)
        fprintf(stderr, "%s%s=0x%llx", i == 0 ? "  " : i % 6 == 0 ? "\n  " : " ", names[i], registers[i]);
    fprintf(stderr, "\n");
    for (int i = 0; i < 15; i++) {
        const uint64_t value = uint64_t(registers[i]);
        uint64_t words[4];
        if (value < 0x10000 || !readMemory(value, words, sizeof words))
            continue;
        fprintf(stderr, "  [%s] %lx %lx %lx %lx", names[i], words[0], words[1], words[2], words[3]);
        uint64_t before[4];
        if (words[0] >= 0x10000 && readMemory(words[0] - 16, before, sizeof before))
            fprintf(stderr, "  [*-16] %lx %lx | %lx %lx", before[0], before[1], before[2], before[3]);
        fprintf(stderr, "\n");
    }
    const uint64_t stack = uint64_t(registers[REG_RSP]);
    if (stack >= low && stack < high) {
        fprintf(stderr, "  stack words pointing into images:\n");
        int found = 0;
        for (uint64_t at = stack; at + 8 <= high && found < 40 && at < stack + 0x2000; at += 8) {
            const uint64_t value = *reinterpret_cast<const uint64_t*>(at);
            HostSymbol symbol = {};
            if (value >= 0x1000 && Linker::current && Linker::current->findSymbolUnlocked(reinterpret_cast<const void*>(value), &symbol))
                printFrame(100 + found++, value);
        }
    }
}

static bool crashSignal(int signal)
{
    return signal == SIGSEGV || signal == SIGBUS || signal == SIGILL || signal == SIGFPE || signal == SIGTRAP;
}

// Runs the handler the program set, as Darwin would.
static void deliver(int signal, siginfo_t* information, void* context)
{
    ucontext_t* host = static_cast<ucontext_t*>(context);
    if (haveStubs() && crashSignal(signal) && actions[darwinSignal(signal)].handler > 1)
        reportCrash(signal, information->si_addr, host);
    const int error = errno;
    const int darwin = darwinSignal(signal);
    if (signalFdMask >> (darwin - 1) & 1) {
        signalfd_siginfo record = {};
        record.ssi_signo = uint32_t(darwin);
        if (information) {
            record.ssi_code = information->si_code;
            record.ssi_pid = uint32_t(information->si_pid);
            record.ssi_uid = information->si_uid;
        }
        for (const SignalFd& entry : signalFds) {
            if (entry.mask & 1u << (darwin - 1))
                (void)!write(entry.fd, &record, sizeof record);
        }
    }
    const uint64_t handler = actions[darwin].handler;
    const uint32_t flags = actions[darwin].flags;
    if (flags & 4)
        actions[darwin] = {};
    if (handler == 0) {
        if (signalFdMask >> (darwin - 1) & 1) {
            if (signal != SIGCHLD && signal != SIGCONT && signal != SIGURG && signal != SIGWINCH) {
                if (crashSignal(signal)) {
                    raiseException(signal, information, host, true);
                    reportCrash(signal, information->si_addr, host);
                }
                struct sigaction action = {};
                sigaction(signal, &action, nullptr);
                sigset_t set;
                sigemptyset(&set);
                sigaddset(&set, signal);
                pthread_sigmask(SIG_UNBLOCK, &set, nullptr);
                raise(signal);
            }
        }
        errno = error;
        return;
    }
    if (handler == 1) {
        errno = error;
        return;
    }
    if (!(flags & 0x40)) {
        reinterpret_cast<void (*)(int)>(handler)(darwin);
        errno = error;
        return;
    }
    DarwinSiginfo details = {};
    details.signal = darwinSignal(signal);
    details.error = information->si_errno ? darwinError(information->si_errno) : 0;
    details.code = information->si_code;
    switch (information->si_code) {
    case SI_USER:
    case SI_TKILL:
        details.code = 0x10001;
        break;
    case SI_QUEUE:
        details.code = 0x10002;
        break;
    case SI_TIMER:
        details.code = 0x10003;
        break;
    case SI_ASYNCIO:
        details.code = 0x10004;
        break;
    case SI_MESGQ:
        details.code = 0x10005;
        break;
    default:
        if (signal == SIGFPE)
            details.code = unsigned(details.code - 1) < 8 ? floatCodes[details.code - 1] : 0;
        break;
    }
    switch (signal) {
    case SIGILL:
    case SIGTRAP:
    case SIGBUS:
    case SIGFPE:
    case SIGSEGV:
        details.address = information->si_addr;
        break;
    case SIGCHLD:
        details.process = information->si_pid;
        details.user = information->si_uid;
        details.status = information->si_status;
        break;
    case SIGIO:
        details.band = information->si_band;
        break;
    default:
        details.process = information->si_pid;
        details.user = information->si_uid;
        details.value = information->si_value.sival_ptr;
        break;
    }
    MachineContext state;
    readContext(state, information, host);
    DarwinContext program = {};
    program.onStack = !(alternateStack.flags & 4) && state.thread.rsp > reinterpret_cast<uint64_t>(alternateStack.memory) &&
                      state.thread.rsp <= reinterpret_cast<uint64_t>(alternateStack.memory) + alternateStack.size;
    program.mask = darwinMask(&host->uc_sigmask);
    hostSigaltstack(nullptr, &program.stack);
    program.size = sizeof state;
    program.context = &state;
    reinterpret_cast<void (*)(int, DarwinSiginfo*, DarwinContext*)>(handler)(darwin, &details, &program);
    writeContext(state, host);
    for (int i = 1; i < 32; i++) {
        if (hostSignals[i]) {
            if (program.mask & 1u << (i - 1))
                sigaddset(&host->uc_sigmask, hostSignals[i]);
            else
                sigdelset(&host->uc_sigmask, hostSignals[i]);
        }
    }
    errno = error;
}

static void handleSignal(int signal, siginfo_t* information, void* context)
{
    if (returnFromMissing(signal, static_cast<ucontext_t*>(context)))
        return;
    if (crashSignal(signal) && raiseException(signal, information, static_cast<ucontext_t*>(context), false))
        return;
    deliver(signal, information, context);
}

void addDarwinSignals(uint32_t mask, sigset_t* set)
{
    for (int signal = 1; signal < 32; signal++) {
        if ((mask >> (signal - 1) & 1) && hostSignals[signal])
            sigaddset(set, hostSignals[signal]);
    }
}

static int hostSigwait(uint32_t mask)
{
    sigset_t set;
    sigemptyset(&set);
    addDarwinSignals(mask, &set);
    int signal = 0;
    const int error = sigwait(&set, &signal);
    if (error)
        return -darwinError(error);
    return darwinSignal(signal);
}

static void removeDarwinSignals(sigset_t* set)
{
    for (int signal = 1; signal < 32; signal++) {
        if (hostSignals[signal])
            sigdelset(set, hostSignals[signal]);
    }
}

static int hostSigsuspend(uint32_t mask)
{
    sigset_t set;
    pthread_sigmask(SIG_SETMASK, nullptr, &set);
    removeDarwinSignals(&set);
    addDarwinSignals(mask, &set);
    sigsuspend(&set);
    return -darwinError(errno);
}

static void handleCrash(int signal, siginfo_t* information, void* context);

static void watchCrash(int signal)
{
    struct sigaction action = {};
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handleCrash;
    action.sa_flags = SA_SIGINFO | SA_ONSTACK | int(SA_RESETHAND);
    sigaction(signal, &action, nullptr);
}

static void handleCrash(int signal, siginfo_t* information, void* context)
{
    ucontext_t* host = static_cast<ucontext_t*>(context);
    if (returnFromMissing(signal, host) || raiseException(signal, information, host, false)) {
        watchCrash(signal);
        return;
    }
    raiseException(signal, information, host, true);
    reportCrash(signal, information->si_addr, host);
    struct sigaction action = {};
    sigaction(signal, &action, nullptr);
    raise(signal);
}

static int hostSigaction(int signal, uint64_t handler, uint32_t mask, uint32_t flags, uint64_t* oldHandler, uint32_t* oldMask, uint32_t* oldFlags)
{
    if (signal < 1 || signal > 31)
        return -22;
    if (oldHandler)
        *oldHandler = actions[signal].handler;
    if (oldMask)
        *oldMask = actions[signal].mask;
    if (oldFlags)
        *oldFlags = actions[signal].flags;
    if (handler == ~uint64_t(0))
        return 0;
    const int host = hostSignals[signal];
    if (host == SIGKILL || host == SIGSTOP)
        return -22;
    actions[signal].handler = handler;
    actions[signal].mask = mask;
    actions[signal].flags = flags;
    if (!host)
        return 0;
    struct sigaction action = {};
    sigemptyset(&action.sa_mask);
    addDarwinSignals(mask, &action.sa_mask);
    if (!(signalFdMask >> (signal - 1) & 1) && handler < 2) {
        if (handler == 1) {
            action.sa_handler = SIG_IGN;
        } else if (crashSignal(host)) {
            action.sa_sigaction = handleCrash;
            action.sa_flags = SA_SIGINFO | SA_ONSTACK | int(SA_RESETHAND);
        } else {
            action.sa_handler = SIG_DFL;
        }
    } else {
        action.sa_sigaction = handleSignal;
        action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    }
    if (flags & 2)
        action.sa_flags |= SA_RESTART;
    if (flags & 4)
        action.sa_flags |= int(SA_RESETHAND);
    if (flags & 8)
        action.sa_flags |= SA_NOCLDSTOP;
    if (flags & 0x10)
        action.sa_flags |= SA_NODEFER;
    if (flags & 0x20)
        action.sa_flags |= SA_NOCLDWAIT;
    if (sigaction(host, &action, nullptr) < 0)
        return -darwinError(errno);
    return 0;
}

static int hostSigmask(int how, const uint32_t* set, uint32_t* old)
{
    sigset_t current;
    pthread_sigmask(SIG_SETMASK, nullptr, &current);
    if (old)
        *old = darwinMask(&current);
    if (!set)
        return 0;
    sigset_t host;
    sigemptyset(&host);
    addDarwinSignals(*set, &host);
    int error;
    if (how == 1) {
        error = pthread_sigmask(SIG_BLOCK, &host, nullptr);
    } else if (how == 2) {
        error = pthread_sigmask(SIG_UNBLOCK, &host, nullptr);
    } else if (how == 3) {
        removeDarwinSignals(&current);
        addDarwinSignals(*set, &current);
        error = pthread_sigmask(SIG_SETMASK, &current, nullptr);
    } else {
        return -22;
    }
    return error ? -22 : 0;
}

void saveThreadState(const void* context, const void* information, ThreadState* thread, FloatState* floats)
{
    MachineContext state;
    readContext(state, static_cast<const siginfo_t*>(information), static_cast<const ucontext_t*>(context));
    *thread = state.thread;
    *floats = state.floats;
}

void setUpCrashHandler()
{
    for (const int signal : crashSignals)
        watchCrash(signal);
}

void setUpSignalStack()
{
    (void)signalStack.memory;
}

// signalfd for Darwin signals: a pipe that the handler writes to.
int emulatedSignalfd(int fd, uint32_t mask, int flags)
{
    std::lock_guard lock(signalFdMutex);
    if (fd < 0) {
        int ends[2];
        if (pipe2(ends, (flags & O_NONBLOCK) | O_CLOEXEC) < 0)
            return -darwinError(errno);
        fcntl(ends[1], F_SETFL, O_NONBLOCK);
        if (!(flags & O_CLOEXEC))
            fcntl(ends[0], F_SETFD, 0);
        // The handler goes through the entries: they never move.
        if (signalFds.capacity() < 64)
            signalFds.reserve(64);
        if (signalFds.size() == signalFds.capacity()) {
            close(ends[0]);
            close(ends[1]);
            return -24;
        }
        signalFds.push_back({ends[1], mask});
        fd = ends[0];
    } else {
        for (SignalFd& entry : signalFds) {
            if (entry.fd == fd)
                entry.mask = mask;
        }
    }
    uint32_t all = 0;
    for (const SignalFd& entry : signalFds)
        all |= entry.mask;
    const uint32_t added = ~signalFdMask.exchange(all) & all;
    for (int signal = 1; signal < 32; signal++) {
        const int host = hostSignals[signal];
        if (!(added >> (signal - 1) & 1) || host == 0 || host == SIGKILL || host == SIGSTOP)
            continue;
        struct sigaction action = {};
        sigemptyset(&action.sa_mask);
        action.sa_sigaction = handleSignal;
        action.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESTART;
        sigaction(host, &action, nullptr);
    }
    return fd;
}

void addSignalServices(HostServices& services)
{
    services.sigaction = hostSigaction;
    services.sigmask = hostSigmask;
    services.killThread = hostKillThread;
    services.sigaltstack = hostSigaltstack;
    services.kill = hostKill;
    services.raise = hostRaise;
    services.alarm = hostAlarm;
    services.sigpending = hostSigpending;
    services.sigsuspend = hostSigsuspend;
    services.sigwait = hostSigwait;
    services.setExceptionHandler = setExceptionHandler;
}

} // namespace cutefish::reef
