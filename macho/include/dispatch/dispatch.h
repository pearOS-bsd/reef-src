// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal libdispatch declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct dispatch_queue_s *dispatch_queue_t;
typedef void (^dispatch_block_t)(void);
extern struct dispatch_queue_s _dispatch_main_q;
#define dispatch_get_main_queue() (&_dispatch_main_q)
void dispatch_async(dispatch_queue_t queue, dispatch_block_t block);
#define DISPATCH_QUEUE_PRIORITY_DEFAULT 0
#define DISPATCH_APPLY_AUTO ((dispatch_queue_t)0)
dispatch_queue_t dispatch_get_global_queue(long identifier, unsigned long flags);
void dispatch_apply_f(size_t iterations, dispatch_queue_t queue, void *context, void (*work)(void *, size_t));
#ifdef __cplusplus
}
#endif
