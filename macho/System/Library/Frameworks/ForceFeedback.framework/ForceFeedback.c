// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ForceFeedback: there is no force feedback hardware; devices cannot be made and every call reports that.

#include <stddef.h>

typedef int HRESULT;

#define E_NOINTERFACE ((HRESULT)0x80004002)
#define E_INVALIDARG ((HRESULT)0x80070057)

HRESULT FFIsForceFeedback(unsigned service)
{
    return E_NOINTERFACE;
}

HRESULT FFCreateDevice(unsigned service, void **device)
{
    if (device)
        *device = NULL;
    return E_NOINTERFACE;
}

HRESULT FFReleaseDevice(void *device)
{
    return E_INVALIDARG;
}

HRESULT FFDeviceGetForceFeedbackCapabilities(void *device, void *capabilities)
{
    return E_INVALIDARG;
}

HRESULT FFDeviceCreateEffect(void *device, const void *uuid, void *definition, void **effect)
{
    if (effect)
        *effect = NULL;
    return E_INVALIDARG;
}

HRESULT FFDeviceReleaseEffect(void *device, void *effect)
{
    return E_INVALIDARG;
}

HRESULT FFEffectSetParameters(void *effect, void *parameters, unsigned flags)
{
    return E_INVALIDARG;
}

HRESULT FFEffectStart(void *effect, unsigned iterations, unsigned flags)
{
    return E_INVALIDARG;
}

HRESULT FFEffectStop(void *effect)
{
    return E_INVALIDARG;
}
