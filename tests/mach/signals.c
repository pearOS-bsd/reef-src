// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Signals: handlers, masks, the alternate stack, information, waiting.
#include "darwin.h"

static volatile int got;
static volatile int nested;
static void simple(int signal) { got = signal; }

static void detailed(int signal, struct siginfo* information, void* context)
{
    char here;
    stack_t stack;
    sigaltstack(NULL, &stack);
    got = signal;
    printf("handler %d: signo %d code %x own pid %d context %d, on alternate stack %d %d\n", signal, information->si_signo, information->si_code,
           information->si_pid == getpid(), context != NULL, stack.ss_flags,
           (char*)&here >= (char*)stack.ss_sp && (char*)&here < (char*)stack.ss_sp + stack.ss_size);
}

static void fault(int signal, struct siginfo* information, void* context)
{
    // struct ucontext64: the machine context is at 48; rip is the 17th register after the exception state.
    uint64_t* machine = *(uint64_t**)((char*)context + 48);
    printf("fault %d code %d address %p, error %x\n", signal, information->si_code, information->si_addr, ((uint32_t*)machine)[1]);
    machine[2 + 16] += 2;
    nested++;
}

static void* waiter(void* argument)
{
    sigset_t set = 1u << (30 - 1);
    int signal = 0;
    int error = sigwait(&set, &signal);
    printf("sigwait %d %d\n", error, signal);
    return argument;
}

int main(void)
{
    struct sigaction action, old;
    memset(&action, 0, sizeof action);
    for (int signal = 0; signal < 34; signal++) {
        errno = 0;
        int result = sigaction(signal, NULL, &old);
        if (result)
            printf("sigaction %d: %d %d\n", signal, result, errno);
    }
    action.u.handler = simple;
    for (int signal = 1; signal < 32; signal++) {
        if (signal == 4 || signal == 5 || signal == 8 || signal == 10 || signal == 11)
            continue;
        errno = 0;
        got = 0;
        int set = sigaction(signal, &action, NULL);
        int error = errno;
        int raised = set == 0 && signal != 9 && signal != 17 ? raise(signal) : -1;
        printf("signal %d: set %d %d raised %d got %d\n", signal, set, set ? error : 0, raised, got);
    }
    sigset_t mask = 1u << 29, pending = 0, current = 0;
    CHECK(sigprocmask(1, &mask, &current));
    printf("mask was %x\n", current);
    got = 0;
    CHECK(raise(30));
    CHECK(sigpending(&pending));
    printf("pending %x got %d\n", pending, got);
    mask = 0;
    errno = 0;
    CHECK(sigsuspend(&mask));
    printf("after sigsuspend got %d\n", got);
    CHECK(sigprocmask(3, &mask, &current));
    printf("mask was %x\n", current);
    CHECK(sigprocmask(9, &mask, NULL));
    stack_t stack = {malloc(65536), 65536, 0}, previous;
    CHECK(sigaltstack(&stack, &previous));
    printf("alternate stack was %d\n", previous.ss_flags);
    stack_t small = {malloc(1024), 1024, 0};
    CHECK(sigaltstack(&small, NULL));
    action.u.action = detailed;
    action.sa_flags = 0x41;
    CHECK(sigaction(31, &action, &old));
    printf("old handler %d flags %x\n", old.u.handler == simple, old.sa_flags);
    CHECK(raise(31));
    CHECK(kill(getpid(), 31));
    action.sa_flags = 0x40;
    CHECK(sigaction(31, &action, NULL));
    CHECK(pthread_kill(pthread_self(), 31));
    CHECK(kill(getpid(), 99));
    CHECK(kill(getpid(), 0));
    action.u.handler = simple;
    action.sa_flags = 4;
    CHECK(sigaction(30, &action, NULL));
    CHECK(raise(30));
    CHECK(sigaction(30, NULL, &old));
    printf("reset %d\n", old.u.handler == NULL);
    action.u.action = fault;
    action.sa_flags = 0x41;
    CHECK(sigaction(11, &action, NULL));
    CHECK(sigaction(10, &action, NULL));
    CHECK(sigaction(4, &action, NULL));
    __asm volatile("xorl %%ecx, %%ecx\n\tmovb (%%rcx), %%al" : : : "rcx", "rax", "memory");
    __asm volatile("movl $8, %%ecx\n\tmovb %%al, (%%rcx)" : : : "rcx", "rax", "memory");
    __asm volatile("ud2");
    printf("faults %d\n", nested);
    mask = 1u << 29;
    CHECK(sigprocmask(1, &mask, NULL));
    pthread_t thread;
    CHECK(pthread_create(&thread, NULL, waiter, NULL));
    usleep(20000);
    CHECK(kill(getpid(), 30));
    CHECK(pthread_join(thread, NULL));
    action.u.handler = simple;
    action.sa_flags = 0;
    CHECK(sigaction(14, &action, NULL));
    got = 0;
    CHECK(alarm(1));
    struct timespec time = {3, 0}, rest = {0, 0};
    CHECK(nanosleep(&time, &rest));
    printf("alarm got %d, rest %ld\n", got, rest.tv_sec);
    fflush(__stdoutp);
    raise(6);
    printf("after the handler of abort\n");
    fflush(__stdoutp);
    action.u.handler = NULL;
    sigaction(11, &action, NULL);
    *(volatile int*)16 = 1;
    return 0;
}
