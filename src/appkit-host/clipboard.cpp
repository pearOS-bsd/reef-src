// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The clipboard.

#include "clipboard.h"

#include "ui/clipboard.h"

namespace cutefish::reef {

static bool watching;
static base::Subscription subscription;
// How often something was copied.
static long changes;

// 89
void hostClipboardRequestText(void (*callback)(void* context, const char* text), void* context)
{
    if (!application)
        return callback(context, "");
    ui::Clipboard::requestText([callback, context](std::string text) { callback(context, text.c_str()); });
}

// 105
int hostClipboardOwnsData()
{
    return application && ui::Clipboard::ownsData();
}

// 88
void hostClipboardSetText(const char* text)
{
    if (application)
        ui::Clipboard::setText(text ? std::string(text) : std::string());
}

// 106
void hostClipboardRequestData(const char* type, void (*callback)(void* context, const void* bytes, size_t size, int found), void* context)
{
    if (!application)
        return callback(context, nullptr, 0, 0);
    ui::Clipboard::requestData(type ? type : "", [callback, context](std::optional<std::string> data) {
        if (data)
            callback(context, data->data(), data->size(), 1);
        else
            callback(context, nullptr, 0, 0);
    });
}

static void watchClipboard()
{
    if (watching || !application)
        return;
    watching = true;
    subscription = ui::Clipboard::changed().connect([](ui::ClipboardMode mode) {
        if (mode == ui::ClipboardMode::Clipboard)
            changes++;
    });
}

// 102
int hostClipboardSetData(const reef_ui_mime_data* items, unsigned count)
{
    if (!application)
        return 0;
    watchClipboard();
    ui::MimeData data;
    for (unsigned i = 0; i < count; i++) {
        if (items[i].type)
            data.setData(items[i].type, std::string(static_cast<const char*>(items[i].bytes), items[i].size));
    }
    ui::Clipboard::setMimeData(data);
    return 1;
}

// 103
long hostClipboardChanges()
{
    watchClipboard();
    return changes;
}

// 104
// The types one after another, each with a line end.
int hostClipboardTypes(char* buffer, int size)
{
    if (!application)
        return copyString({}, buffer, size);
    watchClipboard();
    std::string types;
    for (const std::string& type : ui::Clipboard::mimeTypes())
        types += type + '\n';
    return copyString(types, buffer, size);
}

} // namespace cutefish::reef
