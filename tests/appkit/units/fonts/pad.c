// LD_PRELOAD: memory is zero, also past the end of what was asked for (FreeType reads there, see sizes of c-v0.ttf drawn aliased).
#include <stddef.h>
void* __libc_calloc(size_t, size_t);
void* __libc_realloc(void*, size_t);
void* malloc(size_t n) { return __libc_calloc(1, n + 4096); }
void* calloc(size_t a, size_t b) { return __libc_calloc(1, a * b + 4096); }
void* realloc(void* p, size_t n) { return p ? __libc_realloc(p, n + 4096) : __libc_calloc(1, n + 4096); }
