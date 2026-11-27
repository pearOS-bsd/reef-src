// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFUserNotification: an alert shown by AppKit (NSAlert, run modally on the main queue) when the process has an NSApp, otherwise as a desktop
// notification of org.freedesktop.Notifications through the D-Bus helpers of the host (_reefDBusCall).
#include "CFInternal.h"
#include "CFRuntime_Internal.h"
#include <dispatch/dispatch.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

extern Class objc_getClass(const char *name);
extern SEL sel_registerName(const char *name);
extern id objc_msgSend(id self, SEL op, ...);
extern void *dlsym(void *handle, const char *symbol);
extern const char *getprogname(void);
extern Boolean _CFIsMainThread(void);

#define MSG(rettype, ...) ((rettype (*)(id, SEL, ...))objc_msgSend)
#define SEND(obj, name) ((id (*)(id, SEL))objc_msgSend)((id)(obj), sel_registerName(name))
#define SEND1(obj, name, type, a) ((id (*)(id, SEL, type))objc_msgSend)((id)(obj), sel_registerName(name), a)

typedef struct __CFUserNotification *CFUserNotificationRef;
typedef void (*CFUserNotificationCallBack)(CFUserNotificationRef userNotification, CFOptionFlags responseFlags);
typedef struct { double x, y, w, h; } Frame;
SInt32 CFUserNotificationCancel(CFUserNotificationRef userNotification);

const CFStringRef kCFUserNotificationIconURLKey = CFSTR("IconURL");
const CFStringRef kCFUserNotificationSoundURLKey = CFSTR("SoundURL");
const CFStringRef kCFUserNotificationLocalizationURLKey = CFSTR("LocalizationURL");
const CFStringRef kCFUserNotificationAlertHeaderKey = CFSTR("AlertHeader");
const CFStringRef kCFUserNotificationAlertMessageKey = CFSTR("AlertMessage");
const CFStringRef kCFUserNotificationDefaultButtonTitleKey = CFSTR("DefaultButtonTitle");
const CFStringRef kCFUserNotificationAlternateButtonTitleKey = CFSTR("AlternateButtonTitle");
const CFStringRef kCFUserNotificationOtherButtonTitleKey = CFSTR("OtherButtonTitle");
const CFStringRef kCFUserNotificationProgressIndicatorValueKey = CFSTR("ProgressIndicatorValue");
const CFStringRef kCFUserNotificationPopUpTitlesKey = CFSTR("PopUpTitles");
const CFStringRef kCFUserNotificationTextFieldTitlesKey = CFSTR("TextFieldTitles");
const CFStringRef kCFUserNotificationCheckBoxTitlesKey = CFSTR("CheckBoxTitles");
const CFStringRef kCFUserNotificationTextFieldValuesKey = CFSTR("TextFieldValues");
const CFStringRef kCFUserNotificationPopUpSelectionKey = CFSTR("PopUpSelection");
const CFStringRef kCFUserNotificationAlertTopMostKey = CFSTR("AlertTopMost");
const CFStringRef kCFUserNotificationKeyboardTypesKey = CFSTR("KeyboardTypes");

enum {
    kNoDefaultButtonFlag = 1 << 5,
    kUseRadioButtonsFlag = 1 << 6,
    kSecureTextFieldFlag = 1 << 16,
};

struct __CFUserNotification {
    CFRuntimeBase _base;
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
    CFDictionaryRef _parameters;
    CFOptionFlags _flags;
    double _timeout;
    Boolean _responded;
    Boolean _cancelled;
    CFOptionFlags _responseFlags;
    CFMutableDictionaryRef _responseDictionary;
    int _desktopID;
    id _alert;
    CFUserNotificationCallBack _callout;
    CFRunLoopSourceRef _source;
};

static CFTypeID notificationTypeID;
static pthread_once_t notificationOnce = PTHREAD_ONCE_INIT;
static pthread_mutex_t desktopLock = PTHREAD_MUTEX_INITIALIZER;
static CFMutableDictionaryRef desktopNotes;

static void finalizeNotification(CFTypeRef cf) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)cf;
    if (n->_parameters) CFRelease(n->_parameters);
    if (n->_responseDictionary) CFRelease(n->_responseDictionary);
    if (n->_source) CFRelease(n->_source);
    pthread_mutex_destroy(&n->_lock);
    pthread_cond_destroy(&n->_cond);
}

static const CFRuntimeClass notificationClass = {
    0, "CFUserNotification", NULL, NULL, finalizeNotification, NULL, NULL, NULL, NULL,
};

static void registerNotification(void) {
    notificationTypeID = _CFRuntimeRegisterClass(&notificationClass);
}

CFTypeID CFUserNotificationGetTypeID(void) {
    pthread_once(&notificationOnce, registerNotification);
    return notificationTypeID;
}

// The string at a key of the parameters: the value itself, or the element of an array of strings.
static CFStringRef stringFor(CFDictionaryRef dict, CFStringRef key, CFIndex index) {
    if (!dict) return NULL;
    CFTypeRef value = CFDictionaryGetValue(dict, key);
    if (!value) return NULL;
    if (CFGetTypeID(value) != CFStringGetTypeID()) {
        if (CFGetTypeID(value) != CFArrayGetTypeID() || CFArrayGetCount((CFArrayRef)value) <= index) return NULL;
        value = CFArrayGetValueAtIndex((CFArrayRef)value, index);
        if (!value) return NULL;
        if (CFGetTypeID(value) != CFStringGetTypeID()) return NULL;
    }
    return (CFStringRef)value;
}

static void respond(struct __CFUserNotification *n, CFOptionFlags flags, CFDictionaryRef responses) {
    pthread_mutex_lock(&n->_lock);
    if (n->_responded) {
        pthread_mutex_unlock(&n->_lock);
        return;
    }
    n->_responded = true;
    n->_responseFlags = flags;
    if (responses) {
        CFDictionaryRef copy = CFDictionaryCreateCopy(kCFAllocatorSystemDefault, responses);
        CFRelease(n->_responseDictionary);
        n->_responseDictionary = (CFMutableDictionaryRef)copy;
    }
    CFRunLoopSourceRef source = n->_source;
    pthread_cond_broadcast(&n->_cond);
    pthread_mutex_unlock(&n->_lock);
    if (source) {
        CFRunLoopSourceSignal(source);
        CFRunLoopWakeUp(CFRunLoopGetMain());
    }
}

static void performCallout(void *info) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)info;
    pthread_mutex_lock(&n->_lock);
    CFOptionFlags flags = n->_responseFlags;
    CFUserNotificationCallBack callout = n->_callout;
    n->_callout = NULL;
    Boolean responded = n->_responded;
    pthread_mutex_unlock(&n->_lock);
    if (callout && responded) callout(n, flags);
}

typedef CFArrayRef (*DBusCall)(CFStringRef service, CFStringRef path, CFStringRef interface, CFStringRef method, CFStringRef signature, CFArrayRef args, void *reserved);
typedef void (*DBusAddSignalHandler)(CFStringRef interface, CFStringRef signal, void *handler);

static void closeOnDesktop(struct __CFUserNotification *n) {
    if (!n->_desktopID) return;
    DBusCall call = (DBusCall)dlsym((void *)-2, "__reefDBusCall");
    long long ident = (unsigned int)n->_desktopID;
    CFNumberRef key = CFNumberCreate(kCFAllocatorSystemDefault, kCFNumberLongLongType, &ident);
    pthread_mutex_lock(&desktopLock);
    if (desktopNotes) CFDictionaryRemoveValue(desktopNotes, key);
    pthread_mutex_unlock(&desktopLock);
    if (call) {
        CFArrayRef args = CFArrayCreate(kCFAllocatorSystemDefault, (const void **)&key, 1, &kCFTypeArrayCallBacks);
        call(CFSTR("org.freedesktop.Notifications"), CFSTR("/org/freedesktop/Notifications"), CFSTR("org.freedesktop.Notifications"), CFSTR("CloseNotification"), CFSTR("u"), args, NULL);
        CFRelease(args);
    }
    CFRelease(key);
}

static struct __CFUserNotification *desktopNotification(CFTypeRef key) {
    pthread_mutex_lock(&desktopLock);
    struct __CFUserNotification *n = desktopNotes ? (struct __CFUserNotification *)CFDictionaryGetValue(desktopNotes, key) : NULL;
    if (n) CFRetain(n);
    else pthread_mutex_unlock(&desktopLock);
    return n;
}

static Boolean showOnDesktop(struct __CFUserNotification *n) {
    DBusCall call = (DBusCall)dlsym((void *)-2, "__reefDBusCall");
    DBusAddSignalHandler addHandler = (DBusAddSignalHandler)dlsym((void *)-2, "__reefDBusAddSignalHandler");
    if (!addHandler || !call) return false;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        desktopNotes = CFDictionaryCreateMutable(kCFAllocatorSystemDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        addHandler(CFSTR("org.freedesktop.Notifications"), CFSTR("ActionInvoked"), ^(void *unused, CFArrayRef args) {
            if (CFArrayGetCount(args) < 2) return;
            CFTypeRef key = CFArrayGetValueAtIndex(args, 0);
            struct __CFUserNotification *note = desktopNotification(key);
            if (!note) return;
            pthread_mutex_unlock(&desktopLock);
            CFStringRef action = (CFStringRef)CFArrayGetValueAtIndex(args, 1);
            CFOptionFlags response = 0;
            if (CFEqual(action, CFSTR("alternate"))) response = 1;
            else if (CFEqual(action, CFSTR("other"))) response = 2;
            pthread_mutex_lock(&desktopLock);
            if (desktopNotes) CFDictionaryRemoveValue(desktopNotes, key);
            pthread_mutex_unlock(&desktopLock);
            respond(note, response, NULL);
            CFRelease(note);
        });
        addHandler(CFSTR("org.freedesktop.Notifications"), CFSTR("NotificationClosed"), ^(void *unused, CFArrayRef args) {
            if (CFArrayGetCount(args) < 1) return;
            CFTypeRef key = CFArrayGetValueAtIndex(args, 0);
            struct __CFUserNotification *note = desktopNotification(key);
            if (!note) return;
            pthread_mutex_unlock(&desktopLock);
            pthread_mutex_lock(&desktopLock);
            if (desktopNotes) CFDictionaryRemoveValue(desktopNotes, key);
            pthread_mutex_unlock(&desktopLock);
            respond(note, 3, NULL);
            CFRelease(note);
        });
    });
    CFDictionaryRef params = n->_parameters;
    CFAllocatorRef alloc = kCFAllocatorSystemDefault;
    CFMutableArrayRef actions = CFArrayCreateMutable(alloc, 0, &kCFTypeArrayCallBacks);
    if (!(n->_flags & kNoDefaultButtonFlag)) {
        CFStringRef title = stringFor(params, kCFUserNotificationDefaultButtonTitleKey, 0);
        CFArrayAppendValue(actions, CFSTR("default"));
        CFArrayAppendValue(actions, title ? title : CFSTR("OK"));
    }
    CFStringRef alternate = stringFor(params, kCFUserNotificationAlternateButtonTitleKey, 0);
    CFStringRef other = stringFor(params, kCFUserNotificationOtherButtonTitleKey, 0);
    if (alternate) {
        CFArrayAppendValue(actions, CFSTR("alternate"));
        CFArrayAppendValue(actions, alternate);
    }
    if (other) {
        CFArrayAppendValue(actions, CFSTR("other"));
        CFArrayAppendValue(actions, other);
    }
    CFMutableDictionaryRef hints = CFDictionaryCreateMutable(alloc, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(hints, CFSTR("resident"), kCFBooleanTrue);
    char urgencyValue = ((n->_flags & 3) == 0) + 1;
    CFNumberRef urgency = CFNumberCreate(alloc, kCFNumberCharType, &urgencyValue);
    CFDictionarySetValue(hints, CFSTR("urgency"), urgency);
    CFRelease(urgency);
    CFStringRef header = stringFor(params, kCFUserNotificationAlertHeaderKey, 0);
    CFStringRef message = stringFor(params, kCFUserNotificationAlertMessageKey, 0);
    int replaces = 0, expire = n->_timeout > 0.0 ? (int)(n->_timeout * 1000.0) : 0;
    CFNumberRef replacesNumber = CFNumberCreate(alloc, kCFNumberIntType, &replaces);
    CFNumberRef expireNumber = CFNumberCreate(alloc, kCFNumberIntType, &expire);
    CFStringRef appName = CFStringCreateWithCString(alloc, getprogname(), kCFStringEncodingUTF8);
    const void *values[8] = { appName, replacesNumber, CFSTR("dialog-information"), header ? header : CFSTR(""), message ? message : CFSTR(""), actions, hints, expireNumber };
    CFArrayRef args = CFArrayCreate(alloc, values, 8, &kCFTypeArrayCallBacks);
    CFArrayRef reply = call(CFSTR("org.freedesktop.Notifications"), CFSTR("/org/freedesktop/Notifications"), CFSTR("org.freedesktop.Notifications"), CFSTR("Notify"), CFSTR("susssasa{sv}i"), args, NULL);
    CFRelease(args);
    CFRelease(appName);
    CFRelease(replacesNumber);
    CFRelease(expireNumber);
    CFRelease(actions);
    CFRelease(hints);
    if (reply && CFArrayGetCount(reply) > 0) {
        long long ident = 0;
        CFNumberGetValue((CFNumberRef)CFArrayGetValueAtIndex(reply, 0), kCFNumberLongLongType, &ident);
        n->_desktopID = (int)ident;
        long long wide = ident & 0xffffffff;
        CFNumberRef key = CFNumberCreate(alloc, kCFNumberLongLongType, &wide);
        pthread_mutex_lock(&desktopLock);
        CFDictionarySetValue(desktopNotes, key, n);
        pthread_mutex_unlock(&desktopLock);
        CFRelease(key);
        return true;
    }
    return false;
}

static CFIndex titleCount(CFDictionaryRef params, CFStringRef key) {
    CFTypeRef value = CFDictionaryGetValue(params, key);
    if (!value) return 0;
    if (CFGetTypeID(value) == CFArrayGetTypeID()) return CFArrayGetCount((CFArrayRef)value);
    return CFGetTypeID(value) == CFStringGetTypeID();
}

// Builds and runs the alert on the main thread.
static void present(struct __CFUserNotification *n) {
    CFAllocatorRef alloc = kCFAllocatorSystemDefault;
    CFMutableArrayRef fields = CFArrayCreateMutable(alloc, 0, &kCFTypeArrayCallBacks);
    CFMutableArrayRef boxes = CFArrayCreateMutable(alloc, 0, &kCFTypeArrayCallBacks);
    CFDictionaryRef params = n->_parameters;
    id alert = SEND(SEND(objc_getClass("NSAlert"), "alloc"), "init");
    CFStringRef header = stringFor(params, kCFUserNotificationAlertHeaderKey, 0);
    CFStringRef message = stringFor(params, kCFUserNotificationAlertMessageKey, 0);
    SEND1(alert, "setMessageText:", CFStringRef, header ? header : CFSTR(""));
    SEND1(alert, "setInformativeText:", CFStringRef, message ? message : CFSTR(""));
    CFOptionFlags level = n->_flags & 3;
    long style = level == 0 ? 2 : (level != 2);
    SEND1(alert, "setAlertStyle:", long, style);
    if (!(n->_flags & kNoDefaultButtonFlag)) {
        CFStringRef title = stringFor(params, kCFUserNotificationDefaultButtonTitleKey, 0);
        SEND1(alert, "addButtonWithTitle:", CFStringRef, title ? title : CFSTR("OK"));
    }
    CFStringRef alternate = stringFor(params, kCFUserNotificationAlternateButtonTitleKey, 0);
    CFStringRef other = stringFor(params, kCFUserNotificationOtherButtonTitleKey, 0);
    if (alternate) SEND1(alert, "addButtonWithTitle:", CFStringRef, alternate);
    if (other) SEND1(alert, "addButtonWithTitle:", CFStringRef, other);
    CFIndex fieldCount = 0, boxCount = 0, popUpCount = 0;
    if (params) {
        fieldCount = titleCount(params, kCFUserNotificationTextFieldTitlesKey);
        boxCount = titleCount(params, kCFUserNotificationCheckBoxTitlesKey);
        popUpCount = titleCount(params, kCFUserNotificationPopUpTitlesKey);
    }
    id popUp = nil;
    if ((popUpCount != 0) + fieldCount + boxCount != 0) {
        double rows = fieldCount + boxCount + (popUpCount != 0), y = rows * 26.0;
        Frame frame = { 0, 0, 300, rows * 26.0 };
        id view = ((id (*)(id, SEL, Frame))objc_msgSend)(SEND(objc_getClass("NSView"), "alloc"), sel_registerName("initWithFrame:"), frame);
        for (CFIndex i = 0; i < fieldCount; i++) {
            const char *cls = (n->_flags >> i) & kSecureTextFieldFlag ? "NSSecureTextField" : "NSTextField";
            y -= 26.0;
            Frame f = { 0, y, 300, 22 };
            id field = ((id (*)(id, SEL, Frame))objc_msgSend)(SEND(objc_getClass(cls), "alloc"), sel_registerName("initWithFrame:"), f);
            CFStringRef title = stringFor(params, kCFUserNotificationTextFieldTitlesKey, i);
            CFStringRef value = stringFor(params, kCFUserNotificationTextFieldValuesKey, i);
            if (title) SEND1(field, "setPlaceholderString:", CFStringRef, title);
            SEND1(field, "setStringValue:", CFStringRef, value ? value : CFSTR(""));
            SEND1(view, "addSubview:", id, field);
            CFArrayAppendValue(fields, field);
            SEND(field, "release");
        }
        for (CFIndex i = 0; i < boxCount; i++) {
            y -= 26.0;
            Frame f = { 0, y, 300, 22 };
            id box = ((id (*)(id, SEL, Frame))objc_msgSend)(SEND(objc_getClass("NSButton"), "alloc"), sel_registerName("initWithFrame:"), f);
            SEND1(box, "setButtonType:", long, (n->_flags & kUseRadioButtonsFlag) ? 4 : 3);
            CFStringRef title = stringFor(params, kCFUserNotificationCheckBoxTitlesKey, i);
            SEND1(box, "setTitle:", CFStringRef, title ? title : CFSTR(""));
            SEND1(box, "setState:", long, (n->_flags >> i >> 8) & 1);
            SEND1(view, "addSubview:", id, box);
            CFArrayAppendValue(boxes, box);
            SEND(box, "release");
        }
        if (popUpCount) {
            y -= 26.0;
            Frame f = { 0, y, 300, 22 };
            popUp = ((id (*)(id, SEL, Frame, BOOL))objc_msgSend)(SEND(objc_getClass("NSPopUpButton"), "alloc"), sel_registerName("initWithFrame:pullsDown:"), f, NO);
            for (CFIndex i = 0; i < popUpCount; i++) {
                CFStringRef title = stringFor(params, kCFUserNotificationPopUpTitlesKey, i);
                SEND1(popUp, "addItemWithTitle:", CFStringRef, title ? title : CFSTR(""));
            }
            SEND1(popUp, "selectItemAtIndex:", long, (n->_flags >> 24) & 0xff);
            SEND1(view, "addSubview:", id, popUp);
        }
        SEND1(alert, "setAccessoryView:", id, view);
        SEND(view, "release");
    }
    pthread_mutex_lock(&n->_lock);
    n->_alert = alert;
    pthread_mutex_unlock(&n->_lock);
    long result = (long)SEND(alert, "runModal");
    pthread_mutex_lock(&n->_lock);
    n->_alert = nil;
    Boolean cancelled = n->_cancelled;
    pthread_mutex_unlock(&n->_lock);
    CFOptionFlags response = 3;
    if (!cancelled && result >= 1000) {
        long button = (result - 1000) + ((n->_flags & kNoDefaultButtonFlag) != 0);
        if (button == 1) button = stringFor(n->_parameters, kCFUserNotificationAlternateButtonTitleKey, 0) == NULL ? 2 : 1;
        response = button == 0 ? 0 : (button != 1) + 1;
    }
    for (CFIndex i = 0; i < CFArrayGetCount(boxes); i++) {
        long state = (long)SEND((id)CFArrayGetValueAtIndex(boxes, i), "state");
        if (state) response |= 0x100L << i;
    }
    if (popUp) response |= (CFOptionFlags)SEND(popUp, "indexOfSelectedItem") << 24;
    CFMutableDictionaryRef responses = CFDictionaryCreateMutable(alloc, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (CFArrayGetCount(fields) != 0) {
        CFMutableArrayRef values = CFArrayCreateMutable(alloc, 0, &kCFTypeArrayCallBacks);
        for (CFIndex i = 0; i < CFArrayGetCount(fields); i++) {
            CFStringRef s = (CFStringRef)SEND((id)CFArrayGetValueAtIndex(fields, i), "stringValue");
            CFArrayAppendValue(values, s ? s : CFSTR(""));
        }
        CFDictionarySetValue(responses, kCFUserNotificationTextFieldValuesKey, values);
        CFRelease(values);
    }
    SEND(alert, "release");
    CFRelease(fields);
    CFRelease(boxes);
    respond(n, response, responses);
    CFRelease(responses);
    CFRelease(n);
}

CFUserNotificationRef CFUserNotificationCreate(CFAllocatorRef allocator, CFTimeInterval timeout, CFOptionFlags flags, SInt32 *error, CFDictionaryRef dictionary) {
    pthread_once(&notificationOnce, registerNotification);
    struct __CFUserNotification *n = (struct __CFUserNotification *)_CFRuntimeCreateInstance(allocator, notificationTypeID, sizeof(struct __CFUserNotification) - sizeof(CFRuntimeBase), NULL);
    if (!n) {
        if (error) *error = -1;
        return NULL;
    }
    memset((char *)n + sizeof(CFRuntimeBase), 0, sizeof(struct __CFUserNotification) - sizeof(CFRuntimeBase));
    pthread_mutex_init(&n->_lock, NULL);
    pthread_cond_init(&n->_cond, NULL);
    n->_parameters = dictionary ? CFDictionaryCreateCopy(kCFAllocatorSystemDefault, dictionary) : NULL;
    n->_flags = flags;
    n->_timeout = timeout;
    n->_responseDictionary = CFDictionaryCreateMutable(kCFAllocatorSystemDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (error) *error = 0;
    void **app;
    if (objc_getClass("NSApplication") && objc_getClass("NSAlert") && (app = (void **)dlsym((void *)-2, "NSApp")) && *app) {
        CFRetain(n);
        dispatch_async(dispatch_get_main_queue(), ^{ present(n); });
    } else if (!showOnDesktop(n)) {
        CFStringRef header = stringFor(n->_parameters, kCFUserNotificationAlertHeaderKey, 0);
        char buffer[1024] = {0};
        if (header) CFStringGetCString(header, buffer, sizeof(buffer), kCFStringEncodingUTF8);
        fprintf(stderr, "%s: no way to show the notice \"%s\"\n", getprogname(), buffer);
        respond(n, 3, NULL);
    }
    if (timeout > 0.0) {
        CFRetain(n);
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(timeout * 1e9)), dispatch_get_global_queue(0, 0), ^{
            CFUserNotificationCancel(n);
            CFRelease(n);
        });
    }
    return n;
}

SInt32 CFUserNotificationCancel(CFUserNotificationRef userNotification) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)userNotification;
    if (!n) return -1;
    pthread_mutex_lock(&n->_lock);
    if (n->_responded) {
        pthread_mutex_unlock(&n->_lock);
        return 0;
    }
    n->_cancelled = true;
    id alert = n->_alert;
    pthread_mutex_unlock(&n->_lock);
    if (!alert) {
        closeOnDesktop(n);
        respond(n, 3, NULL);
        return 0;
    }
    CFStringRef modes[2] = { kCFRunLoopCommonModes, CFSTR("NSModalPanelRunLoopMode") };
    CFArrayRef array = CFArrayCreate(kCFAllocatorSystemDefault, (const void **)modes, 2, &kCFTypeArrayCallBacks);
    id app = nil;
    void **slot;
    if (objc_getClass("NSApplication") && objc_getClass("NSAlert") && (slot = (void **)dlsym((void *)-2, "NSApp"))) app = (id)*slot;
    ((void (*)(id, SEL, SEL, id, BOOL, CFArrayRef))objc_msgSend)(app, sel_registerName("performSelectorOnMainThread:withObject:waitUntilDone:modes:"), sel_registerName("abortModal"), nil, NO, array);
    CFRelease(array);
    return 0;
}

SInt32 CFUserNotificationReceiveResponse(CFUserNotificationRef userNotification, CFTimeInterval timeout, CFOptionFlags *responseFlags) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)userNotification;
    if (!n) return -1;
    CFAbsoluteTime deadline = 0.0;
    if (timeout > 0.0) deadline = CFAbsoluteTimeGetCurrent() + timeout;
    Boolean mainThread = _CFIsMainThread();
    pthread_mutex_lock(&n->_lock);
    while (!n->_responded) {
        if (deadline != 0.0 && deadline <= CFAbsoluteTimeGetCurrent()) break;
        if (!mainThread) {
            struct timespec until;
            clock_gettime(CLOCK_REALTIME, &until);
            until.tv_nsec += 50000000;
            if (until.tv_nsec > 999999999) {
                until.tv_sec += 1;
                until.tv_nsec -= 1000000000;
            }
            pthread_cond_timedwait(&n->_cond, &n->_lock, &until);
        } else {
            pthread_mutex_unlock(&n->_lock);
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.05, true);
            pthread_mutex_lock(&n->_lock);
        }
    }
    Boolean responded = n->_responded;
    if (responseFlags && responded) *responseFlags = n->_responseFlags;
    pthread_mutex_unlock(&n->_lock);
    return responded ? 0 : -1;
}

CFStringRef CFUserNotificationGetResponseValue(CFUserNotificationRef userNotification, CFStringRef key, CFIndex idx) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)userNotification;
    if (!n) return NULL;
    return stringFor(n->_responseDictionary, key, idx);
}

CFDictionaryRef CFUserNotificationGetResponseDictionary(CFUserNotificationRef userNotification) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)userNotification;
    return n ? n->_responseDictionary : NULL;
}

SInt32 CFUserNotificationUpdate(CFUserNotificationRef userNotification, CFTimeInterval timeout, CFOptionFlags flags, CFDictionaryRef dictionary) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)userNotification;
    if (!n) return -1;
    pthread_mutex_lock(&n->_lock);
    if (n->_responded) {
        pthread_mutex_unlock(&n->_lock);
        return -1;
    }
    if (n->_parameters) CFRelease(n->_parameters);
    n->_parameters = dictionary ? CFDictionaryCreateCopy(kCFAllocatorSystemDefault, dictionary) : NULL;
    n->_flags = flags;
    id alert = n->_alert;
    pthread_mutex_unlock(&n->_lock);
    if (!alert) {
        if (n->_desktopID) {
            closeOnDesktop(n);
            showOnDesktop(n);
        }
        return 0;
    }
    CFStringRef header = stringFor(n->_parameters, kCFUserNotificationAlertHeaderKey, 0);
    CFStringRef message = stringFor(n->_parameters, kCFUserNotificationAlertMessageKey, 0);
    dispatch_async(dispatch_get_main_queue(), ^{
        SEND1(alert, "setMessageText:", CFStringRef, header ? header : CFSTR(""));
        SEND1(alert, "setInformativeText:", CFStringRef, message ? message : CFSTR(""));
    });
    return 0;
}

CFRunLoopSourceRef CFUserNotificationCreateRunLoopSource(CFAllocatorRef allocator, CFUserNotificationRef userNotification, CFUserNotificationCallBack callout, CFIndex order) {
    struct __CFUserNotification *n = (struct __CFUserNotification *)userNotification;
    if (!n) return NULL;
    CFRunLoopSourceContext context = {0};
    context.info = n;
    context.retain = (const void *(*)(const void *))CFRetain;
    context.release = (void (*)(const void *))CFRelease;
    context.perform = performCallout;
    CFRunLoopSourceRef source = CFRunLoopSourceCreate(allocator, order, &context);
    pthread_mutex_lock(&n->_lock);
    n->_callout = callout;
    if (n->_source) CFRelease(n->_source);
    n->_source = (CFRunLoopSourceRef)CFRetain(source);
    Boolean responded = n->_responded;
    pthread_mutex_unlock(&n->_lock);
    if (responded) CFRunLoopSourceSignal(source);
    return source;
}

SInt32 CFUserNotificationDisplayNotice(CFTimeInterval timeout, CFOptionFlags flags, CFURLRef iconURL, CFURLRef soundURL, CFURLRef localizationURL, CFStringRef alertHeader, CFStringRef alertMessage, CFStringRef defaultButtonTitle) {
    CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorSystemDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (alertHeader) CFDictionarySetValue(dict, kCFUserNotificationAlertHeaderKey, alertHeader);
    if (alertMessage) CFDictionarySetValue(dict, kCFUserNotificationAlertMessageKey, alertMessage);
    if (defaultButtonTitle) CFDictionarySetValue(dict, kCFUserNotificationDefaultButtonTitleKey, defaultButtonTitle);
    SInt32 error = 0;
    CFUserNotificationRef n = CFUserNotificationCreate(kCFAllocatorDefault, timeout, flags, &error, dict);
    CFRelease(dict);
    if (n) CFRelease(n);
    return error;
}

SInt32 CFUserNotificationDisplayAlert(CFTimeInterval timeout, CFOptionFlags flags, CFURLRef iconURL, CFURLRef soundURL, CFURLRef localizationURL, CFStringRef alertHeader, CFStringRef alertMessage, CFStringRef defaultButtonTitle, CFStringRef alternateButtonTitle, CFStringRef otherButtonTitle, CFOptionFlags *responseFlags) {
    CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorSystemDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (alertHeader) CFDictionarySetValue(dict, kCFUserNotificationAlertHeaderKey, alertHeader);
    if (alertMessage) CFDictionarySetValue(dict, kCFUserNotificationAlertMessageKey, alertMessage);
    if (defaultButtonTitle) CFDictionarySetValue(dict, kCFUserNotificationDefaultButtonTitleKey, defaultButtonTitle);
    if (alternateButtonTitle) CFDictionarySetValue(dict, kCFUserNotificationAlternateButtonTitleKey, alternateButtonTitle);
    if (otherButtonTitle) CFDictionarySetValue(dict, kCFUserNotificationOtherButtonTitleKey, otherButtonTitle);
    SInt32 error = 0;
    CFUserNotificationRef n = CFUserNotificationCreate(kCFAllocatorDefault, timeout, flags, &error, dict);
    CFRelease(dict);
    if (n) {
        error = CFUserNotificationReceiveResponse(n, 0, responseFlags);
        CFRelease(n);
    }
    return error;
}
