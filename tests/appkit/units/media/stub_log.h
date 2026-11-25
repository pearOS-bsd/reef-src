// What the stubs log, in the order of the calls.
#pragma once
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <unistd.h>

static void note(const char* format, ...)
{
    char line[4096];
    va_list arguments;
    va_start(arguments, format);
    int n = vsnprintf(line, sizeof line - 1, format, arguments);
    va_end(arguments);
    if (n > int(sizeof line) - 2)
        n = sizeof line - 2;
    line[n++] = '\n';
    // What other threads log goes to another file: the order between threads is not the same twice.
    write(gettid() == getpid() || lseek(3, 0, SEEK_CUR) < 0 ? 1 : 3, line, n);
}

static uint64_t hashOf(const void* data, size_t size)
{
    uint64_t h = 1469598103934665603ull;
    for (size_t i = 0; i < size; i++)
        h = (h ^ static_cast<const unsigned char*>(data)[i]) * 1099511628211ull;
    return h;
}
