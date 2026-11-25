// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#pragma once

// As MenuExporter::Item. Strings may be null.
struct reef_ui_menu_item
{
    const char* title;
    void* item;
    int depth;
    unsigned flags;
    const char* key;
    unsigned modifiers;
};
