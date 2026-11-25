// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A status item of the program on the session bus, as org.kde.StatusNotifierItem.

#include "status_item.h"

#include "menu_bus.h"

#include "base/event_loop.h"

#include <algorithm>
#include <utility>

namespace cutefish::reef {

using base::dbus::MethodCall;
using base::dbus::ObjectPath;

static int nextItem = 1;

StatusItem::StatusItem(base::dbus::Connection* connection, void* context, Handler handler, std::string id)
    : m_connection(connection)
    , m_context(context)
    , m_handler(handler)
    , m_id(std::move(id))
    , m_path("/StatusNotifierItem/" + std::to_string(nextItem++))
{
    if (!m_connection)
        return;
    m_object = std::make_unique<base::dbus::Object>(m_connection, m_path, "org.kde.StatusNotifierItem");
    m_object->addProperty("Category", [] { return std::string("ApplicationStatus"); });
    m_object->addProperty("Id", [this] { return m_id; });
    m_object->addProperty("Title", [this] { return m_title; });
    m_object->addProperty("Status", [] { return std::string("Active"); });
    m_object->addProperty("WindowId", [] { return int32_t(0); });
    m_object->addProperty("IconThemePath", [] { return std::string(); });
    m_object->addProperty("IconName", [] { return std::string(); });
    m_object->addProperty("IconPixmap", [this] { return m_icon; });
    m_object->addProperty("OverlayIconName", [] { return std::string(); });
    m_object->addProperty("OverlayIconPixmap", [] { return Pixmaps(); });
    m_object->addProperty("AttentionIconName", [] { return std::string(); });
    m_object->addProperty("AttentionIconPixmap", [] { return Pixmaps(); });
    m_object->addProperty("AttentionMovieName", [] { return std::string(); });
    m_object->addProperty("ToolTip", [this] { return ToolTip(std::string(), Pixmaps(), m_toolTip, std::string()); });
    m_object->addProperty("ItemIsMenu", [this] { return m_menu != nullptr; });
    m_object->addProperty("Menu", [this] { return ObjectPath{m_menu ? m_menu->path() : "/NO_DBUSMENU"}; });
    m_object->addMethod("Activate", "ii", "", [this](MethodCall& call) {
        int32_t x = 0;
        int32_t y = 0;
        call.read(x, y);
        call.reply();
        activated(Activate, x, y);
    });
    m_object->addMethod("SecondaryActivate", "ii", "", [this](MethodCall& call) {
        int32_t x = 0;
        int32_t y = 0;
        call.read(x, y);
        call.reply();
        activated(SecondaryActivate, x, y);
    });
    m_object->addMethod("ContextMenu", "ii", "", [this](MethodCall& call) {
        int32_t x = 0;
        int32_t y = 0;
        call.read(x, y);
        call.reply();
        activated(ContextMenu, x, y);
    });
    m_object->addMethod("Scroll", "is", "", [](MethodCall& call) { call.reply(); });
    for (const char* name : {"NewTitle", "NewIcon", "NewAttentionIcon", "NewOverlayIcon", "NewToolTip", "NewMenu"})
        m_object->addSignal(name, "");
    m_object->addSignal("NewStatus", "s");
    m_object->publish();
    m_watch = m_connection->watchName("org.kde.StatusNotifierWatcher", [this, alive = m_alive](const std::string& owner) {
        if (*alive && !owner.empty())
            registerItem();
    });
}

StatusItem::~StatusItem()
{
    *m_alive = false;
}

void StatusItem::registerItem()
{
    const base::dbus::Proxy watcher(m_connection, "org.kde.StatusNotifierWatcher", "/StatusNotifierWatcher", "org.kde.StatusNotifierWatcher");
    watcher.send("RegisterStatusNotifierItem", m_path);
}

void StatusItem::activated(int action, int x, int y)
{
    // Out of the handler: the program may run a loop of its own.
    base::EventLoop::current()->post([this, action, x, y, alive = m_alive] {
        if (*alive && m_handler)
            m_handler(m_context, action, x, y);
    });
}

void StatusItem::setTitle(std::string title)
{
    if (title == m_title)
        return;
    m_title = std::move(title);
    if (m_object)
        m_object->emitSignal("NewTitle");
}

void StatusItem::setToolTip(std::string toolTip)
{
    if (toolTip == m_toolTip)
        return;
    m_toolTip = std::move(toolTip);
    if (m_object)
        m_object->emitSignal("NewToolTip");
}

void StatusItem::setIcon(const uint32_t* pixels, int width, int height)
{
    m_icon.clear();
    if (pixels && width > 0 && height > 0) {
        const size_t count = size_t(width) * size_t(height);
        std::vector<uint8_t> data(count * 4);
        for (size_t i = 0; i < count; i++) {
            const uint32_t alpha = pixels[i] >> 24;
            const auto unpremultiplied = [alpha](uint32_t value) { return alpha ? uint8_t(std::min((value * 255 + alpha / 2) / alpha, 255u)) : 0; };
            data[i * 4] = uint8_t(alpha);
            data[i * 4 + 1] = unpremultiplied(pixels[i] >> 16 & 0xff);
            data[i * 4 + 2] = unpremultiplied(pixels[i] >> 8 & 0xff);
            data[i * 4 + 3] = unpremultiplied(pixels[i] & 0xff);
        }
        m_icon.emplace_back(width, height, std::move(data));
    }
    if (m_object)
        m_object->emitSignal("NewIcon");
}

void StatusItem::setMenu(MenuExporter* menu)
{
    m_menu = menu;
    if (menu)
        menu->setPath(m_path + "/Menu");
    if (m_object)
        m_object->emitSignal("NewMenu");
}

} // namespace cutefish::reef
