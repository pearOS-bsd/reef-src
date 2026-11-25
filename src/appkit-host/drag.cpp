// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Drops over a window, and drags out of one.

#include "drag.h"

#include "views.h"

#include "ui/bitmap.h"
#include "ui/drag.h"
#include "ui/test_input.h"

#include <algorithm>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace cutefish::reef {

// What is dragged in one type, made when it is asked for.
struct DragData
{
    std::string bytes;
    const void* (*provide)(void* context, size_t* size) = nullptr;
    void (*release)(void* context) = nullptr;
    void* context = nullptr;

    ~DragData()
    {
        if (release)
            release(context);
    }

    const std::string& read()
    {
        if (const auto function = std::exchange(provide, nullptr)) {
            size_t size = 0;
            const void* data = function(context, &size);
            bytes.assign(static_cast<const char*>(data), data ? size : 0);
        }
        return bytes;
    }
};

using DragItems = std::vector<std::pair<std::string, std::shared_ptr<DragData>>>;

// A drag from elsewhere, for tests.
class TestOffer : public ui::DropOffer
{
public:
    const std::vector<std::string>& mimeTypes() const override { return types; }
    uint32_t sourceActions() const override { return actions; }

    void read(const std::string& mimeType, std::function<void(std::string data)> done) override
    {
        for (size_t i = 0; i < types.size(); i++) {
            if (types[i] == mimeType) {
                done(data[i]);
                return;
            }
        }
        done({});
    }

    std::vector<std::string> types;
    std::vector<std::string> data;
    uint32_t actions = ui::CopyAction;
};

// For tests: what the last drag has, and how one of a window ends.
static DragItems dragItems;
static unsigned dragActions = 0;
static std::function<void(ui::DropAction, bool)> windowDragEnded;

// 250
void hostTestWindowDragEnd(unsigned action, int dropped)
{
    if (const auto ended = std::exchange(windowDragEnded, nullptr))
        ended(ui::DropAction(action), dropped);
}

// 110
ssize_t hostTestDragData(const char* type, void* buffer, size_t size)
{
    for (const auto& [name, item] : dragItems) {
        if (type && name == type) {
            const std::string& bytes = item->read();
            if (buffer)
                memcpy(buffer, bytes.data(), std::min(size, bytes.size()));
            return ssize_t(bytes.size());
        }
    }
    return -1;
}

// 111
unsigned hostTestDragActions()
{
    return dragActions;
}

// 107
int hostDropOfferTypes(reef_ui_drop_offer* offer, char* buffer, int size)
{
    std::string types;
    if (offer) {
        for (const std::string& type : reinterpret_cast<ui::DropOffer*>(offer)->mimeTypes())
            types += type + "\n";
    }
    return copyString(types, buffer, size);
}

// 108
void hostDropOfferRead(reef_ui_drop_offer* offer, const char* type, void (*callback)(void* context, const void* bytes, size_t size, int read),
                       void* context)
{
    if (!offer || !type)
        return callback(context, nullptr, 0, 0);
    reinterpret_cast<ui::DropOffer*>(offer)->read(type, [callback, context](std::string data) {
        callback(context, data.data(), data.size(), 1);
    });
}

// 112
unsigned hostWindowTestDrop(reef_ui_window* window, double x, double y, const reef_ui_mime_data* items, unsigned count, unsigned actions)
{
    TestOffer offer;
    for (unsigned i = 0; i < count; i++) {
        offer.types.push_back(items[i].type ? items[i].type : "");
        offer.data.emplace_back(static_cast<const char*>(items[i].bytes), items[i].size);
    }
    offer.actions = actions ? actions : ui::CopyAction;
    ui::Window& target = *window->window;
    ui::TestInput::presentFrame(target);
    ui::TestInput::dragEnter(target, {x, y}, &offer);
    ui::TestInput::dragMove(target, {x, y});
    return ui::TestInput::drop(target);
}

static ui::DragSource dragSourceWith(const reef_ui_mime_data* items, unsigned count)
{
    auto data = std::make_shared<DragItems>();
    for (unsigned i = 0; i < count; i++) {
        auto item = std::make_shared<DragData>();
        if (items[i].provide) {
            item->provide = items[i].provide;
            item->release = items[i].release;
            item->context = items[i].context;
        } else if (items[i].bytes) {
            item->bytes.assign(static_cast<const char*>(items[i].bytes), items[i].size);
        }
        if (items[i].type)
            data->emplace_back(items[i].type, std::move(item));
    }
    dragItems = *data;
    ui::DragSource source;
    for (const auto& [type, item] : *data)
        source.mimeTypes.push_back(type);
    source.data = [data](const std::string& mimeType) -> std::string {
        for (const auto& [type, item] : *data) {
            if (type == mimeType)
                return item->read();
        }
        return {};
    };
    return source;
}

static void setIcon(ui::DragSource& source, const void* pixels, int width, int height, double scale, double hotX, double hotY)
{
    if (!pixels || width <= 0 || height <= 0)
        return;
    source.icon = ui::Bitmap::copyOf(pixels, width, height, size_t(width) * 4, ui::Bitmap::Format::Argb32Premultiplied);
    source.iconScale = scale > 0 ? scale : 1;
    source.hotSpot = {hotX, hotY};
}

// 109
int hostWindowStartDrag(reef_ui_window* window, const reef_ui_mime_data* items, unsigned count, unsigned actions, const void* pixels,
                        int width, int height, double scale, double hotX, double hotY, void (*finished)(void* context, unsigned action),
                        void* context)
{
    ui::DragSource source = dragSourceWith(items, count);
    source.actions = actions ? actions : ui::CopyAction;
    dragActions = source.actions;
    setIcon(source, pixels, width, height, scale, hotX, hotY);
    source.onFinished = [finished, context](ui::DropAction action) {
        if (finished)
            finished(context, action);
    };
    if (!window->root->startDrag(std::move(source)))
        return 1;
    window->root->pressed = false;
    return 0;
}

// 249
int hostWindowStartWindowDrag(reef_ui_window* window, reef_ui_window* dragged, double offsetX, double offsetY,
                              const reef_ui_mime_data* items, unsigned count, const void* pixels, int width, int height, double scale,
                              double hotX, double hotY, void (*finished)(void* context, unsigned action, int dropped), void* context)
{
    ui::DragSource source = dragSourceWith(items, count);
    source.actions = ui::MoveAction;
    setIcon(source, pixels, width, height, scale, hotX, hotY);
    source.attachedWindow = dragged ? dragged->window : nullptr;
    source.attachedOffset = {offsetX, offsetY};
    source.onEnded = [finished, context](ui::DropAction action, bool dropped) {
        if (finished)
            finished(context, action, dropped);
    };
    if (headless) {
        windowDragEnded = std::move(source.onEnded);
        window->root->pressed = false;
        return 0;
    }
    if (!window->root->startDrag(std::move(source)))
        return 1;
    window->root->pressed = false;
    return 0;
}

} // namespace cutefish::reef
