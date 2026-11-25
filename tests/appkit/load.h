// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// What the test drivers of the AppKit host share: the interface of the library given, a log and random numbers that are the same
// on both sides.
#pragma once

#include "interface.h"

#include <dlfcn.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

using cutefish::reef::Interface;

inline const Interface* host;

inline void load(const char* path)
{
    void* library = dlopen(path, RTLD_NOW);
    if (!library) {
        printf("%s\n", dlerror());
        exit(1);
    }
    host = reinterpret_cast<const Interface* (*)(int)>(dlsym(library, "reef_appkit_host_interface"))(24);
}

inline uint64_t state = 1;

inline void seed(uint64_t value)
{
    state = value * 0x9e3779b97f4a7c15ULL + 1;
}

inline uint32_t random32()
{
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return uint32_t((state * 0x2545f4914f6cdd1dULL) >> 32);
}

// 0 to count - 1.
inline int pick(int count)
{
    return int(random32() % uint32_t(count));
}

inline bool chance(int percent)
{
    return pick(100) < percent;
}

inline double number(double low, double high)
{
    const double value = low + (high - low) * (random32() / 4294967295.0);
    // Often whole, sometimes halves.
    switch (pick(4)) {
    case 0:
        return double(long(value));
    case 1:
        return double(long(value * 2)) / 2;
    default:
        return value;
    }
}

inline uint64_t hashOf(const void* bytes, size_t size)
{
    uint64_t hash = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < size; i++)
        hash = (hash ^ static_cast<const unsigned char*>(bytes)[i]) * 0x100000001b3ULL;
    return hash;
}

__attribute__((format(printf, 1, 2))) inline void say(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
    putchar('\n');
}
