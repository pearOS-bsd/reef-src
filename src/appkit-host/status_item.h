// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A status item of the program on the session bus, as org.kde.StatusNotifierItem.

#pragma once

#include "base/dbus.h"

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace cutefish::reef {

class MenuExporter;

class StatusItem
{
public:
    enum Action {
        Activate,
        SecondaryActivate,
        ContextMenu,
    };
    using Handler = void (*)(void* context, int action, double x, double y);

    StatusItem(base::dbus::Connection* connection, void* context, Handler handler, std::string id);
    ~StatusItem();

    void setTitle(std::string title);
    void setToolTip(std::string toolTip);
    // Premultiplied ARGB; null for none.
    void setIcon(const uint32_t* pixels, int width, int height);
    void setMenu(MenuExporter* menu);

private:
    // Width, height and the pixels, unpremultiplied and big endian.
    using Pixmap = std::tuple<int32_t, int32_t, std::vector<uint8_t>>;
    using Pixmaps = std::vector<Pixmap>;
    // Icon name, icon, title and text.
    using ToolTip = std::tuple<std::string, Pixmaps, std::string, std::string>;

    void registerItem();
    void activated(int action, int x, int y);

    base::dbus::Connection* m_connection;
    void* m_context;
    Handler m_handler;
    std::string m_id;
    std::string m_path;
    std::string m_title;
    std::string m_toolTip;
    Pixmaps m_icon;
    MenuExporter* m_menu = nullptr;
    std::unique_ptr<base::dbus::Object> m_object;
    base::dbus::NameWatch m_watch;
    std::shared_ptr<bool> m_alive = std::make_shared<bool>(true);
};

} // namespace cutefish::reef
