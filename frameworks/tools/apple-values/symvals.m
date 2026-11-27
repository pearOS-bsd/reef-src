// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Runs on the Mac: prints what an exported symbol of Apple's library is (function, Objective-C, an object constant
// with its class and value, or raw data). usage: symvals LIB SYMBOL
#import <Foundation/Foundation.h>
#import <dlfcn.h>
#import <mach-o/dyld.h>
#import <mach-o/loader.h>
#import <objc/runtime.h>
#import <malloc/malloc.h>
static const char *segmentOf(const void *p) {
    Dl_info info; if (!dladdr(p, &info) || !info.dli_fbase) return "?";
    const struct mach_header_64 *mh = info.dli_fbase;
    const struct load_command *lc = (const void *)(mh + 1);
    intptr_t slide = 0;
    for (uint32_t i = 0; i < _dyld_image_count(); i++) if (_dyld_get_image_header(i) == (const void *)mh) slide = _dyld_get_image_vmaddr_slide(i);
    for (uint32_t i = 0; i < mh->ncmds; i++, lc = (const void *)((const char *)lc + lc->cmdsize)) {
        if (lc->cmd != LC_SEGMENT_64) continue;
        const struct segment_command_64 *sc = (const void *)lc;
        uintptr_t a = (uintptr_t)p - slide;
        if (a >= sc->vmaddr && a < sc->vmaddr + sc->vmsize) { static char buf[32]; strlcpy(buf, sc->segname, 32); return buf; }
    }
    return "?";
}
static BOOL isObject(const void *p) {
    if (!p || ((uintptr_t)p & 7)) return NO;
    if ((uintptr_t)p & (1ULL << 63)) return YES;
    Dl_info info;
    if (malloc_zone_from_ptr(p)) return YES;
    if (dladdr(p, &info) && info.dli_fbase) { Class c = object_getClass((__bridge id)p); return c && class_isMetaClass(object_getClass((id)c)); }
    return NO;
}
int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    const char *lib = argv[1], *sym = argv[2];
    void *h = dlopen(lib, RTLD_LAZY);
    const char *name = sym[0] == '_' ? sym + 1 : sym;
    void *p = h ? dlsym(h, name) : NULL;
    if (!p) { printf("%s\t%s\tMISSING\n", lib, sym); return 0; }
    const char *seg = segmentOf(p);
    if (!strcmp(seg, "__TEXT")) { printf("%s\t%s\tFUNC\n", lib, sym); return 0; }
    if (!strncmp(sym, "_OBJC_", 6)) { printf("%s\t%s\tOBJC\n", lib, sym); return 0; }
    const void *v = *(const void **)p;
    if (isObject(v)) {
        id o = (__bridge id)v;
        NSString *d = [o isKindOfClass:[NSString class]] ? o : [o description];
        d = [[d stringByReplacingOccurrencesOfString:@"\\" withString:@"\\\\"] stringByReplacingOccurrencesOfString:@"\n" withString:@"\\n"];
        printf("%s\t%s\tOBJ\t%s\t%s\n", lib, sym, class_getName(object_getClass(o)), d.UTF8String);
    } else {
        const unsigned char *b = p; printf("%s\t%s\tDATA\t%s\t", lib, sym, seg);
        for (int i = 0; i < 16; i++) printf("%02x", b[i]);
        printf("\n");
    }
    return 0;
}
