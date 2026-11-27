// Debug aid: REEF_TRACE="Class sel;+Class sel;Class *" prints each call of the named methods ("*": every method the
// class itself defines), at most REEF_TRACE_MAX (default 20) times per method, with the nesting depth and the receiver.
// Each traced method gets a small trampoline that loads its record and jumps to a common stub; the stub saves the
// argument registers, logs, restores them and jumps to the original implementation, so any signature works.
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

struct traced {
    IMP original;
    char name[160];
    int count;
};

static int traceMax = 20;
static __thread int traceDepth;

IMP reefTraceEnter(struct traced *t, void *self);
IMP reefTraceEnter(struct traced *t, void *self) {
    if (t->count < traceMax) {
        t->count++;
        fprintf(stderr, "trace: %*s%s %p\n", traceDepth > 20 ? 20 : traceDepth, "", t->name, self);
    }
    return t->original;
}

// Common stub: r10 = struct traced *. Keeps the stack aligned (entry rsp is 8 mod 16; push rbp makes it 0).
__asm__(
    ".text\n"
    ".p2align 4\n"
    "_reefTraceCommon:\n"
    "    pushq %rbp\n"
    "    movq %rsp, %rbp\n"
    "    subq $0xd0, %rsp\n"
    "    movq %rdi, 0x00(%rsp)\n"
    "    movq %rsi, 0x08(%rsp)\n"
    "    movq %rdx, 0x10(%rsp)\n"
    "    movq %rcx, 0x18(%rsp)\n"
    "    movq %r8, 0x20(%rsp)\n"
    "    movq %r9, 0x28(%rsp)\n"
    "    movq %rax, 0x30(%rsp)\n"
    "    movdqu %xmm0, 0x40(%rsp)\n"
    "    movdqu %xmm1, 0x50(%rsp)\n"
    "    movdqu %xmm2, 0x60(%rsp)\n"
    "    movdqu %xmm3, 0x70(%rsp)\n"
    "    movdqu %xmm4, 0x80(%rsp)\n"
    "    movdqu %xmm5, 0x90(%rsp)\n"
    "    movdqu %xmm6, 0xa0(%rsp)\n"
    "    movdqu %xmm7, 0xb0(%rsp)\n"
    "    movq %r10, %rdi\n"
    "    movq 0x00(%rsp), %rsi\n"
    "    call _reefTraceEnter\n"
    "    movq %rax, %r11\n"
    "    movq 0x00(%rsp), %rdi\n"
    "    movq 0x08(%rsp), %rsi\n"
    "    movq 0x10(%rsp), %rdx\n"
    "    movq 0x18(%rsp), %rcx\n"
    "    movq 0x20(%rsp), %r8\n"
    "    movq 0x28(%rsp), %r9\n"
    "    movq 0x30(%rsp), %rax\n"
    "    movdqu 0x40(%rsp), %xmm0\n"
    "    movdqu 0x50(%rsp), %xmm1\n"
    "    movdqu 0x60(%rsp), %xmm2\n"
    "    movdqu 0x70(%rsp), %xmm3\n"
    "    movdqu 0x80(%rsp), %xmm4\n"
    "    movdqu 0x90(%rsp), %xmm5\n"
    "    movdqu 0xa0(%rsp), %xmm6\n"
    "    movdqu 0xb0(%rsp), %xmm7\n"
    "    leave\n"
    "    jmpq *%r11\n");
extern char reefTraceCommon[] __asm__("_reefTraceCommon");

static unsigned char *trampolines;
static size_t trampolinesUsed, trampolinesSize;

static IMP makeTrampoline(struct traced *t) {
    if (!trampolines || trampolinesUsed + 32 > trampolinesSize) {
        trampolinesSize = 1 << 16;
        trampolines = mmap(NULL, trampolinesSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (trampolines == MAP_FAILED) { trampolines = NULL; return NULL; }
        trampolinesUsed = 0;
    }
    unsigned char *p = trampolines + trampolinesUsed;
    trampolinesUsed += 32;
    uint64_t record = (uint64_t)t, common = (uint64_t)reefTraceCommon;
    p[0] = 0x49; p[1] = 0xba; memcpy(p + 2, &record, 8);   // movabs $record, %r10
    p[10] = 0x49; p[11] = 0xbb; memcpy(p + 12, &common, 8); // movabs $common, %r11
    p[20] = 0x41; p[21] = 0xff; p[22] = 0xe3;               // jmp *%r11
    return (IMP)p;
}

static void traceMethod(Class cls, BOOL meta, SEL sel) {
    Class target = meta ? object_getClass(cls) : cls;
    Method m = meta ? class_getClassMethod(cls, sel) : class_getInstanceMethod(cls, sel);
    if (!m) { fprintf(stderr, "trace: no %c[%s %s]\n", meta ? '+' : '-', class_getName(cls), sel_getName(sel)); return; }
    const char *name = sel_getName(sel);
    if (!strcmp(name, "retain") || !strcmp(name, "release") || !strcmp(name, "autorelease") || !strcmp(name, "dealloc") ||
        !strcmp(name, ".cxx_destruct") || !strcmp(name, "retainCount") || !strcmp(name, "_tryRetain") || !strcmp(name, "_isDeallocating"))
        return;
    struct traced *t = calloc(1, sizeof *t);
    snprintf(t->name, sizeof t->name, "%c[%s %s]", meta ? '+' : '-', class_getName(cls), name);
    IMP trampoline = makeTrampoline(t);
    if (!trampoline) return;
    t->original = method_getImplementation(m);
    if (!class_addMethod(target, sel, trampoline, method_getTypeEncoding(m))) {
        unsigned n = 0;
        Method *own = class_copyMethodList(target, &n);
        for (unsigned i = 0; i < n; i++)
            if (method_getName(own[i]) == sel) { t->original = method_setImplementation(own[i], trampoline); break; }
        free(own);
    }
}

static void traceAll(Class cls, BOOL meta) {
    unsigned n = 0;
    Method *own = class_copyMethodList(meta ? object_getClass(cls) : cls, &n);
    SEL *sels = calloc(n ? n : 1, sizeof *sels);
    for (unsigned i = 0; i < n; i++) sels[i] = method_getName(own[i]);
    free(own);
    for (unsigned i = 0; i < n; i++) traceMethod(cls, meta, sels[i]);
    free(sels);
}

static void installTraces(const char *spec) {
    char *copy = strdup(spec), *save = NULL;
    for (char *entry = strtok_r(copy, ";", &save); entry; entry = strtok_r(NULL, ";", &save)) {
        while (*entry == ' ') entry++;
        BOOL meta = *entry == '+';
        if (meta || *entry == '-') entry++;
        char *space = strchr(entry, ' ');
        if (!space) continue;
        *space = 0;
        Class cls = objc_getClass(entry);
        if (!cls) { fprintf(stderr, "trace: no class %s\n", entry); continue; }
        const char *selName = space + 1;
        if (!strcmp(selName, "*")) { traceAll(cls, NO); traceAll(cls, YES); }
        else traceMethod(cls, meta, sel_registerName(selName));
    }
    free(copy);
}

__attribute__((constructor)) static void ReefTrace(void) {
    const char *spec = getenv("REEF_TRACE");
    if (!spec) return;
    if (getenv("REEF_TRACE_MAX")) traceMax = atoi(getenv("REEF_TRACE_MAX"));
    char *copy = strdup(spec);
    // After every image is loaded: the first turn of the main run loop.
    dispatch_async(dispatch_get_main_queue(), ^{ installTraces(copy); });
}
