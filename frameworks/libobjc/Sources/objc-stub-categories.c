// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Categories on Swift class stubs are attached when Swift realizes the class, as in Apple's libobjc:
// _objc_realizeClassFromSwift(cls, previously) with previously the class stub (objc4 case 3, then
// methodizeClass(cls, previously) attaches the categories that waited for the stub). The libSystem
// wrapper set them aside when the image was mapped (Reef's libobjc does not know stubs in categories).
// Category methods win over the class's own methods, the last category loaded wins: class_replaceMethod.

#include <dlfcn.h>
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include <objc/runtime.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct category {
    const char *name;
    void *cls;
    const struct entryList *instanceMethods;
    const struct entryList *classMethods;
    const struct protocolList *protocols;
    const struct entryList *instanceProperties;
    const struct entryList *classProperties;
};

// objc4 entsize_list_tt: method_list_t (flags 0xffff0003, small methods 0x80000000) and property_list_t
struct entryList {
    uint32_t entsizeAndFlags;
    uint32_t count;
};

struct protocolList {
    uintptr_t count;
    const void *list[];
};

struct bigMethod {
    const char *name;
    const char *types;
    IMP imp;
};

struct smallMethod {
    int32_t name;   // to a selector reference
    int32_t types;
    int32_t imp;
};

struct property {
    const char *name;
    const char *attributes;
};

static const void *relative(const int32_t *field) { return (const char *)field + *field; }

static void addMethods(Class cls, const struct entryList *list) {
    if (!cls || !list)
        return;
    const int small = (list->entsizeAndFlags & 0x80000000) != 0;
    const uint32_t size = list->entsizeAndFlags & 0xfffc;
    const char *entries = (const char *)(list + 1);
    for (uint32_t i = 0; i < list->count; i++) {
        const char *entry = entries + (size_t)i * size;
        SEL name;
        const char *types;
        IMP imp;
        if (small) {
            const struct smallMethod *m = (const struct smallMethod *)entry;
            name = sel_registerName(*(const char *const *)relative(&m->name));
            types = relative(&m->types);
            imp = (IMP)relative(&m->imp);
        } else {
            const struct bigMethod *m = (const struct bigMethod *)entry;
            name = sel_registerName(m->name);
            types = m->types;
            imp = m->imp;
        }
        class_replaceMethod(cls, name, imp, types);
    }
}

static void addProperties(Class cls, const struct entryList *list) {
    if (!cls || !list)
        return;
    const uint32_t size = list->entsizeAndFlags & 0xfffc;
    const char *entries = (const char *)(list + 1);
    for (uint32_t i = 0; i < list->count; i++) {
        const struct property *p = (const struct property *)(entries + (size_t)i * size);
        // "T@\"NSString\",C,N,V_name": one attribute per comma, its name is the first letter.
        char *text = strdup(p->attributes ? p->attributes : "");
        if (!text)
            continue;
        size_t count = 1;
        for (const char *c = text; *c; c++)
            count += *c == ',';
        objc_property_attribute_t *attributes = calloc(count, sizeof *attributes);
        char *names = calloc(count, 2);
        size_t n = 0;
        if (attributes && names) {
            for (char *token = text, *next; token && *token; token = next) {
                next = strchr(token, ',');
                if (next)
                    *next++ = '\0';
                names[n * 2] = token[0];
                attributes[n].name = &names[n * 2];
                attributes[n].value = token + 1;
                n++;
            }
            class_replaceProperty(cls, p->name, attributes, (unsigned)n);
        }
        free(attributes);
        free(names);
        free(text);
    }
}

static int hasClassProperties(const struct mach_header *header) {
    static const char *const segments[] = {"__DATA", "__DATA_CONST", "__DATA_DIRTY", "__OBJC"};
    for (unsigned s = 0; s < 4; s++) {
        unsigned long size = 0;
        const uint32_t *info = (const uint32_t *)getsectiondata((const struct mach_header_64 *)header, segments[s], "__objc_imageinfo", &size);
        if (info && size >= 8)
            return (info[1] & (1u << 6)) != 0;   // HasCategoryClassProperties
    }
    return 0;
}

static void attachStubCategories(Class cls, const void *stub) {
    static struct category **(*take)(const void *, const struct mach_header ***, size_t *);
    if (!take)
        take = (struct category **(*)(const void *, const struct mach_header ***, size_t *))dlsym(RTLD_DEFAULT, "reef_objc_take_stub_categories");
    if (!take)
        return;
    const struct mach_header **headers = NULL;
    size_t count = 0;
    struct category **categories = take(stub, &headers, &count);
    Class meta = object_getClass((id)cls);
    for (size_t i = 0; i < count; i++) {
        const struct category *category = categories[i];
        addMethods(cls, category->instanceMethods);
        addMethods(meta, category->classMethods);
        if (category->protocols) {
            for (uintptr_t p = 0; p < category->protocols->count; p++) {
                const void *protocol = category->protocols->list[p];
                // protocol_t: isa, then the mangled name; the runtime's own copy if it has one
                Protocol *known = objc_getProtocol(((const char *const *)protocol)[1]);
                class_addProtocol(cls, known ? known : (Protocol *)protocol);
            }
        }
        addProperties(cls, category->instanceProperties);
        if (hasClassProperties(headers[i]))
            addProperties(meta, category->classProperties);
    }
    free(categories);
    free(headers);
}

Class _objc_realizeClassFromSwift(Class cls, void *previously) {
    static Class (*original)(Class, void *);
    if (!original) {
        // Reef's libobjc, which this library re-exports
        Dl_info info;
        if (dladdr((const void *)class_getName, &info))
            original = (Class (*)(Class, void *))dlsym((void *)info.dli_fbase, "_objc_realizeClassFromSwift");
        if (!original)
            abort();
    }
    Class realized = original(cls, previously);
    if (cls && previously && previously != (void *)cls)
        attachStubCategories(realized ? realized : cls, previously);
    return realized;
}
