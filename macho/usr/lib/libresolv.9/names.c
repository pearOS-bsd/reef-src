// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libresolv: packing a name into labels and building a query (compiled with wrapping integer arithmetic).

#include <string.h>

// Packs a dotted name into labels (no compression).
int res_9_dn_comp(const char *name, unsigned char *buffer, int size, unsigned char **pointers, unsigned char **last)
{
    if (name[0] == '.' && name[1] == 0)
        name = "";
    int n = 0;
    while (*name) {
        const char *dot = strchr(name, '.');
        long length = dot ? dot - name : (long)strlen(name);
        if ((unsigned long)(length - 1) > 62)
            return -1;
        int next = (int)(n + length + 1);
        if (next >= size)
            return -1;
        buffer[n++] = (unsigned char)length;
        memcpy(buffer + n, name, length);
        n = next;
        name = dot + 1;
        if (!dot)
            break;
    }
    if (n >= size)
        return -1;
    buffer[n++] = 0;
    return n;
}

// Builds a standard query: a header with a fresh id and recursion desired, then the name, type and class.
int res_9_mkquery(int op, const char *name, int class, int type, const unsigned char *data, int dataLength, const void *newRecord,
                  unsigned char *buffer, int size)
{
    static unsigned short nextID = 1;
    if (op != 0 || size < 12)
        return -1;
    unsigned short id = nextID++;
    memset(buffer + 3, 0, 9);
    buffer[0] = id >> 8;
    buffer[1] = (unsigned char)id;
    buffer[2] = 1;
    buffer[5] = 1;
    if (name[0] == '.' && name[1] == 0)
        name = "";
    unsigned char *packed = buffer + 12;
    size -= 16;
    int n = 0;
    while (*name) {
        const char *dot = strchr(name, '.');
        long length = dot ? dot - name : (long)strlen(name);
        if ((unsigned long)(length - 1) > 62)
            return -1;
        int next = (int)(n + length + 1);
        if (next >= size)
            return -1;
        packed[n++] = (unsigned char)length;
        memcpy(packed + n, name, length);
        n = next;
        name = dot + 1;
        if (!dot)
            break;
    }
    if (n >= size)
        return -1;
    packed[n] = 0;
    if (n < -1)
        return -1;
    packed[n + 1] = type >> 8;
    packed[n + 2] = (unsigned char)type;
    packed[n + 3] = class >> 8;
    packed[n + 4] = (unsigned char)class;
    return n + 17;
}
