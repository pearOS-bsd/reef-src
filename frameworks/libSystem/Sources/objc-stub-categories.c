// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Categories whose class is a Swift class stub.
//
// Swift gives a class with resilient ancestry (its metadata is made at run time) a class stub for
// Objective-C (objc4's stub_class_t: an isa of 1 to 15, then the initializer), and puts the categories
// on such a class into __objc_catlist2, with the stub as their class. Apple's libobjc keeps these
// categories aside and attaches them when Swift realizes the class: _objc_realizeClassFromSwift(cls,
// stub) (objc4 load_categories_nolock, methodizeClass). Reef's libobjc attaches every entry of
// __objc_catlist2 at once and treats the stub as a class, which crashes. Before Reef's libobjc maps an
// image, the entries whose class is a stub are swapped for an empty category without a class (which it
// skips); the libobjc wrapper attaches the real ones after _objc_realizeClassFromSwift.

#include <dlfcn.h>
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef void (*MappedCallback)(unsigned count, const char *const paths[], const struct mach_header *const headers[]);
typedef void (*InitializedCallback)(const char *path, const struct mach_header *header);
typedef void (*UnmappedCallback)(const char *path, const struct mach_header *header);

// objc4 category_t
struct category {
    const char *name;
    void *cls;
    void *instanceMethods;
    void *classMethods;
    void *protocols;
    void *instanceProperties;
    void *classProperties;
};

struct stubCategory {
    const void *stub;
    struct category *category;
    const struct mach_header *header;
};

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static struct stubCategory *pending;
static size_t pendingCount;
static MappedCallback objcMapped;

static int isStub(const void *cls) {
    const uintptr_t isa = *(const uintptr_t *)cls;
    return isa >= 1 && isa < 16;
}

static void setAside(const struct mach_header *header, const char *segment, const char *section) {
    unsigned long size = 0;
    struct category **list = (struct category **)getsectiondata((const struct mach_header_64 *)header, segment, section, &size);
    if (!list)
        return;
    for (unsigned long i = 0; i < size / sizeof *list; i++) {
        struct category *category = list[i];
        if (!category || !category->cls || !isStub(category->cls))
            continue;
        struct category *empty = calloc(1, sizeof *empty);
        struct stubCategory *grown = realloc(pending, (pendingCount + 1) * sizeof *pending);
        if (!empty || !grown) {
            free(empty);
            continue;
        }
        pending = grown;
        pending[pendingCount++] = (struct stubCategory){category->cls, category, header};
        empty->name = category->name;
        list[i] = empty;
    }
}

static void mapped(unsigned count, const char *const paths[], const struct mach_header *const headers[]) {
    pthread_mutex_lock(&lock);
    for (unsigned i = 0; i < count; i++) {
        static const char *const segments[] = {"__DATA", "__DATA_CONST", "__DATA_DIRTY"};
        for (unsigned s = 0; s < 3; s++) {
            setAside(headers[i], segments[s], "__objc_catlist");
            setAside(headers[i], segments[s], "__objc_catlist2");
        }
    }
    pthread_mutex_unlock(&lock);
    objcMapped(count, paths, headers);
}

void _dyld_objc_notify_register(MappedCallback mappedCallback, InitializedCallback initialized, UnmappedCallback unmapped) {
    // The original is the one of Reef's libSystem, which this library re-exports.
    Dl_info info;
    if (!dladdr((const void *)malloc, &info))
        return;
    void (*original)(MappedCallback, InitializedCallback, UnmappedCallback) =
        (void (*)(MappedCallback, InitializedCallback, UnmappedCallback))dlsym((void *)info.dli_fbase, "_dyld_objc_notify_register");
    if (!original)
        return;
    objcMapped = mappedCallback;
    original(mapped, initialized, unmapped);
}

// The categories set aside for a stub, in load order; the caller frees the array. Each comes with the
// header of its image (for its image info).
struct category **reef_objc_take_stub_categories(const void *stub, const struct mach_header ***headers, size_t *count) {
    pthread_mutex_lock(&lock);
    size_t found = 0;
    for (size_t i = 0; i < pendingCount; i++)
        found += pending[i].stub == stub;
    struct category **list = found ? malloc(found * sizeof *list) : NULL;
    const struct mach_header **images = found ? malloc(found * sizeof *images) : NULL;
    size_t n = 0, kept = 0;
    for (size_t i = 0; i < pendingCount; i++) {
        if (pending[i].stub == stub && list && images) {
            list[n] = pending[i].category;
            images[n++] = pending[i].header;
        } else
            pending[kept++] = pending[i];
    }
    pendingCount = kept;
    pthread_mutex_unlock(&lock);
    if (!list || !images) {
        free(list);
        free(images);
        *count = 0;
        *headers = NULL;
        return NULL;
    }
    *count = n;
    *headers = images;
    return list;
}
