// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFNotificationCenter: the local and distributed centers are forwarded to NSNotificationCenter and NSDistributedNotificationCenter when
// Foundation is loaded; the Darwin center, and every center without Foundation, keeps its own table of observers.
#include "CFInternal.h"
#include "CFRuntime_Internal.h"
#include <pthread.h>
#include <stdlib.h>

extern Class objc_getClass(const char *name);
extern SEL sel_registerName(const char *name);
extern id objc_msgSend(id self, SEL op, ...);

typedef struct __CFNotificationCenter *CFNotificationCenterRef;
typedef void (*CFNotificationCallback)(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo);

enum { kLocalCenter = 0, kDistributedCenter = 1, kDarwinCenter = 2 };

struct observer {
    const void *observer;
    CFNotificationCallback callback;
    CFStringRef name;
    const void *object;
    id token;
};

struct __CFNotificationCenter {
    CFRuntimeBase _base;
    int _kind;
    pthread_mutex_t _lock;
    struct observer *_observers;
    CFIndex _count;
    CFIndex _capacity;
};

static CFTypeID centerTypeID;
static CFNotificationCenterRef centers[3];
static pthread_once_t centersOnce = PTHREAD_ONCE_INIT;

static const CFRuntimeClass centerClass = {
    0, "CFNotificationCenter", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

static void createCenters(void) {
    centerTypeID = _CFRuntimeRegisterClass(&centerClass);
    for (int i = 0; i < 3; i++) {
        struct __CFNotificationCenter *c = (struct __CFNotificationCenter *)_CFRuntimeCreateInstance(kCFAllocatorSystemDefault, centerTypeID, sizeof(struct __CFNotificationCenter) - sizeof(CFRuntimeBase), NULL);
        c->_kind = i;
        pthread_mutex_init(&c->_lock, NULL);
        centers[i] = c;
    }
}

CFTypeID CFNotificationCenterGetTypeID(void) {
    pthread_once(&centersOnce, createCenters);
    return centerTypeID;
}

CFNotificationCenterRef CFNotificationCenterGetLocalCenter(void) {
    pthread_once(&centersOnce, createCenters);
    return centers[kLocalCenter];
}

CFNotificationCenterRef CFNotificationCenterGetDistributedCenter(void) {
    pthread_once(&centersOnce, createCenters);
    return centers[kDistributedCenter];
}

CFNotificationCenterRef CFNotificationCenterGetDarwinNotifyCenter(void) {
    pthread_once(&centersOnce, createCenters);
    return centers[kDarwinCenter];
}

// The Foundation center that stands for this one, or nil.
static id foundationCenter(struct __CFNotificationCenter *center) {
    const char *name = center->_kind == kLocalCenter ? "NSNotificationCenter" : center->_kind == kDistributedCenter ? "NSDistributedNotificationCenter" : NULL;
    if (!name) return nil;
    Class cls = objc_getClass(name);
    if (!cls) return nil;
    return ((id (*)(id, SEL))objc_msgSend)((id)cls, sel_registerName("defaultCenter"));
}

void CFNotificationCenterAddObserver(CFNotificationCenterRef center, const void *observer, CFNotificationCallback callBack, CFStringRef name, const void *object, CFIndex suspensionBehavior) {
    struct __CFNotificationCenter *c = (struct __CFNotificationCenter *)center;
    if (!c || !callBack) return;
    if (c->_kind == kDarwinCenter) {
        if (!name) return;
        object = NULL;
    }
    CFStringRef nameCopy = name ? CFStringCreateCopy(NULL, name) : NULL;
    id token = nil;
    id nc = foundationCenter(c);
    if (nc) {
        id obj = ((id (*)(id, SEL, CFStringRef, const void *, id, void (^)(id)))objc_msgSend)(nc, sel_registerName("addObserverForName:object:queue:usingBlock:"), name, object, nil, ^(id note) {
            CFStringRef n = ((CFStringRef (*)(id, SEL))objc_msgSend)(note, sel_registerName("name"));
            const void *o = ((const void *(*)(id, SEL))objc_msgSend)(note, sel_registerName("object"));
            CFDictionaryRef info = ((CFDictionaryRef (*)(id, SEL))objc_msgSend)(note, sel_registerName("userInfo"));
            callBack(center, (void *)observer, n, o, info);
        });
        if (obj) {
            ((id (*)(id, SEL))objc_msgSend)(obj, sel_registerName("retain"));
            token = obj;
        }
    }
    pthread_mutex_lock(&c->_lock);
    if (c->_count == c->_capacity) {
        c->_capacity = c->_count ? c->_count * 2 : 16;
        c->_observers = (struct observer *)realloc(c->_observers, c->_capacity * sizeof(struct observer));
    }
    struct observer *e = &c->_observers[c->_count++];
    e->observer = observer;
    e->callback = callBack;
    e->name = nameCopy;
    e->object = object;
    e->token = token;
    pthread_mutex_unlock(&c->_lock);
}

static void removeMatching(CFNotificationCenterRef center, const void *observer, CFStringRef name, const void *object, Boolean everyObserver) {
    struct __CFNotificationCenter *c = (struct __CFNotificationCenter *)center;
    if (!c) return;
    id nc = foundationCenter(c);
    pthread_mutex_lock(&c->_lock);
    CFIndex kept = 0;
    for (CFIndex i = 0; i < c->_count; i++) {
        struct observer *e = &c->_observers[i];
        Boolean match = e->observer == observer;
        if (match && !everyObserver) {
            if (name && (!e->name || !CFEqual(e->name, name))) match = false;
            if (object && c->_kind != kDarwinCenter) match = match && e->object == object;
        }
        if (match) {
            if (e->token) {
                if (nc) ((void (*)(id, SEL, id))objc_msgSend)(nc, sel_registerName("removeObserver:"), e->token);
                ((void (*)(id, SEL))objc_msgSend)(e->token, sel_registerName("release"));
            }
            if (e->name) CFRelease(e->name);
        } else {
            c->_observers[kept++] = *e;
        }
    }
    c->_count = kept;
    pthread_mutex_unlock(&c->_lock);
}

void CFNotificationCenterRemoveObserver(CFNotificationCenterRef center, const void *observer, CFStringRef name, const void *object) {
    removeMatching(center, observer, name, object, false);
}

void CFNotificationCenterRemoveEveryObserver(CFNotificationCenterRef center, const void *observer) {
    removeMatching(center, observer, NULL, NULL, true);
}

void CFNotificationCenterPostNotificationWithOptions(CFNotificationCenterRef center, CFStringRef name, const void *object, CFDictionaryRef userInfo, CFOptionFlags options) {
    struct __CFNotificationCenter *c = (struct __CFNotificationCenter *)center;
    if (!c || !name) return;
    id nc = foundationCenter(c);
    if (nc) {
        if (c->_kind == kDistributedCenter) {
            ((void (*)(id, SEL, CFStringRef, const void *, CFDictionaryRef, BOOL))objc_msgSend)(nc, sel_registerName("postNotificationName:object:userInfo:deliverImmediately:"), name, object, userInfo, options & 1);
        } else {
            ((void (*)(id, SEL, CFStringRef, const void *, CFDictionaryRef))objc_msgSend)(nc, sel_registerName("postNotificationName:object:userInfo:"), name, object, userInfo);
        }
        return;
    }
    if (c->_kind == kDarwinCenter) {
        object = NULL;
        userInfo = NULL;
    }
    pthread_mutex_lock(&c->_lock);
    CFIndex count = c->_count;
    struct observer *copy = (struct observer *)malloc((count ? count : 1) * sizeof(struct observer));
    CFIndex n = 0;
    for (CFIndex i = 0; i < count; i++) {
        struct observer *e = &c->_observers[i];
        if ((!e->name || CFEqual(e->name, name)) && (!e->object || e->object == object)) copy[n++] = *e;
    }
    pthread_mutex_unlock(&c->_lock);
    for (CFIndex i = 0; i < n; i++) copy[i].callback(center, (void *)copy[i].observer, name, object, userInfo);
    free(copy);
}

void CFNotificationCenterPostNotification(CFNotificationCenterRef center, CFStringRef name, const void *object, CFDictionaryRef userInfo, Boolean deliverImmediately) {
    CFNotificationCenterPostNotificationWithOptions(center, name, object, userInfo, deliverImmediately);
}
