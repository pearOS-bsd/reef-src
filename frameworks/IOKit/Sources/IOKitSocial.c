// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

#include <IOKit/IOKitLib.h>
#include <IOKit/pwr_mgt/IOPMLib.h>

// Assertion ids are never 0 (kIOPMNullAssertionID); an id that is not a live assertion is a bad argument (0xe00002c2, observed on macOS 26).
IOReturn IOPMAssertionSetProperty(IOPMAssertionID assertionID, CFStringRef theProperty, CFTypeRef theValue) {
    (void)theProperty; (void)theValue;
    return assertionID == kIOPMNullAssertionID ? kIOReturnBadArgument : kIOReturnSuccess;
}

// A connection to the root power domain; a Reef host has none: no connection (IO_OBJECT_NULL), as a failed lookup.
io_connect_t IOPMFindPowerManagement(mach_port_t master_device_port) {
    (void)master_device_port;
    return IO_OBJECT_NULL;
}

// Sleeping the system needs that connection.
IOReturn IOPMSleepSystem(io_connect_t fb) {
    (void)fb;
    return kIOReturnBadArgument;
}
