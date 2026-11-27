// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFFileSecurity: owner, group and mode of a file, as the value of kCFURLFileSecurityKey.
#include "CFInternal.h"
#include "CFRuntime_Internal.h"

typedef const struct __CFFileSecurity *CFFileSecurityRef;

enum { kOwnerSet = 1, kGroupSet = 2, kModeSet = 4 };

struct __CFFileSecurity {
    CFRuntimeBase _base;
    uint32_t _flags;
    uint32_t _owner;
    uint32_t _group;
    uint16_t _mode;
};

static CFTypeID securityTypeID;
static pthread_once_t securityOnce = PTHREAD_ONCE_INIT;

static Boolean equalSecurity(CFTypeRef a, CFTypeRef b) {
    const struct __CFFileSecurity *x = (const struct __CFFileSecurity *)a, *y = (const struct __CFFileSecurity *)b;
    return x->_flags == y->_flags && x->_owner == y->_owner && x->_group == y->_group && x->_mode == y->_mode;
}

static const CFRuntimeClass securityClass = {
    0, "CFFileSecurity", NULL, NULL, NULL, equalSecurity, NULL, NULL, NULL, NULL,
};

static void registerSecurity(void) {
    securityTypeID = _CFRuntimeRegisterClass(&securityClass);
}

CFTypeID CFFileSecurityGetTypeID(void) {
    pthread_once(&securityOnce, registerSecurity);
    return securityTypeID;
}

CFFileSecurityRef CFFileSecurityCreate(CFAllocatorRef allocator) {
    pthread_once(&securityOnce, registerSecurity);
    return (CFFileSecurityRef)_CFRuntimeCreateInstance(allocator, securityTypeID, sizeof(struct __CFFileSecurity) - sizeof(CFRuntimeBase), NULL);
}

CFFileSecurityRef CFFileSecurityCreateCopy(CFAllocatorRef allocator, CFFileSecurityRef source) {
    pthread_once(&securityOnce, registerSecurity);
    struct __CFFileSecurity *copy = (struct __CFFileSecurity *)_CFRuntimeCreateInstance(allocator, securityTypeID, sizeof(struct __CFFileSecurity) - sizeof(CFRuntimeBase), NULL);
    if (copy == NULL || source == NULL) return copy;
    copy->_flags = source->_flags;
    copy->_owner = source->_owner;
    copy->_group = source->_group;
    copy->_mode = source->_mode;
    return copy;
}

Boolean CFFileSecurityGetOwner(CFFileSecurityRef fileSec, uid_t *owner) {
    if (!fileSec || !(fileSec->_flags & kOwnerSet)) return false;
    if (owner) *owner = fileSec->_owner;
    return true;
}

Boolean CFFileSecuritySetOwner(CFFileSecurityRef fileSec, uid_t owner) {
    struct __CFFileSecurity *s = (struct __CFFileSecurity *)fileSec;
    if (!s) return false;
    s->_owner = owner;
    s->_flags |= kOwnerSet;
    return true;
}

Boolean CFFileSecurityGetGroup(CFFileSecurityRef fileSec, gid_t *group) {
    if (!fileSec || !(fileSec->_flags & kGroupSet)) return false;
    if (group) *group = fileSec->_group;
    return true;
}

Boolean CFFileSecuritySetGroup(CFFileSecurityRef fileSec, gid_t group) {
    struct __CFFileSecurity *s = (struct __CFFileSecurity *)fileSec;
    if (!s) return false;
    s->_group = group;
    s->_flags |= kGroupSet;
    return true;
}

Boolean CFFileSecurityGetMode(CFFileSecurityRef fileSec, mode_t *mode) {
    if (!fileSec || !(fileSec->_flags & kModeSet)) return false;
    if (mode) *mode = fileSec->_mode & 07777;
    return true;
}

Boolean CFFileSecuritySetMode(CFFileSecurityRef fileSec, mode_t mode) {
    struct __CFFileSecurity *s = (struct __CFFileSecurity *)fileSec;
    if (!s) return false;
    s->_mode = mode & 07777;
    s->_flags |= kModeSet;
    return true;
}
