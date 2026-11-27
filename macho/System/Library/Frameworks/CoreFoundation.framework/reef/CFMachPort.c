// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFMachPort: a Mach port as a CF object with a run loop source fed by a watcher thread; CFMessagePort is not supported (remote ports fail).
#include "CFInternal.h"
#include "CFRuntime_Internal.h"
#include <dispatch/dispatch.h>
#include <mach/mach.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct __CFMachPort *CFMachPortRef;
typedef struct {
    CFIndex version;
    void *info;
    const void *(*retain)(const void *info);
    void (*release)(const void *info);
    CFStringRef (*copyDescription)(const void *info);
} CFMachPortContext;
typedef void (*CFMachPortCallBack)(CFMachPortRef port, void *msg, CFIndex size, void *info);
typedef void (*CFMachPortInvalidationCallBack)(CFMachPortRef port, void *info);

struct __CFMachPort {
    CFRuntimeBase _base;
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
    mach_port_t _port;
    Boolean _ownsReceiveRight;
    Boolean _valid;
    CFMachPortCallBack _callout;
    CFMachPortContext _context;
    CFMachPortInvalidationCallBack _invalidation;
    CFRunLoopSourceRef _source;
    CFMutableArrayRef _runLoops;
    pthread_t _thread;
    Boolean _threadStarted;
    Boolean _pending;
};

enum { kReefTerminateMessageID = 0x52454546 };

static CFTypeID __kCFMachPortTypeID = _kCFRuntimeNotATypeID;
static pthread_mutex_t _portsLock = PTHREAD_MUTEX_INITIALIZER;
static CFMutableDictionaryRef _portsByName;

static void __CFMachPortDeallocate(CFTypeRef cf);
static CFStringRef __CFMachPortCopyDescription(CFTypeRef cf);

static const CFRuntimeClass __CFMachPortRuntimeClass = {
    0, "CFMachPort", NULL, NULL, __CFMachPortDeallocate, NULL, NULL, NULL, __CFMachPortCopyDescription,
};

CFTypeID CFMachPortGetTypeID(void) {
    static dispatch_once_t once;
    dispatch_once(&once, ^{ __kCFMachPortTypeID = _CFRuntimeRegisterClass(&__CFMachPortRuntimeClass); });
    return __kCFMachPortTypeID;
}

static void __CFMachPortWakeRunLoops(struct __CFMachPort *mp) {
    CFRunLoopSourceSignal(mp->_source);
    if (mp->_runLoops) {
        for (CFIndex i = 0; i < CFArrayGetCount(mp->_runLoops); i++) CFRunLoopWakeUp((CFRunLoopRef)CFArrayGetValueAtIndex(mp->_runLoops, i));
    }
}

static struct __CFMachPort *__CFMachPortCreate(CFAllocatorRef allocator, mach_port_t port, Boolean ownsReceiveRight, CFMachPortCallBack callout, CFMachPortContext *context) {
    CFMachPortGetTypeID();
    struct __CFMachPort *mp = (struct __CFMachPort *)_CFRuntimeCreateInstance(allocator, __kCFMachPortTypeID, sizeof(struct __CFMachPort) - sizeof(CFRuntimeBase), NULL);
    if (mp) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mp->_lock, &attr);
        pthread_mutexattr_destroy(&attr);
        pthread_cond_init(&mp->_cond, NULL);
        mp->_port = port;
        mp->_ownsReceiveRight = ownsReceiveRight;
        mp->_valid = true;
        mp->_callout = callout;
        if (context) {
            mp->_context = *context;
            if (context->retain) mp->_context.info = (void *)context->retain(context->info);
        }
    }
    return mp;
}

CFMachPortRef CFMachPortCreate(CFAllocatorRef allocator, CFMachPortCallBack callout, CFMachPortContext *context, Boolean *shouldFreeInfo) {
    if (shouldFreeInfo) *shouldFreeInfo = true;
    mach_port_t port = MACH_PORT_NULL;
    if (mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port) == KERN_SUCCESS) {
        if (mach_port_insert_right(mach_task_self(), port, port, MACH_MSG_TYPE_MAKE_SEND) == KERN_SUCCESS) {
            struct __CFMachPort *mp = __CFMachPortCreate(allocator, port, true, callout, context);
            if (mp) {
                pthread_mutex_lock(&_portsLock);
                if (!_portsByName) _portsByName = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
                CFDictionarySetValue(_portsByName, (const void *)(uintptr_t)port, mp);
                pthread_mutex_unlock(&_portsLock);
                if (shouldFreeInfo) *shouldFreeInfo = false;
                return (CFMachPortRef)mp;
            }
            mach_port_deallocate(mach_task_self(), port);
        }
        mach_port_mod_refs(mach_task_self(), port, MACH_PORT_RIGHT_RECEIVE, -1);
    }
    return NULL;
}

CFMachPortRef CFMachPortCreateWithPort(CFAllocatorRef allocator, mach_port_t portNum, CFMachPortCallBack callout, CFMachPortContext *context, Boolean *shouldFreeInfo) {
    if (shouldFreeInfo) *shouldFreeInfo = true;
    if (portNum == MACH_PORT_NULL) return NULL;
    pthread_mutex_lock(&_portsLock);
    if (!_portsByName) _portsByName = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
    struct __CFMachPort *mp = (struct __CFMachPort *)CFDictionaryGetValue(_portsByName, (const void *)(uintptr_t)portNum);
    if (mp && mp->_valid) {
        CFRetain(mp);
        pthread_mutex_unlock(&_portsLock);
        return (CFMachPortRef)mp;
    }
    mp = __CFMachPortCreate(allocator, portNum, false, callout, context);
    if (mp) {
        CFDictionarySetValue(_portsByName, (const void *)(uintptr_t)portNum, mp);
        pthread_mutex_unlock(&_portsLock);
        if (shouldFreeInfo) *shouldFreeInfo = false;
        return (CFMachPortRef)mp;
    }
    pthread_mutex_unlock(&_portsLock);
    return NULL;
}

mach_port_t CFMachPortGetPort(CFMachPortRef port) {
    return port ? ((struct __CFMachPort *)port)->_port : MACH_PORT_NULL;
}

void CFMachPortGetContext(CFMachPortRef port, CFMachPortContext *context) {
    if (port && context) *context = ((struct __CFMachPort *)port)->_context;
}

Boolean CFMachPortIsValid(CFMachPortRef port) {
    return port ? ((struct __CFMachPort *)port)->_valid : false;
}

CFMachPortInvalidationCallBack CFMachPortGetInvalidationCallBack(CFMachPortRef port) {
    return port ? ((struct __CFMachPort *)port)->_invalidation : NULL;
}

void CFMachPortSetInvalidationCallBack(CFMachPortRef port, CFMachPortInvalidationCallBack callout) {
    struct __CFMachPort *mp = (struct __CFMachPort *)port;
    if (!mp) return;
    pthread_mutex_lock(&mp->_lock);
    Boolean valid = mp->_valid;
    mp->_invalidation = callout;
    pthread_mutex_unlock(&mp->_lock);
    if (callout && !valid) callout(port, mp->_context.info);
}

void CFMachPortInvalidate(CFMachPortRef port) {
    struct __CFMachPort *mp = (struct __CFMachPort *)port;
    if (!mp) return;
    CFRetain(mp);
    pthread_mutex_lock(&mp->_lock);
    if (!mp->_valid) {
        pthread_mutex_unlock(&mp->_lock);
        CFRelease(mp);
        return;
    }
    mp->_valid = false;
    Boolean threadStarted = mp->_threadStarted;
    mp->_threadStarted = false;
    pthread_cond_broadcast(&mp->_cond);
    CFRunLoopSourceRef source = mp->_source;
    mp->_source = NULL;
    pthread_mutex_unlock(&mp->_lock);
    pthread_mutex_lock(&_portsLock);
    if (_portsByName) {
        if (CFDictionaryGetValue(_portsByName, (const void *)(uintptr_t)mp->_port) == mp) CFDictionaryRemoveValue(_portsByName, (const void *)(uintptr_t)mp->_port);
    }
    pthread_mutex_unlock(&_portsLock);
    if (threadStarted) {
        if (mp->_ownsReceiveRight) {
            mach_msg_header_t msg = {0};
            msg.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
            msg.msgh_size = sizeof(msg);
            msg.msgh_remote_port = mp->_port;
            msg.msgh_local_port = MACH_PORT_NULL;
            msg.msgh_id = kReefTerminateMessageID;
            mach_msg(&msg, MACH_SEND_MSG | MACH_SEND_TIMEOUT, sizeof(msg), 0, MACH_PORT_NULL, 0, MACH_PORT_NULL);
        }
        pthread_join(mp->_thread, NULL);
    }
    if (source) {
        CFRunLoopSourceInvalidate(source);
        CFRelease(source);
    }
    if (mp->_ownsReceiveRight) {
        mach_port_deallocate(mach_task_self(), mp->_port);
        mach_port_mod_refs(mach_task_self(), mp->_port, MACH_PORT_RIGHT_RECEIVE, -1);
    }
    if (mp->_invalidation) mp->_invalidation(port, mp->_context.info);
    void *info = mp->_context.info;
    mp->_context.info = NULL;
    if (info && mp->_context.release) mp->_context.release(info);
    CFRelease(mp);
}

static void __CFMachPortSchedule(void *info, CFRunLoopRef rl, CFStringRef mode) {
    struct __CFMachPort *mp = (struct __CFMachPort *)info;
    pthread_mutex_lock(&mp->_lock);
    if (!mp->_runLoops) mp->_runLoops = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    CFArrayAppendValue(mp->_runLoops, rl);
    if (mp->_pending && mp->_source) __CFMachPortWakeRunLoops(mp);
    pthread_cond_broadcast(&mp->_cond);
    pthread_mutex_unlock(&mp->_lock);
}

static void __CFMachPortCancel(void *info, CFRunLoopRef rl, CFStringRef mode) {
    struct __CFMachPort *mp = (struct __CFMachPort *)info;
    pthread_mutex_lock(&mp->_lock);
    if (mp->_runLoops) {
        CFIndex idx = CFArrayGetFirstIndexOfValue(mp->_runLoops, CFRangeMake(0, CFArrayGetCount(mp->_runLoops)), rl);
        if (idx != kCFNotFound) CFArrayRemoveValueAtIndex(mp->_runLoops, idx);
    }
    pthread_mutex_unlock(&mp->_lock);
}

static void __CFMachPortPerform(void *info) {
    struct __CFMachPort *mp = (struct __CFMachPort *)info;
    CFRetain(mp);
    mach_msg_size_t size = 0x400;
    mach_msg_header_t *msg = (mach_msg_header_t *)malloc(size);
    while (msg) {
        if (!mp->_valid) break;
        mach_msg_return_t ret = mach_msg(msg, MACH_RCV_MSG | MACH_RCV_LARGE | MACH_RCV_TIMEOUT, 0, size, mp->_port, 0, MACH_PORT_NULL);
        if (ret == MACH_MSG_SUCCESS) {
            if (msg->msgh_id == kReefTerminateMessageID && mp->_ownsReceiveRight && !mp->_valid) break;
            if (mp->_callout) mp->_callout((CFMachPortRef)mp, msg, msg->msgh_size, mp->_context.info);
            continue;
        }
        if (ret != MACH_RCV_TOO_LARGE) break;
        size = msg->msgh_size;
        mach_msg_header_t *bigger = (mach_msg_header_t *)realloc(msg, size);
        if (!bigger) break;
        msg = bigger;
    }
    free(msg);
    pthread_mutex_lock(&mp->_lock);
    mp->_pending = false;
    pthread_cond_broadcast(&mp->_cond);
    pthread_mutex_unlock(&mp->_lock);
    CFRelease(mp);
}

static void *__CFMachPortWatch(void *arg) {
    struct __CFMachPort *mp = (struct __CFMachPort *)arg;
    pthread_mutex_lock(&mp->_lock);
    while (mp->_valid) {
        while (mp->_pending || !mp->_runLoops || CFArrayGetCount(mp->_runLoops) == 0) {
            pthread_cond_wait(&mp->_cond, &mp->_lock);
            if (!mp->_valid) goto done;
        }
        Boolean valid = mp->_valid;
        pthread_mutex_unlock(&mp->_lock);
        if (!valid) return NULL;
        mach_msg_header_t peek = {0};
        mach_msg_return_t ret = mach_msg(&peek, MACH_RCV_MSG | MACH_RCV_LARGE | MACH_RCV_TIMEOUT, 0, sizeof(peek), mp->_port, 100, MACH_PORT_NULL);
        if (ret != MACH_RCV_TIMED_OUT) {
            pthread_mutex_lock(&mp->_lock);
            if (ret != MACH_RCV_TOO_LARGE) break;
            if (mp->_valid) {
                mp->_pending = true;
                if (mp->_source) __CFMachPortWakeRunLoops(mp);
            }
            pthread_mutex_unlock(&mp->_lock);
        }
        pthread_mutex_lock(&mp->_lock);
    }
done:
    pthread_mutex_unlock(&mp->_lock);
    return NULL;
}

CFRunLoopSourceRef CFMachPortCreateRunLoopSource(CFAllocatorRef allocator, CFMachPortRef port, CFIndex order) {
    struct __CFMachPort *mp = (struct __CFMachPort *)port;
    if (!mp) return NULL;
    pthread_mutex_lock(&mp->_lock);
    if (!mp->_valid) {
        pthread_mutex_unlock(&mp->_lock);
        return NULL;
    }
    CFRunLoopSourceRef result = mp->_source;
    if (!result) {
        CFRunLoopSourceContext context = {0};
        context.info = mp;
        context.retain = (const void *(*)(const void *))CFRetain;
        context.release = (void (*)(const void *))CFRelease;
        context.schedule = __CFMachPortSchedule;
        context.cancel = __CFMachPortCancel;
        context.perform = __CFMachPortPerform;
        result = CFRunLoopSourceCreate(allocator, order, &context);
        mp->_source = result;
        if (result) {
            if (pthread_create(&mp->_thread, NULL, __CFMachPortWatch, mp) == 0) mp->_threadStarted = true;
            result = mp->_source;
        }
    }
    if (result) CFRetain(result);
    pthread_mutex_unlock(&mp->_lock);
    return result;
}

static void __CFMachPortDeallocate(CFTypeRef cf) {
    struct __CFMachPort *mp = (struct __CFMachPort *)cf;
    CFMachPortInvalidate((CFMachPortRef)mp);
    if (mp->_runLoops) CFRelease(mp->_runLoops);
    pthread_cond_destroy(&mp->_cond);
    pthread_mutex_destroy(&mp->_lock);
}

static CFStringRef __CFMachPortCopyDescription(CFTypeRef cf) {
    struct __CFMachPort *mp = (struct __CFMachPort *)cf;
    return CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("<CFMachPort %p [%p]>{valid = %s, port = %x}"), cf, CFGetAllocator(cf), mp->_valid ? "Yes" : "No", mp->_port);
}
