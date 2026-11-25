// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A menu of the program on the session bus, as com.canonical.dbusmenu.

#pragma once

#include "base/dbus.h"

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace cutefish::reef {

class MenuExporter
{
public:
    enum Flag : uint32_t {
        Separator = 1,
        Disabled = 2,
        Hidden = 4,
        Checked = 8,
        Mixed = 16,
    };

    // The items of a menu follow the one that opens it, one deeper.
    struct Item
    {
        std::string title;
        void* item = nullptr;
        int depth = 0;
        uint32_t flags = 0;
        std::string key;
        // ui::Modifier's.
        uint32_t modifiers = 0;
    };

    // Both are called with the items the program knows; opened with null for the menu itself.
    MenuExporter(base::dbus::Connection* connection, void (*activated)(void* item), void (*opened)(void* item));
    ~MenuExporter();

    const std::string& path() const { return m_path; }
    // Exports the menu, once.
    void setPath(const std::string& path);
    void setItems(std::vector<Item> items);

private:
    using Layout = std::tuple<int32_t, base::dbus::VariantMap, std::vector<base::dbus::Variant>>;
    using Properties = std::tuple<int32_t, base::dbus::VariantMap>;
    using RemovedProperties = std::tuple<int32_t, std::vector<std::string>>;

    void exportObject();
    bool contains(int id) const { return id > m_base && size_t(id - m_base) <= m_items.size(); }
    const Item& item(int id) const { return m_items[id - m_base - 1]; }
    bool hasChildren(int id) const;
    std::vector<int> children(int id) const;
    base::dbus::VariantMap properties(int id) const;
    Layout layout(int id, int depth) const;

    base::dbus::Connection* m_connection;
    std::string m_path;
    void (*m_activated)(void*);
    void (*m_opened)(void*);
    std::unique_ptr<base::dbus::Object> m_object;
    std::vector<Item> m_items;
    // The ids of the items before: a menu of another layout gets new ones.
    int m_base = 0;
    uint32_t m_revision = 1;
    std::shared_ptr<bool> m_alive = std::make_shared<bool>(true);
};

} // namespace cutefish::reef
