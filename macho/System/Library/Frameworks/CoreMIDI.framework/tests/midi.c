// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// CoreMIDI: clients and ports (numbers only), the missing devices, the errors, and the packet list and event list helpers (merging, new packets,
// system exclusive, a full list).
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#pragma pack(push, 4)
typedef struct { uint64_t timeStamp; uint16_t length; uint8_t data[256]; } Packet;
typedef struct { uint32_t numPackets; Packet packet[1]; } PacketList;
typedef struct { uint64_t timeStamp; uint32_t wordCount; uint32_t words[64]; } EventPacket;
typedef struct { int32_t protocol; uint32_t numPackets; EventPacket packet[1]; } EventList;
#pragma pack(pop)

int MIDIClientCreate(const void *, void *, void *, uint32_t *);
int MIDIClientCreateWithBlock(const void *, uint32_t *, void *);
int MIDIClientDispose(uint32_t);
int MIDIInputPortCreate(uint32_t, const void *, void *, void *, uint32_t *);
int MIDIInputPortCreateWithBlock(uint32_t, const void *, uint32_t *, void *);
int MIDIInputPortCreateWithProtocol(uint32_t, const void *, int, uint32_t *, void *);
int MIDIOutputPortCreate(uint32_t, const void *, uint32_t *);
int MIDIPortDispose(uint32_t);
int MIDIPortConnectSource(uint32_t, uint32_t, void *);
int MIDIPortDisconnectSource(uint32_t, uint32_t);
unsigned long MIDIGetNumberOfDevices(void), MIDIGetNumberOfSources(void), MIDIGetNumberOfDestinations(void);
uint32_t MIDIGetSource(unsigned long), MIDIGetDestination(unsigned long), MIDIGetDevice(unsigned long);
int MIDIObjectGetIntegerProperty(uint32_t, const void *, int32_t *);
int MIDIObjectGetStringProperty(uint32_t, const void *, const void **);
int MIDISend(uint32_t, uint32_t, const PacketList *);
int MIDISendEventList(uint32_t, uint32_t, const EventList *);
int MIDIFlushOutput(uint32_t);
Packet *MIDIPacketListInit(PacketList *);
Packet *MIDIPacketListAdd(PacketList *, unsigned long, Packet *, uint64_t, unsigned long, const uint8_t *);
EventPacket *MIDIEventListInit(EventList *, int);
EventPacket *MIDIEventListAdd(EventList *, unsigned long, EventPacket *, uint64_t, unsigned long, const uint32_t *);
extern const void *kMIDIPropertyName, *kMIDIPropertyPrivate;
extern unsigned long CFStringGetLength(const void *);

int main(void)
{
    uint32_t c = 0, c2 = 0, p = 0, p2 = 0, p3 = 0, p4 = 0;
    printf("create %d %d, none %d %d\n", MIDIClientCreate(0, 0, 0, &c), MIDIClientCreateWithBlock(0, &c2, 0), MIDIClientCreate(0, 0, 0, 0), MIDIClientCreateWithBlock(0, 0, 0));
    printf("ids %x %x\n", c, c2);
    printf("ports %d %d %d %d\n", MIDIInputPortCreate(c, 0, 0, 0, &p), MIDIInputPortCreateWithBlock(c, 0, &p2, 0), MIDIInputPortCreateWithProtocol(c, 0, 1, &p3, 0), MIDIOutputPortCreate(c, 0, &p4));
    printf("port ids %x %x %x %x\n", p, p2, p3, p4);
    printf("bad ports %d %d %d %d\n", MIDIInputPortCreate(7, 0, 0, 0, &p), MIDIInputPortCreate(c, 0, 0, 0, 0), MIDIOutputPortCreate(0xfffff, 0, &p), MIDIOutputPortCreate(c, 0, 0));
    printf("dispose %d %d %d %d\n", MIDIClientDispose(c), MIDIClientDispose(5), MIDIClientDispose(0xffffff), MIDIPortDispose(p));
    printf("port dispose bad %d %d\n", MIDIPortDispose(3), MIDIPortDispose(0x7fffffff));
    printf("connect %d %d, disconnect %d %d\n", MIDIPortConnectSource(p, 1, 0), MIDIPortConnectSource(3, 1, 0), MIDIPortDisconnectSource(p, 1), MIDIPortDisconnectSource(3, 1));
    printf("counts %lu %lu %lu, %u %u %u\n", MIDIGetNumberOfDevices(), MIDIGetNumberOfSources(), MIDIGetNumberOfDestinations(), MIDIGetSource(0), MIDIGetDestination(0), MIDIGetDevice(0));
    int32_t v; const void *s;
    printf("properties %d %d %d %d\n", MIDIObjectGetIntegerProperty(c, kMIDIPropertyName, &v), MIDIObjectGetIntegerProperty(3, kMIDIPropertyName, &v), MIDIObjectGetStringProperty(c, kMIDIPropertyName, &s), MIDIObjectGetStringProperty(3, kMIDIPropertyName, &s));
    printf("names %lu %lu\n", CFStringGetLength(kMIDIPropertyName), CFStringGetLength(kMIDIPropertyPrivate));
    printf("send %d %d %d %d flush %d\n", MIDISend(p, 1, 0), MIDISend(3, 1, 0), MIDISendEventList(p, 1, 0), MIDISendEventList(3, 1, 0), MIDIFlushOutput(1));

    union { PacketList l; char raw[512]; } u;
    memset(&u, 0, sizeof u);
    Packet *cur = MIDIPacketListInit(&u.l);
    printf("init %lu %u\n", (char *)cur - (char *)&u, u.l.numPackets);
    uint8_t note[3] = { 0x90, 60, 100 }, off[3] = { 0x80, 60, 0 }, data[2] = { 1, 2 }, sysex[4] = { 0xf0, 1, 2, 0xf7 };
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 10, 3, note);
    printf("add1 %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 10, 3, off);
    printf("merge same time %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 10, 2, data);
    printf("data byte first %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 20, 3, note);
    printf("other time %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 20, 4, sysex);
    printf("sysex %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 20, 3, note);
    printf("after sysex %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 30, 0, note);
    printf("empty %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 30, 3, note);
    printf("after empty %lu n=%u len=%u\n", (char *)cur - (char *)&u, u.l.numPackets, cur->length);
    uint8_t big[300] = { 0x90 };
    printf("too big %p\n", (void *)MIDIPacketListAdd(&u.l, sizeof u, cur, 40, 257, big));
    printf("small list %p\n", (void *)MIDIPacketListAdd(&u.l, 40, cur, 40, 100, big));
    for (int i = 0; i < 100 && cur; i++) cur = MIDIPacketListAdd(&u.l, sizeof u, cur, 50 + i, 3, note);
    printf("full: n=%u\n", u.l.numPackets);

    union { EventList l; char raw[512]; } e;
    memset(&e, 0, sizeof e);
    EventPacket *ep = MIDIEventListInit(&e.l, 1);
    printf("einit %lu proto %d n=%u\n", (char *)ep - (char *)&e, e.l.protocol, e.l.numPackets);
    uint32_t w[4] = { 1, 2, 3, 4 };
    ep = MIDIEventListAdd(&e.l, sizeof e, ep, 5, 2, w);
    printf("eadd1 %lu n=%u wc=%u\n", (char *)ep - (char *)&e, e.l.numPackets, ep->wordCount);
    ep = MIDIEventListAdd(&e.l, sizeof e, ep, 5, 2, w + 2);
    printf("emerge %lu n=%u wc=%u w=%u,%u,%u,%u\n", (char *)ep - (char *)&e, e.l.numPackets, ep->wordCount, ep->words[0], ep->words[1], ep->words[2], ep->words[3]);
    ep = MIDIEventListAdd(&e.l, sizeof e, ep, 6, 4, w);
    printf("etime %lu n=%u wc=%u\n", (char *)ep - (char *)&e, e.l.numPackets, ep->wordCount);
    uint32_t many[70] = { 0 };
    printf("etoo many %p\n", (void *)MIDIEventListAdd(&e.l, sizeof e, ep, 7, 65, many));
    EventPacket *ep2 = MIDIEventListAdd(&e.l, sizeof e, ep, 6, 60, many);
    printf("e60 %lu n=%u wc=%u\n", ep2 ? (char *)ep2 - (char *)&e : -1L, e.l.numPackets, ep2 ? ep2->wordCount : 0);
    printf("efull %p\n", (void *)MIDIEventListAdd(&e.l, 100, ep, 8, 40, many));
    return 0;
}
