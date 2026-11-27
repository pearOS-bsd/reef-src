// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// Debug aid: REEF_LOG_MIDI=1 prints the client and port calls of CoreMIDI and their results on their way to Reef's
// CoreMIDI.
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t MIDIObjectRef;
extern unsigned long MIDIGetNumberOfDevices(void);

static void *original(const char *name) {
    static void *image;
    if (!image) {
        Dl_info info;
        if (dladdr((const void *)MIDIGetNumberOfDevices, &info)) image = info.dli_fbase;
    }
    return image ? dlsym(image, name) : NULL;
}

static int logging(void) {
    static int on = -1;
    if (on < 0) on = getenv("REEF_LOG_MIDI") != NULL;
    return on;
}

int32_t MIDIClientCreate(CFStringRef name, void *notify, void *ref, MIDIObjectRef *client) {
    static int32_t (*f)(CFStringRef, void *, void *, MIDIObjectRef *);
    if (!f) f = original("MIDIClientCreate");
    int32_t r = f(name, notify, ref, client);
    if (logging()) fprintf(stderr, "midi: MIDIClientCreate -> %d client %u\n", r, client ? *client : 0);
    return r;
}

int32_t MIDIClientCreateWithBlock(CFStringRef name, MIDIObjectRef *client, void *block) {
    static int32_t (*f)(CFStringRef, MIDIObjectRef *, void *);
    if (!f) f = original("MIDIClientCreateWithBlock");
    int32_t r = f(name, client, block);
    if (logging()) fprintf(stderr, "midi: MIDIClientCreateWithBlock -> %d client %u\n", r, client ? *client : 0);
    return r;
}

int32_t MIDIInputPortCreate(MIDIObjectRef client, CFStringRef name, void *proc, void *ref, MIDIObjectRef *port) {
    static int32_t (*f)(MIDIObjectRef, CFStringRef, void *, void *, MIDIObjectRef *);
    if (!f) f = original("MIDIInputPortCreate");
    int32_t r = f(client, name, proc, ref, port);
    if (logging()) fprintf(stderr, "midi: MIDIInputPortCreate(%u) -> %d port %u\n", client, r, port ? *port : 0);
    return r;
}

int32_t MIDIInputPortCreateWithProtocol(MIDIObjectRef client, CFStringRef name, int32_t protocol, MIDIObjectRef *port, void *block) {
    static int32_t (*f)(MIDIObjectRef, CFStringRef, int32_t, MIDIObjectRef *, void *);
    if (!f) f = original("MIDIInputPortCreateWithProtocol");
    int32_t r = f(client, name, protocol, port, block);
    if (logging()) fprintf(stderr, "midi: MIDIInputPortCreateWithProtocol(%u, %d) -> %d port %u\n", client, protocol, r, port ? *port : 0);
    return r;
}

int32_t MIDIOutputPortCreate(MIDIObjectRef client, CFStringRef name, MIDIObjectRef *port) {
    static int32_t (*f)(MIDIObjectRef, CFStringRef, MIDIObjectRef *);
    if (!f) f = original("MIDIOutputPortCreate");
    int32_t r = f(client, name, port);
    if (logging()) fprintf(stderr, "midi: MIDIOutputPortCreate(%u) -> %d port %u\n", client, r, port ? *port : 0);
    return r;
}
