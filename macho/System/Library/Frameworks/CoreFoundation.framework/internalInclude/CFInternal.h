// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFInternal.h of CoreFoundation 6.0 with the Objective-C dispatch of the Mac build: a CF call on an object that is not a CF object of its type
// (tagged pointer, or an isa other than the one registered for the type) is sent as a message to the Objective-C object.
#pragma once
#include_next "CFInternal.h"

#ifndef __kCFAllocatorTypeID_CONST
#define __kCFAllocatorTypeID_CONST _kCFRuntimeIDCFAllocator
#endif
#if DEPLOYMENT_RUNTIME_OBJC
extern uintptr_t _CFAutoreleasePoolPush(void);
extern void _CFAutoreleasePoolPop(uintptr_t pool);
#endif

#if DEPLOYMENT_RUNTIME_OBJC
#ifdef __OBJC__
#include <Foundation/NSDispatch.h>
#endif

CF_INLINE int __CFIsObjC(CFTypeID typeID, const void *obj) {
    if ((uintptr_t)obj & 1) return 1;
    uintptr_t isa = (uintptr_t)((CFRuntimeBase *)obj)->_cfisa;
    if (isa == 0 || isa == __CFISAForTypeID(typeID)) return 0;
    if (typeID == _kCFRuntimeIDCFString && isa == (uintptr_t)__CFConstantStringClassReferencePtr) return 0;
    return 1;
}

#undef CF_IS_OBJC
#undef CF_OBJC_FUNCDISPATCHV
#undef CF_OBJC_RETAINED_FUNCDISPATCHV
#undef CF_OBJC_CALLV
#define CF_IS_OBJC(typeID, obj) __CFIsObjC(typeID, (const void *)(obj))
#ifdef __OBJC__
#define CF_OBJC_FUNCDISPATCHV(typeID, rettype, obj, ...) do { \
    if (__builtin_expect(CF_IS_OBJC(typeID, obj), 0)) { return (rettype)[(id)(obj) __VA_ARGS__]; } \
} while (0)
#define CF_OBJC_RETAINED_FUNCDISPATCHV(typeID, rettype, obj, ...) do { \
    if (__builtin_expect(CF_IS_OBJC(typeID, obj), 0)) { return (rettype)[(id)(obj) __VA_ARGS__]; } \
} while (0)
#define CF_OBJC_CALLV(obj, ...) ([(id)(obj) __VA_ARGS__])
#endif
#endif
