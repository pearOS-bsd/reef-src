// Loads the original or our library and gives the table of functions.
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>

static void* table[330];

static void load(const char* path)
{
    void* library = dlopen(path, RTLD_NOW);
    if (!library) {
        fprintf(stderr, "%s\n", dlerror());
        exit(2);
    }
    if (auto ours = reinterpret_cast<void* const* (*)()>(dlsym(library, "reef_test_table"))) {
        memcpy(table, ours(), sizeof table);
        return;
    }
    auto get = reinterpret_cast<const void* (*)(uint64_t)>(dlsym(library, "reef_appkit_host_interface"));
    const void* const* interface = static_cast<const void* const*>(get(0x18));
    if (!interface) {
        fprintf(stderr, "no interface\n");
        exit(2);
    }
    for (int i = 0; i < 317; i++)
        table[i] = const_cast<void*>(interface[1 + i]);
    // Function 154 is at 0xfe550.
    char* base = static_cast<char*>(table[154]) - 0xfe550;
    table[317] = base + 0xfc5d0;
    table[318] = base + 0x98430;
    table[319] = base + 0xa31f0;
    table[320] = base + 0xa3260;
}

template <typename F>
static F* fn(int index)
{
    return reinterpret_cast<F*>(table[index]);
}
