// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// NSMethodSignature, NSInvocation and the forwarding handler of Reef's CoreFoundation: signatures of many shapes (registers, memory, x87, stack
// arguments), invocations of real methods, and messages to a class that forwards unknown selectors to forwardInvocation:.
// The same output with the original and with the rebuilt file.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef const void *CFStringRef, *CFAllocatorRef, *CFNumberRef;
typedef long CFIndex;
extern CFAllocatorRef kCFAllocatorDefault;
extern CFStringRef CFStringCreateWithCString(CFAllocatorRef, const char *, unsigned);
extern CFNumberRef CFNumberCreate(CFAllocatorRef, long, const void *);
extern void *objc_msgSend(void *, void *, ...);
extern void objc_msgSend_stret(void);
extern void *sel_registerName(const char *);
extern void *objc_getClass(const char *);
extern void *objc_allocateClassPair(void *, const char *, unsigned long);
extern void objc_registerClassPair(void *);
extern unsigned char class_addMethod(void *, void *, void *, const char *);
#define MSG(T, o, n, ...) ((T (*)(void *, void *, ...))objc_msgSend)((void *)(o), sel_registerName(n), ##__VA_ARGS__)
#define SEL(n) sel_registerName(n)

typedef struct { unsigned long location, length; } Range;
typedef struct { long a, b, c; } Big;
typedef struct { double x, y; } Pair;
typedef struct { float x; int i; } Mixed;
typedef struct { char c[3]; } Tiny;

static void signature(const char *types) {
    void *s = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", types);
    printf("%s: args %lu frame %lu oneway %d return '%s' length %lu\n", types, MSG(unsigned long, s, "numberOfArguments"), MSG(unsigned long, s, "frameLength"),
           MSG(int, s, "isOneway"), MSG(char *, s, "methodReturnType"), MSG(unsigned long, s, "methodReturnLength"));
    for (unsigned long i = 0; i < MSG(unsigned long, s, "numberOfArguments"); i++) printf("  arg %lu '%s'\n", i, MSG(char *, s, "getArgumentTypeAtIndex:", i));
    void *t = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", types);
    printf("  equal %d hash %d\n", MSG(int, s, "isEqual:", t), MSG(unsigned long, s, "hash") == MSG(unsigned long, t, "hash"));
}

// Methods of the class "Calc".
static double calcScale(void *self, void *cmd, double value, long times) { return value * times + 0.5; }
static Big calcBig(void *self, void *cmd, long a) { Big b = {a, a * 2, a * 3}; return b; }
static Pair calcPair(void *self, void *cmd, double a, double b) { Pair p = {a + b, a - b}; return p; }
static Mixed calcMixed(void *self, void *cmd, int i) { Mixed m = {1.5f, i}; return m; }
static Tiny calcTiny(void *self, void *cmd, char c) { Tiny t = {{c, c + 1, c + 2}}; return t; }
static long double calcLong(void *self, void *cmd, long double v) { return v * 2; }
static long calcMany(void *self, void *cmd, long a, long b, long c, long d, long e, long f, long g, long h, double x0, double x1, double x2, double x3, double x4, double x5, double x6, double x7, double x8, Big big) {
    return a + 2 * b + 3 * c + 4 * d + 5 * e + 6 * f + 7 * g + 8 * h + (long)(x0 + x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8) + big.a + big.b + big.c;
}
static Range calcRange(void *self, void *cmd, unsigned long a) { Range r = {a, a * 2}; return r; }
static short calcShort(void *self, void *cmd, short a, unsigned char b, int c) { return a + b + c; }

// Forwarding: the class "Ghost" has no methods; it answers methodSignatureForSelector: and forwardInvocation:.
static void *ghostSignature(void *self, void *cmd, void *selector) {
    extern const char *sel_getName(void *);
    const char *name = sel_getName(selector);
    const char *types = 0;
    if (!strcmp(name, "ghostSum:with:")) types = "q32@0:8q16q24";
    else if (!strcmp(name, "ghostHalf:")) types = "d24@0:8d16";
    else if (!strcmp(name, "ghostBig:")) types = "{Big=qqq}24@0:8q16";
    else if (!strcmp(name, "ghostPair:")) types = "{Pair=dd}32@0:8d16d24";
    else if (!strcmp(name, "ghostNothing")) types = "v16@0:8";
    else if (!strcmp(name, "ghostString:")) types = "*24@0:8*16";
    if (!types) return 0;
    return MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", types);
}
static void ghostForward(void *self, void *cmd, void *invocation) {
    extern const char *sel_getName(void *);
    const char *name = sel_getName(MSG(void *, invocation, "selector"));
    printf("forwarded %s to %d\n", name, MSG(void *, invocation, "target") == self);
    if (!strcmp(name, "ghostSum:with:")) {
        long a, b; MSG(void, invocation, "getArgument:atIndex:", &a, 2); MSG(void, invocation, "getArgument:atIndex:", &b, 3);
        long r = a * 100 + b; MSG(void, invocation, "setReturnValue:", &r);
    } else if (!strcmp(name, "ghostHalf:")) {
        double a; MSG(void, invocation, "getArgument:atIndex:", &a, 2);
        double r = a / 2; MSG(void, invocation, "setReturnValue:", &r);
    } else if (!strcmp(name, "ghostBig:")) {
        long a; MSG(void, invocation, "getArgument:atIndex:", &a, 2);
        Big r = {a, a + 1, a + 2}; MSG(void, invocation, "setReturnValue:", &r);
    } else if (!strcmp(name, "ghostPair:")) {
        double a, b; MSG(void, invocation, "getArgument:atIndex:", &a, 2); MSG(void, invocation, "getArgument:atIndex:", &b, 3);
        Pair r = {b, a}; MSG(void, invocation, "setReturnValue:", &r);
    } else if (!strcmp(name, "ghostString:")) {
        char *a; MSG(void, invocation, "getArgument:atIndex:", &a, 2);
        printf("  string '%s'\n", a);
        char *r = "answer"; MSG(void, invocation, "setReturnValue:", &r);
    }
}

int main(void) {
    signature("v16@0:8");
    signature("q24@0:8d16");
    signature("@32@0:8@16@24");
    signature("{_NSRange=QQ}24@0:8@16");
    signature("{Big=qqq}24@0:8q16");
    signature("{Pair=dd}32@0:8d16d24");
    signature("D32@0:8D16");
    signature("Vv16@0:8");
    signature("c24@0:8{Big=qqq}16");
    signature("i16@0:8^{foo=i}16[4i]24");
    signature("v@?:@\"NSString\"");
    signature("{Mixed=fi}20@0:8i16");
    signature("v40@0:8b3i16r*24j:32");
    // invocation of real methods
    void *string = (void *)CFStringCreateWithCString(kCFAllocatorDefault, "hello world", 0x08000100);
    void *needle = (void *)CFStringCreateWithCString(kCFAllocatorDefault, "wor", 0x08000100);
    void *sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "Q16@0:8");
    void *inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setTarget:", string);
    MSG(void, inv, "setSelector:", SEL("length"));
    printf("invocation target ok %d selector ok %d retained %d\n", MSG(void *, inv, "target") == string, MSG(void *, inv, "selector") == SEL("length"), MSG(int, inv, "argumentsRetained"));
    MSG(void, inv, "invoke");
    unsigned long length = 0; MSG(void, inv, "getReturnValue:", &length); printf("length %lu\n", length);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "c24@0:8@16");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("isEqual:"));
    MSG(void, inv, "setArgument:atIndex:", &needle, 2);
    MSG(void, inv, "invokeWithTarget:", string);
    char equal = 5; MSG(void, inv, "getReturnValue:", &equal); printf("equal %d\n", equal);
    MSG(void, inv, "retainArguments");
    printf("retained %d\n", MSG(int, inv, "argumentsRetained"));
    MSG(void, inv, "invokeWithTarget:", needle);
    MSG(void, inv, "getReturnValue:", &equal); printf("needle equal needle %d\n", equal);
    void *got = 0; MSG(void, inv, "getArgument:atIndex:", &got, 2); printf("argument back %d\n", got == needle);
    MSG(void, inv, "setArgument:atIndex:", &string, 2);
    MSG(void, inv, "invokeWithTarget:", string);
    MSG(void, inv, "getReturnValue:", &equal); printf("string equal string %d\n", equal);
    printf("description %d\n", MSG(void *, inv, "description") != 0);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "S24@0:8Q16");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("characterAtIndex:"));
    unsigned long index = 4; MSG(void, inv, "setArgument:atIndex:", &index, 2);
    MSG(void, inv, "invokeWithTarget:", string);
    unsigned short ch = 0; MSG(void, inv, "getReturnValue:", &ch); printf("character %c\n", ch);
    // a nil target gives a zero result
    MSG(void, inv, "invokeWithTarget:", 0);
    MSG(void, inv, "getReturnValue:", &ch); printf("nil target result %d\n", ch);
    // the Calc class: arguments of every shape
    void *calc = objc_allocateClassPair(objc_getClass("NSObject"), "Calc", 0);
    class_addMethod(calc, SEL("scale:times:"), calcScale, "d32@0:8d16q24");
    class_addMethod(calc, SEL("big:"), calcBig, "{Big=qqq}24@0:8q16");
    class_addMethod(calc, SEL("pair:with:"), calcPair, "{Pair=dd}32@0:8d16d24");
    class_addMethod(calc, SEL("mixed:"), calcMixed, "{Mixed=fi}20@0:8i16");
    class_addMethod(calc, SEL("tiny:"), calcTiny, "{Tiny=[3c]}20@0:8c16");
    class_addMethod(calc, SEL("range:"), calcRange, "{_NSRange=QQ}24@0:8Q16");
    class_addMethod(calc, SEL("long:"), calcLong, "D32@0:8D16");
    class_addMethod(calc, SEL("short:b:c:"), calcShort, "s32@0:8s16C20i24");
    objc_registerClassPair(calc);
    void *object = MSG(void *, MSG(void *, calc, "alloc"), "init");
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "d32@0:8d16q24");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    double dv = 2.25; long lv = 4;
    MSG(void, inv, "setSelector:", SEL("scale:times:")); MSG(void, inv, "setArgument:atIndex:", &dv, 2); MSG(void, inv, "setArgument:atIndex:", &lv, 3);
    MSG(void, inv, "invokeWithTarget:", object);
    double dr; MSG(void, inv, "getReturnValue:", &dr); printf("scale %g\n", dr);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "{Big=qqq}24@0:8q16");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("big:")); lv = 10; MSG(void, inv, "setArgument:atIndex:", &lv, 2);
    MSG(void, inv, "invokeWithTarget:", object);
    Big big; MSG(void, inv, "getReturnValue:", &big); printf("big %ld %ld %ld\n", big.a, big.b, big.c);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "{Pair=dd}32@0:8d16d24");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("pair:with:")); double d1 = 5, d2 = 3; MSG(void, inv, "setArgument:atIndex:", &d1, 2); MSG(void, inv, "setArgument:atIndex:", &d2, 3);
    MSG(void, inv, "invokeWithTarget:", object);
    Pair pair; MSG(void, inv, "getReturnValue:", &pair); printf("pair %g %g\n", pair.x, pair.y);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "{Mixed=fi}20@0:8i16");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("mixed:")); int iv = -7; MSG(void, inv, "setArgument:atIndex:", &iv, 2);
    MSG(void, inv, "invokeWithTarget:", object);
    Mixed mixed; MSG(void, inv, "getReturnValue:", &mixed); printf("mixed %g %d\n", mixed.x, mixed.i);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "{Tiny=[3c]}20@0:8c16");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("tiny:")); char cv = 'a'; MSG(void, inv, "setArgument:atIndex:", &cv, 2);
    MSG(void, inv, "invokeWithTarget:", object);
    Tiny tiny; MSG(void, inv, "getReturnValue:", &tiny); printf("tiny %c%c%c\n", tiny.c[0], tiny.c[1], tiny.c[2]);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "{_NSRange=QQ}24@0:8Q16");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("range:")); unsigned long rv = 21; MSG(void, inv, "setArgument:atIndex:", &rv, 2);
    MSG(void, inv, "invokeWithTarget:", object);
    Range range; MSG(void, inv, "getReturnValue:", &range); printf("range %lu %lu\n", range.location, range.length);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "D32@0:8D16");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("long:")); long double ld = 21.25L; MSG(void, inv, "setArgument:atIndex:", &ld, 2);
    MSG(void, inv, "invokeWithTarget:", object);
    long double ldr = 0; MSG(void, inv, "getReturnValue:", &ldr); printf("long double %Lg\n", ldr);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "s32@0:8s16C20i24");
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("short:b:c:")); short sv = -3; unsigned char bv = 200; iv = 1000;
    MSG(void, inv, "setArgument:atIndex:", &sv, 2); MSG(void, inv, "setArgument:atIndex:", &bv, 3); MSG(void, inv, "setArgument:atIndex:", &iv, 4);
    MSG(void, inv, "invokeWithTarget:", object);
    short shr = 0; MSG(void, inv, "getReturnValue:", &shr); printf("short %d\n", shr);
    // many arguments, some on the stack
    calc = objc_allocateClassPair(objc_getClass("NSObject"), "Many", 0);
    class_addMethod(calc, SEL("many"), calcMany, "q@:qqqqqqqqdddddddddq");
    objc_registerClassPair(calc);
    sig = MSG(void *, objc_getClass("NSMethodSignature"), "signatureWithObjCTypes:", "q@:qqqqqqqqddddddddd{Big=qqq}");
    printf("many: args %lu frame %lu\n", MSG(unsigned long, sig, "numberOfArguments"), MSG(unsigned long, sig, "frameLength"));
    inv = MSG(void *, objc_getClass("NSInvocation"), "invocationWithMethodSignature:", sig);
    MSG(void, inv, "setSelector:", SEL("many"));
    long args[8] = {1, 2, 3, 4, 5, 6, 7, 8}; double ds[9] = {0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5}; Big bigarg = {100, 200, 300};
    for (int i = 0; i < 8; i++) MSG(void, inv, "setArgument:atIndex:", &args[i], 2 + i);
    for (int i = 0; i < 9; i++) MSG(void, inv, "setArgument:atIndex:", &ds[i], 10 + i);
    MSG(void, inv, "setArgument:atIndex:", &bigarg, 19);
    void *many_object = MSG(void *, MSG(void *, calc, "alloc"), "init");
    MSG(void, inv, "invokeWithTarget:", many_object);
    long many; MSG(void, inv, "getReturnValue:", &many); printf("many %ld\n", many);
    // forwarding
    void *ghost = objc_allocateClassPair(objc_getClass("NSObject"), "Ghost", 0);
    class_addMethod(ghost, SEL("methodSignatureForSelector:"), ghostSignature, "@24@0:8:16");
    class_addMethod(ghost, SEL("forwardInvocation:"), ghostForward, "v24@0:8@16");
    objc_registerClassPair(ghost);
    void *g = MSG(void *, MSG(void *, ghost, "alloc"), "init");
    printf("sum %ld\n", ((long (*)(void *, void *, long, long))objc_msgSend)(g, SEL("ghostSum:with:"), 7, 9));
    printf("half %g\n", ((double (*)(void *, void *, double))objc_msgSend)(g, SEL("ghostHalf:"), 9.0));
    {
        Big r;
        ((void (*)(Big *, void *, void *, long))objc_msgSend_stret)(&r, g, SEL("ghostBig:"), 40);
        printf("ghost big %ld %ld %ld\n", r.a, r.b, r.c);
    }
    Pair gp = ((Pair (*)(void *, void *, double, double))objc_msgSend)(g, SEL("ghostPair:"), 1.5, 2.5);
    printf("ghost pair %g %g\n", gp.x, gp.y);
    ((void (*)(void *, void *))objc_msgSend)(g, SEL("ghostNothing"));
    printf("string %s\n", ((char *(*)(void *, void *, char *))objc_msgSend)(g, SEL("ghostString:"), "question"));
    return 0;
}
