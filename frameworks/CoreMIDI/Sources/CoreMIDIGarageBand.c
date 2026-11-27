// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <CoreFoundation/CoreFoundation.h>

// Property names of MIDI objects (CoreMIDI sets them at run time on Apple's side)
__attribute__((weak)) const CFStringRef kMIDIPropertyConnectionUniqueID = CFSTR("connUniqueID");
__attribute__((weak)) const CFStringRef kMIDIPropertyDriverOwner = CFSTR("driver");
__attribute__((weak)) const CFStringRef kMIDIPropertyIsBroadcast = CFSTR("isBroadcast");
__attribute__((weak)) const CFStringRef kMIDIPropertyProtocolID = CFSTR("protocol");
__attribute__((weak)) const CFStringRef kMIDIPropertyReceiveChannels = CFSTR("receiveChannels");
__attribute__((weak)) const CFStringRef kMIDIPropertySupportsGeneralMIDI = CFSTR("supportsGeneralMIDI");
__attribute__((weak)) const CFStringRef kMIDIPropertyTransmitChannels = CFSTR("transmitChannels");

// Virtual endpoints and the device tree: there are no MIDI devices; virtual sources and destinations exist
// as numbers, what they receive goes nowhere.
#include <stdatomic.h>
typedef uint32_t MIDIObjectRef;
static _Atomic uint32_t nextVirtual = 0x7f000000;

int32_t MIDISourceCreateWithProtocol(MIDIObjectRef client, CFStringRef name, int32_t protocol, MIDIObjectRef *out) {
    *out = atomic_fetch_add(&nextVirtual, 1);
    return 0;
}
int32_t MIDIDestinationCreateWithProtocol(MIDIObjectRef client, CFStringRef name, int32_t protocol, MIDIObjectRef *out, void *block) {
    *out = atomic_fetch_add(&nextVirtual, 1);
    return 0;
}
int32_t MIDIEndpointDispose(MIDIObjectRef endpoint) { return 0; }
int32_t MIDIReceived(MIDIObjectRef source, const void *packets) { return 0; }
int32_t MIDIReceivedEventList(MIDIObjectRef source, const void *events) { return 0; }
int32_t MIDIRestart(void) { return 0; }
unsigned long MIDIDeviceGetNumberOfEntities(MIDIObjectRef device) { return 0; }
MIDIObjectRef MIDIDeviceGetEntity(MIDIObjectRef device, unsigned long index) { return 0; }
unsigned long MIDIEntityGetNumberOfSources(MIDIObjectRef entity) { return 0; }
unsigned long MIDIEntityGetNumberOfDestinations(MIDIObjectRef entity) { return 0; }
MIDIObjectRef MIDIEntityGetSource(MIDIObjectRef entity, unsigned long index) { return 0; }
MIDIObjectRef MIDIEntityGetDestination(MIDIObjectRef entity, unsigned long index) { return 0; }
int32_t MIDIEntityGetDevice(MIDIObjectRef entity, MIDIObjectRef *device) { if (device) *device = 0; return -10842; }   // kMIDIObjectNotFound
int32_t MIDIEndpointGetEntity(MIDIObjectRef endpoint, MIDIObjectRef *entity) { if (entity) *entity = 0; return -10842; }
int32_t MIDIObjectFindByUniqueID(int32_t uniqueID, MIDIObjectRef *object, int32_t *type) { if (object) *object = 0; return -10842; }
int32_t MIDIObjectGetDataProperty(MIDIObjectRef object, CFStringRef property, CFDataRef *data) { return -10835; }   // kMIDIUnknownProperty
int32_t MIDIObjectSetIntegerProperty(MIDIObjectRef object, CFStringRef property, int32_t value) { return 0; }
