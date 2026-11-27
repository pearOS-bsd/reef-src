// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// Minimal declarations (the Apple SDK is not used); only what the rebuilt files need.
#pragma once
#include <objc/objc.h>
#include <stdint.h>

typedef uintptr_t objc_AssociationPolicy;
enum {
    OBJC_ASSOCIATION_ASSIGN = 0,
    OBJC_ASSOCIATION_RETAIN_NONATOMIC = 1,
    OBJC_ASSOCIATION_COPY_NONATOMIC = 3,
    OBJC_ASSOCIATION_RETAIN = 01401,
    OBJC_ASSOCIATION_COPY = 01403
};
extern Class objc_getClass(const char *name);
extern id objc_getAssociatedObject(id object, const void *key);
extern void objc_setAssociatedObject(id object, const void *key, id value, objc_AssociationPolicy policy);
