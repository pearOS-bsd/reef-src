// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
typedef uint64_t AbsoluteTime;
#define mk_timer_cancel(port, result) 0
