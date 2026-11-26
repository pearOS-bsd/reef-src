// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <mach/machine.h>
typedef struct { const char *name; cpu_type_t cputype; cpu_subtype_t cpusubtype; } NXArchInfo;
const NXArchInfo *NXGetLocalArchInfo(void);
