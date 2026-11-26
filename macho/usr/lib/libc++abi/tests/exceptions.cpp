// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libc++abi: exceptions (throw, catch by type, rethrow, unwinding with destructors), dynamic_cast, function-local statics, the demangler.
#include <stdio.h>
#include <stdlib.h>
extern "C" char *__cxa_demangle(const char *, char *, unsigned long *, int *);

struct Base { virtual ~Base() {} virtual int id() { return 1; } };
struct Derived : Base { int id() override { return 2; } };
struct Other { virtual ~Other() {} };
struct Guard { const char *name; ~Guard() { printf("destroy %s\n", name); } };

static int thrower(int kind)
{
    Guard g{"thrower"};
    if (kind == 0) throw 42;
    if (kind == 1) throw Derived();
    if (kind == 2) throw "text";
    return kind;
}

static int counter() { static int value = printf("init static\n"); return value + 1; }

int main()
{
    for (int k = 0; k < 4; k++) {
        try {
            printf("result %d\n", thrower(k));
        } catch (int v) {
            printf("int %d\n", v);
        } catch (Base &b) {
            printf("base %d\n", b.id());
        } catch (const char *s) {
            try { throw; } catch (const char *again) { printf("rethrown %s\n", again); }
        }
    }
    Base *b = new Derived;
    printf("cast %d %d\n", dynamic_cast<Derived *>(b) != 0, dynamic_cast<Other *>(b) != 0);
    delete b;
    printf("%d %d\n", counter(), counter());
    const char *names[] = {"_ZN4Base2idEv", "_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKc", "_Z3fooIiEvT_", "bad"};
    for (const char *n : names) {
        int status = 0; char *out = __cxa_demangle(n, 0, 0, &status);
        printf("%d %s\n", status, out ? out : "(null)"); free(out);
    }
    return 0;
}
