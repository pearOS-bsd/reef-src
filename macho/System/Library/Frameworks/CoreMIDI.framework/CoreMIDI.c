// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoreMIDI: there is no MIDI hardware. Clients and ports are only numbers (above 0x10000, counted up), there are no devices or endpoints,
// sending and properties fail; the packet list and event list helpers do their work as Apple's.

#include <stdint.h>
#include <string.h>

typedef int32_t OSStatus;
typedef uint32_t MIDIObjectRef;
typedef MIDIObjectRef MIDIClientRef, MIDIPortRef, MIDIEndpointRef, MIDIDeviceRef;
typedef uint64_t MIDITimeStamp;
typedef unsigned long ByteCount;
typedef unsigned long ItemCount;
typedef const struct __CFString *CFStringRef;

enum {
    kMIDIInvalidClient = -10830,
    kMIDIInvalidPort = -10831,
    kMIDINoConnection = -10833,
    kMIDIUnknownProperty = -10835,
    kMIDIObjectNotFound = -10842,
};

#define CFSTR(string) ((CFStringRef)__builtin___CFStringMakeConstantString(string))

#pragma pack(push, 4)
typedef struct MIDIPacket {
    MIDITimeStamp timeStamp;
    uint16_t length;
    uint8_t data[256];
} MIDIPacket;

typedef struct MIDIPacketList {
    uint32_t numPackets;
    MIDIPacket packet[1];
} MIDIPacketList;

typedef struct MIDIEventPacket {
    MIDITimeStamp timeStamp;
    uint32_t wordCount;
    uint32_t words[64];
} MIDIEventPacket;

typedef struct MIDIEventList {
    int32_t protocol;
    uint32_t numPackets;
    MIDIEventPacket packet[1];
} MIDIEventList;
#pragma pack(pop)

const CFStringRef kMIDIPropertyName = CFSTR("name");
const CFStringRef kMIDIPropertyManufacturer = CFSTR("manufacturer");
const CFStringRef kMIDIPropertyModel = CFSTR("model");
const CFStringRef kMIDIPropertyUniqueID = CFSTR("uniqueID");
const CFStringRef kMIDIPropertyDeviceID = CFSTR("deviceID");
const CFStringRef kMIDIPropertyDisplayName = CFSTR("displayName");
const CFStringRef kMIDIPropertyDriverVersion = CFSTR("driverVersion");
const CFStringRef kMIDIPropertyOffline = CFSTR("offline");
const CFStringRef kMIDIPropertyPrivate = CFSTR("private");

// The last object number handed out; clients and ports share the counter, and the first is 0x10001.
static uint32_t lastObject = 0x10000;

static MIDIObjectRef newObject(void)
{
    return __atomic_add_fetch(&lastObject, 1, __ATOMIC_SEQ_CST);
}

static int validObject(MIDIObjectRef object)
{
    return object >= 0x10001 && object <= lastObject;
}

OSStatus MIDIClientCreate(CFStringRef name, void *notifyProc, void *notifyRefCon, MIDIClientRef *client)
{
    if (!client)
        return kMIDIInvalidClient;
    *client = newObject();
    return 0;
}

OSStatus MIDIClientCreateWithBlock(CFStringRef name, MIDIClientRef *client, void *notifyBlock)
{
    if (!client)
        return kMIDIInvalidClient;
    *client = newObject();
    return 0;
}

OSStatus MIDIClientDispose(MIDIClientRef client)
{
    return validObject(client) ? 0 : kMIDIInvalidClient;
}

static OSStatus createPort(MIDIClientRef client, MIDIPortRef *port)
{
    if (!validObject(client))
        return kMIDIInvalidClient;
    if (!port)
        return kMIDIInvalidPort;
    *port = newObject();
    return 0;
}

OSStatus MIDIInputPortCreate(MIDIClientRef client, CFStringRef name, void *readProc, void *refCon, MIDIPortRef *port)
{
    return createPort(client, port);
}

OSStatus MIDIInputPortCreateWithBlock(MIDIClientRef client, CFStringRef name, MIDIPortRef *port, void *readBlock)
{
    return createPort(client, port);
}

OSStatus MIDIInputPortCreateWithProtocol(MIDIClientRef client, CFStringRef name, int protocol, MIDIPortRef *port, void *receiveBlock)
{
    return createPort(client, port);
}

OSStatus MIDIOutputPortCreate(MIDIClientRef client, CFStringRef name, MIDIPortRef *port)
{
    return createPort(client, port);
}

OSStatus MIDIPortDispose(MIDIPortRef port)
{
    return validObject(port) ? 0 : kMIDIInvalidPort;
}

OSStatus MIDIPortConnectSource(MIDIPortRef port, MIDIEndpointRef source, void *connRefCon)
{
    return (port >= 0x10001 && port <= lastObject) ? kMIDIObjectNotFound : kMIDIInvalidPort;
}

OSStatus MIDIPortDisconnectSource(MIDIPortRef port, MIDIEndpointRef source)
{
    return validObject(port) ? kMIDINoConnection : kMIDIInvalidPort;
}

ItemCount MIDIGetNumberOfDevices(void)
{
    return 0;
}

ItemCount MIDIGetNumberOfSources(void)
{
    return 0;
}

ItemCount MIDIGetNumberOfDestinations(void)
{
    return 0;
}

MIDIEndpointRef MIDIGetSource(ItemCount index)
{
    return 0;
}

MIDIEndpointRef MIDIGetDestination(ItemCount index)
{
    return 0;
}

MIDIDeviceRef MIDIGetDevice(ItemCount index)
{
    return 0;
}

OSStatus MIDIObjectGetIntegerProperty(MIDIObjectRef object, CFStringRef property, int32_t *value)
{
    return (object >= 0x10001 && object <= lastObject) ? kMIDIUnknownProperty : kMIDIObjectNotFound;
}

OSStatus MIDIObjectGetStringProperty(MIDIObjectRef object, CFStringRef property, CFStringRef *value)
{
    return (object >= 0x10001 && object <= lastObject) ? kMIDIUnknownProperty : kMIDIObjectNotFound;
}

OSStatus MIDISend(MIDIPortRef port, MIDIEndpointRef destination, const MIDIPacketList *packets)
{
    return (port >= 0x10001 && port <= lastObject) ? kMIDIObjectNotFound : kMIDIInvalidPort;
}

OSStatus MIDISendEventList(MIDIPortRef port, MIDIEndpointRef destination, const MIDIEventList *events)
{
    return (port >= 0x10001 && port <= lastObject) ? kMIDIObjectNotFound : kMIDIInvalidPort;
}

OSStatus MIDIFlushOutput(MIDIEndpointRef destination)
{
    return 0;
}

MIDIPacket *MIDIPacketListInit(MIDIPacketList *list)
{
    list->numPackets = 0;
    return &list->packet[0];
}

// Data is appended to the current packet when it has the same time, is not empty, neither starts nor continues a system exclusive message, and
// begins with a status byte; otherwise a new packet starts after the current one. Returns NULL when there is no room (or more than 256 bytes).
MIDIPacket *MIDIPacketListAdd(MIDIPacketList *list, ByteCount listSize, MIDIPacket *current, MIDITimeStamp time, ByteCount dataSize, const uint8_t *data)
{
    const uint8_t *end = (const uint8_t *)list + listSize;
    MIDIPacket *next;
    if (list->numPackets == 0) {
        next = &list->packet[0];
    } else {
        uint16_t length = current->length;
        if (current->timeStamp == time && length != 0 && dataSize != 0 && current->data[0] != 0xf0) {
            uint8_t status = data[0];
            if (status != 0xf0 && status >= 0x80) {
                ByteCount total = length + dataSize;
                if (total < 0x101 && current->data + total <= end) {
                    memcpy(current->data + length, data, dataSize);
                    current->length += dataSize;
                    return current;
                }
            }
        }
        next = (MIDIPacket *)(current->data + length);
    }
    if (dataSize > 0x100 || next->data + dataSize > end)
        return NULL;
    next->timeStamp = time;
    next->length = dataSize;
    memcpy(next->data, data, dataSize);
    list->numPackets++;
    return next;
}

MIDIEventPacket *MIDIEventListInit(MIDIEventList *list, int protocol)
{
    list->protocol = protocol;
    list->numPackets = 0;
    return &list->packet[0];
}

// Words are appended to the current packet when it has the same time and the sum stays within 64 words; otherwise a new packet starts.
MIDIEventPacket *MIDIEventListAdd(MIDIEventList *list, ByteCount listSize, MIDIEventPacket *current, MIDITimeStamp time, ItemCount wordCount, const uint32_t *words)
{
    const uint8_t *end = (const uint8_t *)list + listSize;
    MIDIEventPacket *next;
    if (list->numPackets == 0) {
        next = &list->packet[0];
    } else {
        uint32_t count = current->wordCount;
        if (current->timeStamp == time) {
            ItemCount total = count + wordCount;
            if (total <= 0x40) {
                if ((const uint8_t *)(current->words + total) <= end) {
                    memcpy(current->words + count, words, wordCount * 4);
                    current->wordCount += wordCount;
                    return current;
                }
            }
        }
        next = (MIDIEventPacket *)(current->words + count);
    }
    if (wordCount > 0x40 || (const uint8_t *)(next->words + wordCount) > end)
        return NULL;
    next->timeStamp = time;
    next->wordCount = wordCount;
    memcpy(next->words, words, wordCount * 4);
    list->numPackets++;
    return next;
}
