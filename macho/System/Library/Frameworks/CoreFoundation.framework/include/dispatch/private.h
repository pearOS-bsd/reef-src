// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// The private entry points of libdispatch that CoreFoundation uses (the private headers of libdispatch are not used as a whole).
#pragma once
#include <dispatch/dispatch.h>
#define DISPATCH_QUEUE_OVERCOMMIT 0x2ull
extern dispatch_queue_t _dispatch_runloop_root_queue_create_4CF(const char *label, unsigned long flags);
extern int _dispatch_runloop_root_queue_get_port_4CF(dispatch_queue_t queue);
extern void _dispatch_source_set_runloop_timer_4CF(dispatch_source_t source, dispatch_time_t start, uint64_t interval, uint64_t leeway);
extern bool _dispatch_runloop_root_queue_perform_4CF(dispatch_queue_t queue);
