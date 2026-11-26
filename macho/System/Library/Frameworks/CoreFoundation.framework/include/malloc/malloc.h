// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <stdlib.h>
struct malloc_introspection_t { void *enumerator, *good_size, *check, *print, *log, *force_lock, *force_unlock, *statistics, *zone_locked, *enable_discharge_checking, *disable_discharge_checking, *discharge, *enumerate_discharged_pointers, *reinit_lock, *print_task, *task_statistics; };
typedef struct _malloc_zone_t malloc_zone_t;
size_t malloc_good_size(size_t size);
malloc_zone_t *malloc_default_zone(void);
void *malloc_zone_malloc(malloc_zone_t *zone, size_t size);
void *malloc_zone_calloc(malloc_zone_t *zone, size_t num, size_t size);
void *malloc_zone_realloc(malloc_zone_t *zone, void *ptr, size_t size);
void *malloc_zone_memalign(malloc_zone_t *zone, size_t alignment, size_t size);
void malloc_zone_free(malloc_zone_t *zone, void *ptr);
malloc_zone_t *malloc_zone_from_ptr(const void *ptr);

