// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreMediaIO: the properties of the system object, the errors for anything else, and the missing streams.
#include <stdio.h>
#include <stdint.h>

typedef struct { uint32_t selector, scope, element; } Address;
int CMIOObjectGetPropertyDataSize(uint32_t, const Address *, uint32_t, const void *, uint32_t *);
int CMIOObjectGetPropertyData(uint32_t, const Address *, uint32_t, const void *, uint32_t, uint32_t *, void *);
int CMIODeviceStartStream(uint32_t, uint32_t);
int CMIODeviceStopStream(uint32_t, uint32_t);
int CMIOStreamCopyBufferQueue(uint32_t, void *, void *, void **);

int main(void)
{
    uint32_t selectors[] = { 'dIn ', 'dOut', 'yes ', 'wscd', 'dev#', 'ownd', 'ownc', 'abcd', 0 };
    for (int i = 0; i < 9; i++) {
        Address a = { selectors[i], 0, 0 };
        uint32_t size = 99, used = 77, value = 55;
        int r = CMIOObjectGetPropertyDataSize(1, &a, 0, 0, &size);
        int s = CMIOObjectGetPropertyData(1, &a, 0, 0, 4, &used, &value);
        int t = CMIOObjectGetPropertyData(1, &a, 0, 0, 2, &used, &value);
        printf("%08x: %08x size %u, %08x used %u value %u, small %08x\n", selectors[i], r, size, s, used, value, t);
    }
    Address a = { 'dev#', 0, 0 };
    uint32_t size = 0;
    printf("other object %08x\n", CMIOObjectGetPropertyDataSize(2, &a, 0, 0, &size));
    printf("no address %08x, no size %08x\n", CMIOObjectGetPropertyDataSize(1, 0, 0, 0, &size), CMIOObjectGetPropertyDataSize(1, &a, 0, 0, 0));
    void *queue = (void *)1;
    printf("start %08x stop %08x queue %08x %p\n", CMIODeviceStartStream(1, 2), CMIODeviceStopStream(1, 2), CMIOStreamCopyBufferQueue(1, 0, 0, &queue), queue);
    printf("%08x\n", CMIOStreamCopyBufferQueue(1, 0, 0, 0));
    return 0;
}
