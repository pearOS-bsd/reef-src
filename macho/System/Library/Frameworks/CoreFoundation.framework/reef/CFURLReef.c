// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CFURL resource properties, bookmark data and FSRef: the properties and bookmarks are messages to NSURL (Foundation does the work), an FSRef
// is a token that stands for a path remembered in a table.
#include "CFInternal.h"
#include <objc/objc.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

extern Class objc_getClass(const char *name);
extern Class object_getClass(id obj);
extern SEL sel_registerName(const char *name);
extern BOOL class_respondsToSelector(Class cls, SEL sel);
extern id objc_msgSend(id self, SEL op, ...);

#define OBJ(x) ((id)(uintptr_t)(x))
#define CFOBJ(x) ((CFTypeRef)(uintptr_t)(x))

static SEL getResourceValueSelector;
static SEL setResourceValueSelector;
static SEL reachableSelector;

static CFErrorRef retainedError(id error) {
    return error ? (CFErrorRef)CFRetain(CFOBJ(error)) : NULL;
}

Boolean CFURLCopyResourcePropertyForKey(CFURLRef url, CFStringRef key, void *propertyValueTypeRefPtr, CFErrorRef *error) {
    id value = nil, err = nil;
    if (!getResourceValueSelector) getResourceValueSelector = sel_registerName("getResourceValue:forKey:error:");
    BOOL ok = ((BOOL (*)(id, SEL, id *, id, id *))objc_msgSend)(OBJ(url), getResourceValueSelector, &value, OBJ(key), &err);
    if (!ok) {
        if (error) *error = retainedError(err);
        return false;
    }
    if (propertyValueTypeRefPtr) *(CFTypeRef *)propertyValueTypeRefPtr = value ? CFRetain(CFOBJ(value)) : NULL;
    return true;
}

CFDictionaryRef CFURLCopyResourcePropertiesForKeys(CFURLRef url, CFArrayRef keys, CFErrorRef *error) {
    CFMutableDictionaryRef result = CFDictionaryCreateMutable(kCFAllocatorSystemDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFIndex count = CFArrayGetCount(keys);
    for (CFIndex i = 0; i < count; i++) {
        CFStringRef key = (CFStringRef)CFArrayGetValueAtIndex(keys, i);
        id value = nil, err = nil;
        if (!getResourceValueSelector) getResourceValueSelector = sel_registerName("getResourceValue:forKey:error:");
        BOOL ok = ((BOOL (*)(id, SEL, id *, id, id *))objc_msgSend)(OBJ(url), getResourceValueSelector, &value, OBJ(key), &err);
        if (!ok) {
            if (error) *error = retainedError(err);
            CFRelease(result);
            return NULL;
        }
        if (value) CFDictionarySetValue(result, key, CFOBJ(value));
    }
    return result;
}

Boolean CFURLSetResourcePropertyForKey(CFURLRef url, CFStringRef key, CFTypeRef propertyValue, CFErrorRef *error) {
    if (!setResourceValueSelector) setResourceValueSelector = sel_registerName("setResourceValue:forKey:error:");
    id err = nil;
    BOOL ok = ((BOOL (*)(id, SEL, id, id, id *))objc_msgSend)(OBJ(url), setResourceValueSelector, OBJ(propertyValue), OBJ(key), &err);
    if (error && !ok) *error = retainedError(err);
    return ok;
}

Boolean CFURLSetResourcePropertiesForKeys(CFURLRef url, CFDictionaryRef keyedPropertyValues, CFErrorRef *error) {
    CFIndex count = CFDictionaryGetCount(keyedPropertyValues);
    size_t size = (count + (count == 0)) * sizeof(void *);
    const void **keys = (const void **)malloc(size), **values = (const void **)malloc(size);
    CFDictionaryGetKeysAndValues(keyedPropertyValues, keys, values);
    Boolean ok = true;
    for (CFIndex i = 0; i < count; i++) {
        if (!setResourceValueSelector) setResourceValueSelector = sel_registerName("setResourceValue:forKey:error:");
        id err = nil;
        ok = ((BOOL (*)(id, SEL, id, id, id *))objc_msgSend)(OBJ(url), setResourceValueSelector, OBJ(values[i]), OBJ(keys[i]), &err);
        if (!ok) {
            if (error) *error = retainedError(err);
            break;
        }
    }
    free(keys);
    free(values);
    return ok;
}

void CFURLClearResourcePropertyCacheForKey(CFURLRef url, CFStringRef key) {
}

void CFURLClearResourcePropertyCache(CFURLRef url) {
}

void CFURLSetTemporaryResourcePropertyForKey(CFURLRef url, CFStringRef key, CFTypeRef propertyValue) {
}

Boolean CFURLResourceIsReachable(CFURLRef url, CFErrorRef *error) {
    if (!reachableSelector) reachableSelector = sel_registerName("checkResourceIsReachableAndReturnError:");
    id err = nil;
    BOOL ok = ((BOOL (*)(id, SEL, id *))objc_msgSend)(OBJ(url), reachableSelector, &err);
    if (error && !ok) *error = retainedError(err);
    return ok;
}

CFDataRef CFURLCreateBookmarkData(CFAllocatorRef allocator, CFURLRef url, CFOptionFlags options, CFArrayRef resourcePropertiesToInclude, CFURLRef relativeToURL, CFErrorRef *error) {
    SEL op = sel_registerName("bookmarkDataWithOptions:includingResourceValuesForKeys:relativeToURL:error:");
    if (url && class_respondsToSelector(object_getClass(OBJ(url)), op)) {
        id err = nil;
        id data = ((id (*)(id, SEL, unsigned long, id, id, id *))objc_msgSend)(OBJ(url), op, options, OBJ(resourcePropertiesToInclude), OBJ(relativeToURL), &err);
        if (data) return (CFDataRef)CFRetain(CFOBJ(data));
        if (error) *error = retainedError(err);
        return NULL;
    }
    if (error) *error = NULL;
    return NULL;
}

CFURLRef CFURLCreateByResolvingBookmarkData(CFAllocatorRef allocator, CFDataRef bookmark, CFOptionFlags options, CFURLRef relativeToURL, CFArrayRef resourcePropertiesToInclude, Boolean *isStale, CFErrorRef *error) {
    SEL op = sel_registerName("URLByResolvingBookmarkData:options:relativeToURL:bookmarkDataIsStale:error:");
    Class cls = objc_getClass("NSURL");
    if (cls && bookmark && class_respondsToSelector(object_getClass((id)cls), op)) {
        id err = nil;
        BOOL stale = NO;
        id url = ((id (*)(id, SEL, id, unsigned long, id, BOOL *, id *))objc_msgSend)((id)cls, op, OBJ(bookmark), options, OBJ(relativeToURL), &stale, &err);
        if (isStale) *isStale = stale;
        if (url) return (CFURLRef)CFRetain(CFOBJ(url));
        if (error) *error = retainedError(err);
        return NULL;
    }
    if (error) *error = NULL;
    return NULL;
}

CFDictionaryRef CFURLCreateResourcePropertiesForKeysFromBookmarkData(CFAllocatorRef allocator, CFArrayRef resourcePropertiesToReturn, CFDataRef bookmark) {
    SEL op = sel_registerName("resourceValuesForKeys:fromBookmarkData:");
    Class cls = objc_getClass("NSURL");
    if (cls && bookmark && class_respondsToSelector(object_getClass((id)cls), op)) {
        id values = ((id (*)(id, SEL, id, id))objc_msgSend)((id)cls, op, OBJ(resourcePropertiesToReturn), OBJ(bookmark));
        if (values) return (CFDictionaryRef)CFRetain(CFOBJ(values));
    }
    return NULL;
}

CFTypeRef CFURLCreateResourcePropertyForKeyFromBookmarkData(CFAllocatorRef allocator, CFStringRef resourcePropertyKey, CFDataRef bookmark) {
    if (!resourcePropertyKey) return NULL;
    CFArrayRef keys = CFArrayCreate(kCFAllocatorSystemDefault, (const void **)&resourcePropertyKey, 1, &kCFTypeArrayCallBacks);
    SEL op = sel_registerName("resourceValuesForKeys:fromBookmarkData:");
    Class cls = objc_getClass("NSURL");
    if (cls && bookmark && class_respondsToSelector(object_getClass((id)cls), op)) {
        id values = ((id (*)(id, SEL, id, id))objc_msgSend)((id)cls, op, OBJ(keys), OBJ(bookmark));
        if (values) {
            CFDictionaryRef dict = (CFDictionaryRef)CFRetain(CFOBJ(values));
            CFRelease(keys);
            if (!dict) return NULL;
            CFTypeRef value = CFDictionaryGetValue(dict, resourcePropertyKey);
            if (value) CFRetain(value);
            CFRelease(dict);
            return value;
        }
    }
    CFRelease(keys);
    return NULL;
}

CFDataRef CFURLCreateBookmarkDataFromFile(CFAllocatorRef allocator, CFURLRef fileURL, CFErrorRef *errorRef) {
    SEL op = sel_registerName("bookmarkDataWithContentsOfURL:error:");
    Class cls = objc_getClass("NSURL");
    if (cls && fileURL && class_respondsToSelector(object_getClass((id)cls), op)) {
        id err = nil;
        id data = ((id (*)(id, SEL, id, id *))objc_msgSend)((id)cls, op, OBJ(fileURL), &err);
        if (data) return (CFDataRef)CFRetain(CFOBJ(data));
        if (errorRef) *errorRef = retainedError(err);
        return NULL;
    }
    if (errorRef) *errorRef = NULL;
    return NULL;
}

Boolean CFURLWriteBookmarkDataToFile(CFDataRef bookmarkRef, CFURLRef fileURL, CFOptionFlags options, CFErrorRef *errorRef) {
    SEL op = sel_registerName("writeBookmarkData:toURL:options:error:");
    Class cls = objc_getClass("NSURL");
    if (cls && fileURL && bookmarkRef && class_respondsToSelector(object_getClass((id)cls), op)) {
        id err = nil;
        BOOL ok = ((BOOL (*)(id, SEL, id, id, unsigned long, id *))objc_msgSend)((id)cls, op, OBJ(bookmarkRef), OBJ(fileURL), options, &err);
        if (!ok && errorRef) *errorRef = retainedError(err);
        return ok;
    }
    if (errorRef) *errorRef = NULL;
    return false;
}

CFDataRef CFURLCreateBookmarkDataFromAliasRecord(CFAllocatorRef allocatorRef, CFDataRef aliasRecordDataRef) {
    return NULL;
}

Boolean CFURLStartAccessingSecurityScopedResource(CFURLRef url) {
    return true;
}

void CFURLStopAccessingSecurityScopedResource(CFURLRef url) {
}

// An FSRef is 80 bytes: the magic, the index into the table of paths, the device and the inode of the file when the token was made.
typedef struct {
    uint32_t magic;
    uint32_t index;
    int64_t device;
    uint64_t inode;
    uint8_t reserved[56];
} FSRef;

enum { kFSRefMagic = 0x72656566 };

static pthread_mutex_t fsRefLock = PTHREAD_MUTEX_INITIALIZER;
static char **fsRefPaths;
static unsigned fsRefCount;

Boolean _CFFSRefCreateForPath(const char *path, FSRef *ref, Boolean *isDirectory) {
    struct stat st;
    char resolved[1024];
    if (!path || !ref || stat(path, &st) != 0) return false;
    if (!realpath(path, resolved)) return false;
    pthread_mutex_lock(&fsRefLock);
    unsigned index = 0;
    while (index < fsRefCount && strcmp(fsRefPaths[index], resolved) != 0) index++;
    if (index == fsRefCount) {
        fsRefPaths = (char **)realloc(fsRefPaths, (fsRefCount + 1) * sizeof(char *));
        fsRefPaths[fsRefCount++] = strdup(resolved);
    }
    pthread_mutex_unlock(&fsRefLock);
    memset(ref->reserved, 0, sizeof(ref->reserved));
    ref->magic = kFSRefMagic;
    ref->index = index;
    ref->device = st.st_dev;
    ref->inode = st.st_ino;
    if (isDirectory) *isDirectory = (st.st_mode & S_IFMT) == S_IFDIR;
    return true;
}

Boolean _CFFSRefGetPath(const FSRef *ref, char *path, size_t size) {
    if (!path || !ref || ref->magic != kFSRefMagic) return false;
    unsigned index = ref->index;
    int64_t device = ref->device;
    uint64_t inode = ref->inode;
    pthread_mutex_lock(&fsRefLock);
    if (index < fsRefCount && fsRefPaths[index] && strlen(fsRefPaths[index]) < size) {
        strcpy(path, fsRefPaths[index]);
        pthread_mutex_unlock(&fsRefLock);
        struct stat st;
        int result = stat(path, &st);
        return result == 0 && device == st.st_dev && inode == st.st_ino;
    }
    pthread_mutex_unlock(&fsRefLock);
    return false;
}

Boolean CFURLGetFSRef(CFURLRef url, FSRef *fsRef) {
    char path[1024];
    if (url && CFURLGetFileSystemRepresentation(url, true, (UInt8 *)path, sizeof(path))) return _CFFSRefCreateForPath(path, fsRef, NULL);
    return false;
}

CFURLRef CFURLCreateFromFSRef(CFAllocatorRef allocator, const FSRef *fsRef) {
    char path[1024];
    struct stat st;
    if (!_CFFSRefGetPath(fsRef, path, sizeof(path))) return NULL;
    int result = stat(path, &st);
    return CFURLCreateFromFileSystemRepresentation(allocator, (const UInt8 *)path, strlen(path), result == 0 && (st.st_mode & S_IFMT) == S_IFDIR);
}

CFURLRef CFURLCreateFilePathURL(CFAllocatorRef allocator, CFURLRef url, CFErrorRef *error) {
    if (error) *error = NULL;
    if (!url) return NULL;
    CFURLRef absolute = CFURLCopyAbsoluteURL(url);
    if (!absolute) return NULL;
    CFStringRef scheme = CFURLCopyScheme(absolute);
    CFURLRef result = NULL;
    if (scheme) {
        if (CFStringCompare(scheme, CFSTR("file"), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
            CFStringRef path = CFURLCopyFileSystemPath(absolute, kCFURLPOSIXPathStyle);
            if (path) {
                result = CFURLCreateWithFileSystemPath(allocator, path, kCFURLPOSIXPathStyle, CFURLHasDirectoryPath(absolute));
                CFRelease(path);
            }
        }
        CFRelease(scheme);
    }
    CFRelease(absolute);
    if (error && !result) *error = CFErrorCreate(allocator, kCFErrorDomainCocoa, 0x106, NULL);
    return result;
}
