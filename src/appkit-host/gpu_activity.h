// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The log of GPU work that runs long, which reef-diagnose sets against the GPU hangs of the kernel log.

#pragma once

namespace cutefish::reef {

// The GPU work issued so far in the calling thread's EGL context: it is logged when the GPU takes long to finish it.
void hostGpuWorkIssued(const char* work, const char* detail);
// Work that the program measured itself: it has run for so many milliseconds, or still runs, since a time in seconds
// since 1970, and the thread issued it.
void hostGpuWorkMeasured(const char* work, const char* detail, int running, int thread, double milliseconds, double time);
// The calling thread, as the kernel names it.
int hostThreadId();

} // namespace cutefish::reef
