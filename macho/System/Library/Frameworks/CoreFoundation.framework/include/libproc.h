// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal Darwin declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdint.h>
#define PROC_PIDT_SHORTBSDINFO 13
struct proc_bsdshortinfo { uint32_t pbsi_pid; uint32_t pbsi_ppid; uint32_t pbsi_pgid; uint32_t pbsi_status; char pbsi_comm[16]; uint32_t pbsi_flags; uint32_t pbsi_uid; uint32_t pbsi_gid; uint32_t pbsi_ruid; uint32_t pbsi_rgid; uint32_t pbsi_svuid; uint32_t pbsi_svgid; uint32_t pbsi_rfu; };
#define PROC_PIDT_SHORTBSDINFO_SIZE (sizeof(struct proc_bsdshortinfo))
#define PROC_FLAG_TRACED 0x2
int proc_pidinfo(int pid, int flavor, uint64_t arg, void *buffer, int buffersize);
