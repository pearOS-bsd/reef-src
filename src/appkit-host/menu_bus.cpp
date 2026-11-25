// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// A menu of the program on the session bus, as com.canonical.dbusmenu.

#include "menu_bus.h"

#include "base/event_loop.h"
#include "ui/event.h"

#include <utility>

namespace cutefish::reef {

using base::dbus::MethodCall;
using base::dbus::Variant;
using base::dbus::VariantMap;

MenuExporter::MenuExporter(base::dbus::Connection* connection, void (*activated)(void*), void (*opened)(void*))
    : m_connection(connection)
    , m_activated(activated)
    , m_opened(opened)
{
}

MenuExporter::~MenuExporter()
{
    *m_alive = false;
}

void MenuExporter::setPath(const std::string& path)
{
    if (m_object || !m_connection)
        return;
    m_path = path;
    exportObject();
}

// The ids of the items stay while the menu keeps its shape.
static bool sameLayout(const std::vector<MenuExporter::Item>& a, const std::vector<MenuExporter::Item>& b)
{
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].depth != b[i].depth || ((a[i].flags ^ b[i].flags) & MenuExporter::Separator))
            return false;
    }
    return true;
}

void MenuExporter::setItems(std::vector<Item> items)
{
    const bool same = sameLayout(items, m_items);
    if (!same)
        m_base += int(m_items.size());
    const std::vector<Item> previous = std::exchange(m_items, std::move(items));
    if (!m_object)
        return;
    m_revision++;
    if (same) {
        std::vector<Properties> updated;
        for (size_t i = 0; i < m_items.size(); i++) {
            const Item& before = previous[i];
            const Item& now = m_items[i];
            if (before.title != now.title || before.flags != now.flags || before.key != now.key || before.modifiers != now.modifiers)
                updated.emplace_back(m_base + int(i) + 1, properties(m_base + int(i) + 1));
        }
        if (!updated.empty())
            m_object->emitSignal("ItemsPropertiesUpdated", updated, std::vector<RemovedProperties>());
        return;
    }
    m_object->emitSignal("LayoutUpdated", m_revision, int32_t(0));
    for (size_t i = 0; i < m_items.size(); i++) {
        const int id = m_base + int(i) + 1;
        if (hasChildren(id))
            m_object->emitSignal("LayoutUpdated", m_revision, id);
    }
}

bool MenuExporter::hasChildren(int id) const
{
    if (!contains(id))
        return false;
    const int index = id - m_base - 1;
    return size_t(index + 1) < m_items.size() && m_items[index + 1].depth == m_items[index].depth + 1;
}

std::vector<int> MenuExporter::children(int id) const
{
    std::vector<int> children;
    const int depth = id ? item(id).depth + 1 : 0;
    for (size_t i = id ? id - m_base : 0; i < m_items.size(); i++) {
        if (m_items[i].depth < depth)
            break;
        if (m_items[i].depth == depth)
            children.push_back(m_base + int(i) + 1);
    }
    return children;
}

// An underscore alone would mark the access key.
static std::string escaped(const std::string& title)
{
    std::string label;
    for (char c : title) {
        if (c == '_')
            label += "__";
        else
            label += c;
    }
    return label;
}

VariantMap MenuExporter::properties(int id) const
{
    const Item& item = this->item(id);
    VariantMap properties;
    if (item.flags & Separator)
        properties["type"] = std::string("separator");
    else
        properties["label"] = escaped(item.title);
    if (item.flags & Disabled)
        properties["enabled"] = false;
    if (item.flags & Hidden)
        properties["visible"] = false;
    if (item.flags & (Checked | Mixed)) {
        properties["toggle-type"] = std::string("checkmark");
        properties["toggle-state"] = item.flags & Checked ? 1 : -1;
    }
    if (!item.key.empty() && !(item.flags & Separator)) {
        std::vector<std::string> keys;
        if (item.modifiers & ui::ControlModifier)
            keys.push_back("Control");
        if (item.modifiers & ui::AltModifier)
            keys.push_back("Alt");
        if (item.modifiers & ui::ShiftModifier)
            keys.push_back("Shift");
        if (item.modifiers & ui::SuperModifier)
            keys.push_back("Super");
        keys.push_back(item.key);
        properties["shortcut"] = std::vector<std::vector<std::string>>{keys};
    }
    if (hasChildren(id))
        properties["children-display"] = std::string("submenu");
    return properties;
}

MenuExporter::Layout MenuExporter::layout(int id, int depth) const
{
    VariantMap properties;
    if (id == 0)
        properties["children-display"] = std::string("submenu");
    else
        properties = this->properties(id);
    std::vector<Variant> children;
    if (depth != 0) {
        for (int child : this->children(id))
            children.push_back(layout(child, depth - 1));
    }
    return {id, properties, children};
}

void MenuExporter::exportObject()
{
    m_object = std::make_unique<base::dbus::Object>(m_connection, m_path, "com.canonical.dbusmenu");
    m_object->addProperty("Version", [] { return uint32_t(3); });
    m_object->addProperty("TextDirection", [] { return std::string("ltr"); });
    m_object->addProperty("Status", [] { return std::string("normal"); });
    m_object->addProperty("IconThemePath", [] { return std::vector<std::string>(); });
    m_object->addMethod("GetLayout", "iias", "u(ia{sv}av)", [this](MethodCall& call) {
        int32_t parent = 0;
        int32_t depth = -1;
        std::vector<std::string> names;
        call.read(parent, depth, names);
        if (parent != 0 && !contains(parent)) {
            call.replyError("com.canonical.dbusmenu.Error", "No such item");
            return;
        }
        call.reply(m_revision, layout(parent, depth));
    });
    m_object->addMethod("GetGroupProperties", "aias", "a(ia{sv})", [this](MethodCall& call) {
        std::vector<int32_t> ids;
        std::vector<std::string> names;
        call.read(ids, names);
        std::vector<Properties> result;
        for (int id : ids) {
            if (contains(id))
                result.emplace_back(id, properties(id));
        }
        call.reply(result);
    });
    m_object->addMethod("GetProperty", "is", "v", [this](MethodCall& call) {
        int32_t id = 0;
        std::string name;
        call.read(id, name);
        if (!contains(id)) {
            call.replyError("com.canonical.dbusmenu.Error", "No such item");
            return;
        }
        const VariantMap properties = this->properties(id);
        const auto it = properties.find(name);
        call.reply(it == properties.end() ? Variant(std::string()) : it->second);
    });
    m_object->addMethod("Event", "isvu", "", [this](MethodCall& call) {
        int32_t id = 0;
        std::string type;
        call.read(id, type);
        call.reply();
        if (type != "clicked" || !contains(id))
            return;
        const Item& item = this->item(id);
        if (item.flags & (Separator | Disabled))
            return;
        // Out of the handler: the program may run a loop of its own.
        base::EventLoop::current()->post([this, item = item.item, alive = m_alive] {
            if (*alive && m_activated)
                m_activated(item);
        });
    });
    m_object->addMethod("AboutToShow", "i", "b", [this](MethodCall& call) {
        int32_t id = 0;
        call.read(id);
        if (id != 0 && !contains(id)) {
            call.replyError("com.canonical.dbusmenu.Error", "No such item");
            return;
        }
        if (m_opened)
            m_opened(id == 0 ? nullptr : item(id).item);
        call.reply(true);
    });
    m_object->addSignal("LayoutUpdated", "ui");
    m_object->addSignal("ItemsPropertiesUpdated", "a(ia{sv})a(ias)");
    m_object->addSignal("ItemActivationRequested", "iu");
    m_object->publish();
}

} // namespace cutefish::reef
