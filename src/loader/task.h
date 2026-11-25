// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The memory a process shares with the debuggers of its session, found by the name of its mapping: the threads it stopped
// for them and their registers.

#pragma once

#include "thread_state.h"

#include <atomic>
#include <cstdint>

namespace cutefish::reef {

struct TaskThread
{
    std::atomic<int> thread;
    std::atomic<int> captures;
    ThreadState state;
    uint64_t unknownb0[13];
    FloatState floats;
};

struct Task
{
    static constexpr uint32_t Magic = 0x4b534154;
    static constexpr uint32_t MaxThreads = 1024;

    uint32_t magic;
    std::atomic<uint32_t> threadCount;
    uint64_t unknown8[2];
    std::atomic<int> suspended;
    uint32_t unknown1c[71];
    TaskThread threads[MaxThreads];
};

} // namespace cutefish::reef
