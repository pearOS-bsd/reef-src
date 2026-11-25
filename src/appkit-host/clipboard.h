// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#pragma once

#include "host.h"

// One type of what is copied or dragged.
struct reef_ui_mime_data
{
    const char* type;
    const void* bytes;
    size_t size;
    // In place of the bytes, when they are needed (drags).
    const void* (*provide)(void* context, size_t* size);
    void (*release)(void* context);
    void* context;
};
