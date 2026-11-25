// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The timing of displays, as the kernel's DRM devices know it.

#pragma once

#include <cstdint>

namespace cutefish::reef {

// Displays are named as the compositor names them ("eDP-1"); without a name, or without such a display, the first one
// that is lit.

// Waits for the next vertical blank and gives its time (nanoseconds of CLOCK_MONOTONIC). Returns 1, or 0 when it cannot.
int hostDisplayWaitVblank(const char* name, uint64_t* time);
// Nanoseconds between two frames, 0 when it is not known.
uint64_t hostDisplayPeriod(const char* name);

} // namespace cutefish::reef
