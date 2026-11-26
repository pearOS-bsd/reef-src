// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#define CTL_KERN 1
#define CTL_HW 6
#define HW_NCPU 3
#define HW_MEMSIZE 24
#define HW_AVAILCPU 25
#define KERN_OSRELEASE 2
#define KERN_PROC 14
int sysctl(int *name, unsigned int namelen, void *oldp, unsigned long *oldlenp, void *newp, unsigned long newlen);
int sysctlbyname(const char *name, void *oldp, unsigned long *oldlenp, void *newp, unsigned long newlen);
#define KERN_PROC_PID 1
#define P_TRACED 0x00000800
struct kinfo_proc { struct { char pad0[32]; int p_flag; char pad1[400]; } kp_proc; struct { char pad[300]; } kp_eproc; };
