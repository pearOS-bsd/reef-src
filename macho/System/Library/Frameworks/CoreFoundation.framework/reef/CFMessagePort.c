// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFMessagePort: remote ports are not supported; there is no name server to find them.
#include "CFInternal.h"

typedef struct __CFMessagePort *CFMessagePortRef;

CFMessagePortRef CFMessagePortCreateRemote(CFAllocatorRef allocator, CFStringRef name) {
    return NULL;
}

Boolean CFMessagePortIsValid(CFMessagePortRef ms) {
    return false;
}

SInt32 CFMessagePortSendRequest(CFMessagePortRef remote, SInt32 msgid, CFDataRef data, CFTimeInterval sendTimeout, CFTimeInterval rcvTimeout, CFStringRef replyMode, CFDataRef *returnData) {
    if (returnData) *returnData = NULL;
    return -3; // kCFMessagePortTransportError
}
