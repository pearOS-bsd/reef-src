// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// ForceFeedback: no device can be created and the calls answer with their fixed errors.
#include <stdio.h>

int FFIsForceFeedback(unsigned);
int FFCreateDevice(unsigned, void **);
int FFReleaseDevice(void *);
int FFDeviceGetForceFeedbackCapabilities(void *, void *);
int FFDeviceCreateEffect(void *, const void *, void *, void **);
int FFDeviceReleaseEffect(void *, void *);
int FFEffectSetParameters(void *, void *, unsigned);
int FFEffectStart(void *, unsigned, unsigned);
int FFEffectStop(void *);

int main(void)
{
    void *device = (void *)1, *effect = (void *)1;
    printf("%08x\n", FFIsForceFeedback(5));
    printf("%08x %p\n", FFCreateDevice(5, &device), device);
    printf("%08x\n", FFCreateDevice(5, 0));
    printf("%08x\n", FFReleaseDevice(0));
    printf("%08x\n", FFDeviceGetForceFeedbackCapabilities(0, 0));
    printf("%08x %p\n", FFDeviceCreateEffect(0, 0, 0, &effect), effect);
    printf("%08x\n", FFDeviceCreateEffect(0, 0, 0, 0));
    printf("%08x\n", FFDeviceReleaseEffect(0, 0));
    printf("%08x\n", FFEffectSetParameters(0, 0, 0));
    printf("%08x\n", FFEffectStart(0, 1, 0));
    printf("%08x\n", FFEffectStop(0));
    return 0;
}
