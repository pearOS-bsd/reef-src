// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal libdispatch declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
typedef struct dispatch_queue_s *dispatch_queue_t;
typedef void (^dispatch_block_t)(void);
extern struct dispatch_queue_s _dispatch_main_q;
#define dispatch_get_main_queue() (&_dispatch_main_q)
void dispatch_async(dispatch_queue_t queue, dispatch_block_t block);
