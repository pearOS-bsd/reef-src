// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Drops over a window, and drags out of one.

#pragma once

#include "clipboard.h"
#include "host.h"

#include <cstddef>
#include <sys/types.h>

// What the program gets while something is dragged over a window: a ui::DropOffer.
struct reef_ui_drop_offer;

namespace cutefish::reef {

// The types one after another, each with a line end; the length of them all.
int hostDropOfferTypes(reef_ui_drop_offer* offer, char* buffer, int size);
void hostDropOfferRead(reef_ui_drop_offer* offer, const char* type,
                       void (*callback)(void* context, const void* bytes, size_t size, int read), void* context);

// Zero when the drag started. The picture is premultiplied ARGB and may be null.
int hostWindowStartDrag(reef_ui_window* window, const reef_ui_mime_data* items, unsigned count, unsigned actions, const void* pixels,
                        int width, int height, double scale, double hotX, double hotY, void (*finished)(void* context, unsigned action),
                        void* context);
// Another window moves with the pointer, which is at the offset in it.
int hostWindowStartWindowDrag(reef_ui_window* window, reef_ui_window* dragged, double offsetX, double offsetY,
                              const reef_ui_mime_data* items, unsigned count, const void* pixels, int width, int height, double scale,
                              double hotX, double hotY, void (*finished)(void* context, unsigned action, int dropped), void* context);

// For tests, which have no pointer.
// A drop of a drag from elsewhere; the action that the window took.
unsigned hostWindowTestDrop(reef_ui_window* window, double x, double y, const reef_ui_mime_data* items, unsigned count, unsigned actions);
// What the last drag has in a type: its size, or -1 without any.
ssize_t hostTestDragData(const char* type, void* buffer, size_t size);
unsigned hostTestDragActions();
// The end of a drag of a window.
void hostTestWindowDragEnd(unsigned action, int dropped);

} // namespace cutefish::reef
