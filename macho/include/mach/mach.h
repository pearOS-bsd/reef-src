// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin C library declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _MACH_PORT_T
#define _MACH_PORT_T
typedef unsigned int mach_port_t;
#endif
mach_port_t mach_thread_self(void);
#ifdef __cplusplus
}
#endif
