// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFFileDescriptor: a file descriptor watched for readability and writability by two dispatch sources, signalled to a run loop source.
#include "CFInternal.h"
#include "CFRuntime_Internal.h"
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <unistd.h>

typedef int CFFileDescriptorNativeDescriptor;
typedef struct __CFFileDescriptor *CFFileDescriptorRef;
typedef void (*CFFileDescriptorCallBack)(CFFileDescriptorRef f, CFOptionFlags callBackTypes, void *info);
typedef struct {
    CFIndex version;
    void *info;
    void *(*retain)(void *info);
    void (*release)(void *info);
    CFStringRef (*copyDescription)(void *info);
} CFFileDescriptorContext;

enum { kCFFileDescriptorReadCallBack = 1 << 0, kCFFileDescriptorWriteCallBack = 1 << 1 };

struct __CFFileDescriptor {
    CFRuntimeBase _base;
    pthread_mutex_t _lock;
    CFFileDescriptorNativeDescriptor _fd;
    Boolean _closeOnInvalidate;
    Boolean _valid;
    CFFileDescriptorCallBack _callout;
    CFFileDescriptorContext _context;
    CFOptionFlags _enabled;
    CFOptionFlags _fired;
    dispatch_queue_t _queue;
    dispatch_source_t _readSource;
    dispatch_source_t _writeSource;
    Boolean _readResumed;
    Boolean _writeResumed;
    CFRunLoopSourceRef _source;
    CFMutableArrayRef _runLoops;
};

static CFTypeID __kCFFileDescriptorTypeID = _kCFRuntimeNotATypeID;

static void __CFFileDescriptorFired(struct __CFFileDescriptor *f, unsigned long which) {
    pthread_mutex_lock(&f->_lock);
    dispatch_source_t source;
    if (which == 2) {
        if (!f->_writeResumed) goto done;
        f->_writeResumed = false;
        source = f->_writeSource;
    } else {
        if (which != 1 || !f->_readResumed) goto done;
        f->_readResumed = false;
        source = f->_readSource;
    }
    dispatch_suspend(source);
    if (f->_valid) {
        f->_fired |= which;
        if (f->_source) {
            CFRunLoopSourceSignal(f->_source);
            if (f->_runLoops) {
                for (CFIndex i = 0; i < CFArrayGetCount(f->_runLoops); i++) CFRunLoopWakeUp((CFRunLoopRef)CFArrayGetValueAtIndex(f->_runLoops, i));
            }
        }
    }
done:
    pthread_mutex_unlock(&f->_lock);
}

CFFileDescriptorNativeDescriptor CFFileDescriptorGetNativeDescriptor(CFFileDescriptorRef f) {
    return f->_fd;
}

void CFFileDescriptorGetContext(CFFileDescriptorRef f, CFFileDescriptorContext *context) {
    if (context) *context = f->_context;
}

void CFFileDescriptorEnableCallBacks(CFFileDescriptorRef f, CFOptionFlags callBackTypes) {
    pthread_mutex_lock(&f->_lock);
    if (f->_valid) {
        f->_enabled |= callBackTypes;
        if ((callBackTypes & kCFFileDescriptorReadCallBack) && !f->_readResumed && !(f->_fired & kCFFileDescriptorReadCallBack)) {
            f->_readResumed = true;
            dispatch_resume(f->_readSource);
        }
        if ((callBackTypes & kCFFileDescriptorWriteCallBack) && !f->_writeResumed && !(f->_fired & kCFFileDescriptorWriteCallBack)) {
            f->_writeResumed = true;
            dispatch_resume(f->_writeSource);
        }
    }
    pthread_mutex_unlock(&f->_lock);
}

void CFFileDescriptorDisableCallBacks(CFFileDescriptorRef f, CFOptionFlags callBackTypes) {
    pthread_mutex_lock(&f->_lock);
    f->_enabled &= ~callBackTypes;
    if ((callBackTypes & kCFFileDescriptorReadCallBack) && f->_readResumed) {
        f->_readResumed = false;
        dispatch_suspend(f->_readSource);
    }
    if ((callBackTypes & kCFFileDescriptorWriteCallBack) && f->_writeResumed) {
        f->_writeResumed = false;
        dispatch_suspend(f->_writeSource);
    }
    pthread_mutex_unlock(&f->_lock);
}

void CFFileDescriptorInvalidate(CFFileDescriptorRef f) {
    pthread_mutex_lock(&f->_lock);
    if (!f->_valid) {
        pthread_mutex_unlock(&f->_lock);
        return;
    }
    f->_valid = false;
    f->_enabled = 0;
    f->_fired = 0;
    dispatch_source_t readSource = f->_readSource, writeSource = f->_writeSource;
    Boolean writeResumed = f->_writeResumed, readResumed = f->_readResumed;
    dispatch_source_cancel(readSource);
    if (!readResumed) dispatch_resume(readSource);
    dispatch_release(readSource);
    dispatch_source_cancel(writeSource);
    if (!writeResumed) dispatch_resume(writeSource);
    dispatch_release(writeSource);
    f->_readSource = NULL;
    f->_writeSource = NULL;
    f->_readResumed = f->_writeResumed = false;
    int fd = f->_fd;
    dispatch_queue_t queue = f->_queue;
    f->_queue = NULL;
    CFRunLoopSourceRef source = f->_source;
    f->_source = NULL;
    void *info = f->_context.info;
    void (*release)(void *) = f->_context.release;
    f->_context.info = NULL;
    Boolean closeFD = f->_closeOnInvalidate;
    pthread_mutex_unlock(&f->_lock);
    dispatch_sync(queue, ^{ });
    dispatch_release(queue);
    if (closeFD) close(fd);
    if (source) {
        CFRunLoopSourceInvalidate(source);
        CFRelease(source);
    }
    if (info && release) release(info);
}

Boolean CFFileDescriptorIsValid(CFFileDescriptorRef f) {
    return f->_valid;
}

static void __CFFileDescriptorSchedule(void *info, CFRunLoopRef rl, CFStringRef mode) {
    struct __CFFileDescriptor *f = (struct __CFFileDescriptor *)info;
    pthread_mutex_lock(&f->_lock);
    if (!f->_runLoops) f->_runLoops = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    CFArrayAppendValue(f->_runLoops, rl);
    if (f->_fired && f->_source) {
        CFRunLoopSourceSignal(f->_source);
        if (f->_runLoops) {
            for (CFIndex i = 0; i < CFArrayGetCount(f->_runLoops); i++) CFRunLoopWakeUp((CFRunLoopRef)CFArrayGetValueAtIndex(f->_runLoops, i));
        }
    }
    pthread_mutex_unlock(&f->_lock);
}

static void __CFFileDescriptorCancel(void *info, CFRunLoopRef rl, CFStringRef mode) {
    struct __CFFileDescriptor *f = (struct __CFFileDescriptor *)info;
    pthread_mutex_lock(&f->_lock);
    if (f->_runLoops) {
        CFIndex idx = CFArrayGetFirstIndexOfValue(f->_runLoops, CFRangeMake(0, CFArrayGetCount(f->_runLoops)), rl);
        if (idx != kCFNotFound) CFArrayRemoveValueAtIndex(f->_runLoops, idx);
    }
    pthread_mutex_unlock(&f->_lock);
}

static void __CFFileDescriptorPerform(void *info) {
    struct __CFFileDescriptor *f = (struct __CFFileDescriptor *)info;
    CFRetain(f);
    pthread_mutex_lock(&f->_lock);
    CFOptionFlags fired = f->_enabled & f->_fired;
    f->_fired ^= fired;
    f->_enabled ^= fired;
    Boolean valid = f->_valid;
    CFFileDescriptorCallBack callout = f->_callout;
    void *ctxInfo = f->_context.info;
    pthread_mutex_unlock(&f->_lock);
    if (callout && valid && fired) callout(f, fired, ctxInfo);
    CFRelease(f);
}

CFRunLoopSourceRef CFFileDescriptorCreateRunLoopSource(CFAllocatorRef allocator, CFFileDescriptorRef f, CFIndex order) {
    pthread_mutex_lock(&f->_lock);
    if (!f->_valid) {
        pthread_mutex_unlock(&f->_lock);
        return NULL;
    }
    CFRunLoopSourceRef result = f->_source;
    if (!result) {
        CFRunLoopSourceContext context = {0};
        context.info = f;
        context.retain = (const void *(*)(const void *))CFRetain;
        context.release = (void (*)(const void *))CFRelease;
        context.schedule = __CFFileDescriptorSchedule;
        context.cancel = __CFFileDescriptorCancel;
        context.perform = __CFFileDescriptorPerform;
        result = CFRunLoopSourceCreate(allocator, order, &context);
        f->_source = result;
    }
    CFRetain(result);
    pthread_mutex_unlock(&f->_lock);
    return result;
}

static void __CFFileDescriptorDeallocate(CFTypeRef cf) {
    struct __CFFileDescriptor *f = (struct __CFFileDescriptor *)cf;
    CFFileDescriptorInvalidate(f);
    if (f->_runLoops) CFRelease(f->_runLoops);
    pthread_mutex_destroy(&f->_lock);
}

static CFStringRef __CFFileDescriptorCopyDescription(CFTypeRef cf) {
    struct __CFFileDescriptor *f = (struct __CFFileDescriptor *)cf;
    return CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("<CFFileDescriptor %p [%p]>{valid = %s, fd = %d}"), cf, CFGetAllocator(cf), f->_valid ? "Yes" : "No", f->_fd);
}

static const CFRuntimeClass __CFFileDescriptorClass = {
    0, "CFFileDescriptor", NULL, NULL, __CFFileDescriptorDeallocate, NULL, NULL, NULL, __CFFileDescriptorCopyDescription,
};

CFTypeID CFFileDescriptorGetTypeID(void) {
    static dispatch_once_t once;
    dispatch_once(&once, ^{ __kCFFileDescriptorTypeID = _CFRuntimeRegisterClass(&__CFFileDescriptorClass); });
    return __kCFFileDescriptorTypeID;
}

CFFileDescriptorRef CFFileDescriptorCreate(CFAllocatorRef allocator, CFFileDescriptorNativeDescriptor descriptor, Boolean closeOnInvalidate, CFFileDescriptorCallBack callout, const CFFileDescriptorContext *context) {
    if (descriptor < 0) return NULL;
    CFTypeID typeID = CFFileDescriptorGetTypeID();
    struct __CFFileDescriptor *f = (struct __CFFileDescriptor *)_CFRuntimeCreateInstance(allocator, typeID, sizeof(struct __CFFileDescriptor) - sizeof(CFRuntimeBase), NULL);
    if (!f) return NULL;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&f->_lock, &attr);
    pthread_mutexattr_destroy(&attr);
    f->_fd = descriptor;
    f->_closeOnInvalidate = closeOnInvalidate;
    f->_valid = true;
    f->_callout = callout;
    if (context) {
        f->_context = *context;
        if (context->retain) f->_context.info = context->retain(context->info);
    }
    f->_queue = dispatch_queue_create("org.cutefish.reef.CFFileDescriptor", NULL);
    f->_readSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, descriptor, 0, f->_queue);
    f->_writeSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_WRITE, descriptor, 0, f->_queue);
    dispatch_source_set_event_handler(f->_readSource, ^{ __CFFileDescriptorFired(f, 1); });
    dispatch_source_set_event_handler(f->_writeSource, ^{ __CFFileDescriptorFired(f, 2); });
    return f;
}
