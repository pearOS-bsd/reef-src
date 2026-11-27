// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoreMediaIO: the system object (ID 1) answers a few properties, and there are no devices or streams.

#include <stddef.h>
#include <stdint.h>

typedef int32_t OSStatus;
typedef uint32_t CMIOObjectID;

typedef struct {
    uint32_t selector, scope, element;
} CMIOObjectPropertyAddress;

enum {
    kCMIOHardwareNoError = 0,
    kCMIOHardwareIllegalOperationError = 'nope',
    kCMIOHardwareBadObjectError = '!obj',
    kCMIOHardwareBadPropertySizeError = '!siz',
    kCMIOHardwareBadDeviceError = '!dev',
    kCMIOHardwareBadStreamError = '!str',
    kCMIOHardwareUnknownPropertyError = 'who?',
};

// The properties of the system object: 'dev#', 'ownd' are lists (empty); 'dIn ', 'dOut', 'yes ', 'wscd' are one 32-bit value (0).
// ('ownc' itself is not one of them.)
static int propertySize(uint32_t selector, uint32_t *size)
{
    switch (selector) {
    case 'dIn ':
    case 'dOut':
    case 'yes ':
    case 'wscd':
        *size = 4;
        return 1;
    case 'dev#':
    case 'ownd':
        *size = 0;
        return 1;
    }
    return 0;
}

OSStatus CMIOObjectGetPropertyDataSize(CMIOObjectID object, const CMIOObjectPropertyAddress *address, uint32_t qualifierSize,
                                       const void *qualifier, uint32_t *dataSize)
{
    if (!address || !dataSize)
        return kCMIOHardwareIllegalOperationError;
    if (object != 1)
        return kCMIOHardwareBadObjectError;
    uint32_t size = 0;
    if (!propertySize(address->selector, &size))
        return kCMIOHardwareUnknownPropertyError;
    *dataSize = size;
    return kCMIOHardwareNoError;
}

OSStatus CMIOObjectGetPropertyData(CMIOObjectID object, const CMIOObjectPropertyAddress *address, uint32_t qualifierSize,
                                   const void *qualifier, uint32_t dataSize, uint32_t *dataUsed, void *data)
{
    if (!address || !dataUsed)
        return kCMIOHardwareIllegalOperationError;
    if (object != 1)
        return kCMIOHardwareBadObjectError;
    uint32_t size = 0;
    if (!propertySize(address->selector, &size))
        return kCMIOHardwareUnknownPropertyError;
    if (size) {
        if (dataSize < size)
            return kCMIOHardwareBadPropertySizeError;
        *(uint32_t *)data = 0;
    }
    *dataUsed = size;
    return kCMIOHardwareNoError;
}

OSStatus CMIODeviceStartStream(CMIOObjectID device, CMIOObjectID stream)
{
    return kCMIOHardwareBadDeviceError;
}

OSStatus CMIODeviceStopStream(CMIOObjectID device, CMIOObjectID stream)
{
    return kCMIOHardwareBadDeviceError;
}

OSStatus CMIOStreamCopyBufferQueue(CMIOObjectID stream, void *queueAlteredProc, void *queueAlteredRefCon, void **queue)
{
    if (queue)
        *queue = NULL;
    return kCMIOHardwareBadStreamError;
}
