// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#define major(dev) ((unsigned int)((((unsigned long long)(dev) >> 8) & 0xfff) | (((unsigned long long)(dev) >> 32) & ~0xfffULL)))
#define minor(dev) ((unsigned int)(((unsigned long long)(dev) & 0xff) | (((unsigned long long)(dev) >> 12) & ~0xffULL)))
