// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The registers of a thread as Darwin programs see them (x86_thread_state64, x86_float_state64, mcontext_t).

#pragma once

#include <cstdint>

namespace cutefish::reef {

struct ExceptionState
{
    uint16_t trap;
    uint16_t cpu;
    uint32_t error;
    uint64_t address;
};

struct ThreadState
{
    uint64_t rax, rbx, rcx, rdx, rdi, rsi, rbp, rsp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags, cs, fs, gs;
};

struct FloatState
{
    int reserved[2];
    uint8_t fpu[512];
    int reserved1;
};

struct MachineContext
{
    ExceptionState exception;
    ThreadState thread;
    FloatState floats;
};

// The state of the thread a signal handler interrupted.
void saveThreadState(const void* context, const void* information, ThreadState* thread, FloatState* floats);

// Called for the signals of exceptions: the Darwin signal, the code, the address and the registers, which it may change.
// True when the thread goes on; `second` when no handler of the program took the signal.
using ExceptionHandler = bool (*)(int signal, int code, uint64_t address, ThreadState* state, bool second);

} // namespace cutefish::reef
