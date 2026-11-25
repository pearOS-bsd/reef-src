// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#pragma once

struct reef_ui_scroll_axis
{
    double minimum;
    double maximum;
    double viewport;
    int enabled;
};

// What a scroller moves.
struct reef_ui_scroller_callbacks
{
    void* context;
    void (*axes)(void* context, reef_ui_scroll_axis* horizontal, reef_ui_scroll_axis* vertical);
    void (*position)(void* context, double* x, double* y);
    void (*applyPosition)(void* context, double x, double y);
    void (*movingChanged)(void* context, int moving);
};
