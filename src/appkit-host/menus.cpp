// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Menus: exported on the session bus, or the toolkit's, and status items.

#include "host.h"

#include "menu_bus.h"
#include "status_item.h"

#include "ui/menu.h"

#include <map>

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

struct reef_ui_menu_level
{
    cutefish::ui::Menu* menu = nullptr;
    // 0 an item, 1 a separator, 2 a submenu.
    std::vector<int> kinds;
    std::vector<void*> items;
    std::vector<std::unique_ptr<reef_ui_menu_level>> submenus;
};

struct reef_ui_menu
{
    std::unique_ptr<cutefish::reef::MenuExporter> exporter;
    std::vector<cutefish::reef::MenuExporter::Item> items;
    // When it is the main menu or the one of the dock.
    std::unique_ptr<cutefish::ui::Menu> menu;
    reef_ui_menu_level level;
};

struct reef_ui_status_item
{
    std::unique_ptr<cutefish::reef::StatusItem> item;
};

namespace cutefish::reef {

// Of reef_ui_menu_item, beyond those of the exporter.
enum MenuItemFlag : uint32_t {
    ApplicationMenu = 32,
    WithoutSubmenu = 64,
};

// 77
reef_ui_menu* hostMenuCreate()
{
    auto menu = new reef_ui_menu;
    menu->exporter = std::make_unique<MenuExporter>(sessionBus(), callbacks.menuItem, callbacks.menuOpens);
    return menu;
}

// 78
void hostMenuDestroy(reef_ui_menu* menu)
{
    if (menu->menu && application && application->mainMenu() == menu->menu.get())
        application->setMainMenu(nullptr);
    if (menu->menu && application && application->dockMenu() == menu->menu.get())
        application->setDockMenu(nullptr);
    delete menu;
}

// The name of a key equivalent, which may be one of the characters of function keys.
static std::string keyName(const std::string& key)
{
    static const std::map<std::string, std::string> names = {
        {"\t", "Tab"},
        {"\r", "Return"},
        {"\x03", "KP_Enter"},
        {"\x1b", "Escape"},
        {"\b", "BackSpace"},
        {"\x7f", "BackSpace"},
        {" ", "space"},
        {"", "Up"},
        {"", "Down"},
        {"", "Left"},
        {"", "Right"},
        {"", "Delete"},
        {"", "Home"},
        {"", "End"},
        {"", "Page_Up"},
        {"", "Page_Down"},
    };
    const auto found = names.find(key);
    if (found != names.end())
        return found->second;
    // U+F704 is F1.
    if (key.size() == 3 && key[0] == '\xef' && key[1] == '\x9c') {
        const int code = uint8_t(key[2]);
        if (code >= 0x84 && code <= 0xa6)
            return "F" + std::to_string(code - 0x83);
    }
    return key;
}

// The items from `first` on that are below the one of this depth. The menu is only made anew when its kinds of items change.
static void applyLevel(reef_ui_menu_level& level, const std::vector<MenuExporter::Item>& items, size_t first, int depth)
{
    std::vector<size_t> indices;
    std::vector<int> kinds;
    for (size_t i = first; i < items.size() && items[i].depth > depth; i++) {
        if (items[i].depth != depth + 1)
            continue;
        indices.push_back(i);
        if (items[i].flags & MenuExporter::Separator)
            kinds.push_back(1);
        else if (!(items[i].flags & WithoutSubmenu) && i + 1 < items.size() && items[i + 1].depth > items[i].depth)
            kinds.push_back(2);
        else
            kinds.push_back(0);
    }
    ui::Menu* menu = level.menu;
    bool made = false;
    if (level.kinds != kinds) {
        menu->clear();
        level.submenus.clear();
        made = true;
    }
    level.kinds = kinds;
    level.items.resize(kinds.size());
    level.submenus.resize(kinds.size());
    for (size_t i = 0; i < kinds.size(); i++) {
        const MenuExporter::Item& item = items[indices[i]];
        level.items[i] = item.item;
        if (kinds[i] == 1) {
            if (made)
                menu->addSeparator();
            menu->setItemVisible(i, !(item.flags & MenuExporter::Hidden));
            continue;
        }
        if (made) {
            if (kinds[i] == 2) {
                auto submenu = std::make_unique<reef_ui_menu_level>();
                submenu->menu = menu->addSubmenu(item.title);
                submenu->menu->onAboutToShow = [&level, i] {
                    if (callbacks.menuOpens)
                        callbacks.menuOpens(level.items[i]);
                };
                level.submenus[i] = std::move(submenu);
            } else {
                menu->addItem(item.title, [&level, i] {
                    base::EventLoop::current()->post([item = level.items[i]] {
                        if (callbacks.menuItem)
                            callbacks.menuItem(item);
                    });
                });
            }
        }
        menu->setItemText(i, item.title);
        menu->setItemEnabled(i, !(item.flags & MenuExporter::Disabled));
        menu->setItemVisible(i, !(item.flags & MenuExporter::Hidden));
        menu->setItemState(i, item.flags & MenuExporter::Checked ? ui::MenuItemState::On
                               : item.flags & MenuExporter::Mixed ? ui::MenuItemState::Mixed
                                                                  : ui::MenuItemState::Off);
        menu->setItemKeyEquivalent(i, item.key.empty() ? std::string() : keyName(item.key), item.modifiers);
        if (reef_ui_menu_level* submenu = level.submenus[i].get()) {
            submenu->menu->setRole(item.flags & ApplicationMenu ? ui::MenuRole::Application : ui::MenuRole::None);
            applyLevel(*submenu, items, indices[i] + 1, item.depth);
        }
    }
}

// 79
void hostMenuSetItems(reef_ui_menu* menu, const reef_ui_menu_item* items, unsigned count)
{
    std::vector<MenuExporter::Item> list;
    for (unsigned i = 0; i < count; i++) {
        const reef_ui_menu_item& item = items[i];
        list.push_back({item.title ? item.title : "", item.item, item.depth, item.flags, item.key ? item.key : "", item.modifiers});
    }
    menu->items = list;
    menu->exporter->setItems(std::move(list));
    if (menu->menu)
        applyLevel(menu->level, menu->items, 0, -1);
}

// 80
void hostMenuSetMain(reef_ui_menu* menu)
{
    if (!application)
        return;
    if (!menu->menu) {
        menu->menu = std::make_unique<ui::Menu>();
        menu->level.menu = menu->menu.get();
        applyLevel(menu->level, menu->items, 0, -1);
    }
    application->setMainMenu(menu->menu.get());
}

// 283
void hostMenuSetDock(reef_ui_menu* menu)
{
    if (!application)
        return;
    if (!menu->menu) {
        menu->menu = std::make_unique<ui::Menu>();
        menu->level.menu = menu->menu.get();
        menu->menu->onAboutToShow = [] {
            if (callbacks.dockMenu)
                callbacks.dockMenu();
        };
        applyLevel(menu->level, menu->items, 0, -1);
    }
    application->setDockMenu(menu->menu.get());
}

static void statusItemActivated(void* context, int action, double x, double y)
{
    const ui::PointF origin = screensOrigin();
    if (callbacks.statusItem)
        callbacks.statusItem(context, action, x - origin.x, y - origin.y);
}

// 113
reef_ui_status_item* hostStatusItemCreate(void* context)
{
    auto item = new reef_ui_status_item;
    item->item = std::make_unique<StatusItem>(sessionBus(), context, statusItemActivated, applicationId);
    return item;
}

// 114
void hostStatusItemDestroy(reef_ui_status_item* item)
{
    delete item;
}

// 115
// Premultiplied ARGB; null for none.
void hostStatusItemSetIcon(reef_ui_status_item* item, const uint32_t* pixels, int width, int height)
{
    item->item->setIcon(pixels, width, height);
}

// 116
void hostStatusItemSetTitle(reef_ui_status_item* item, const char* title)
{
    item->item->setTitle(title ? title : "");
}

// 117
void hostStatusItemSetToolTip(reef_ui_status_item* item, const char* toolTip)
{
    item->item->setToolTip(toolTip ? toolTip : "");
}

// 118
void hostStatusItemSetMenu(reef_ui_status_item* item, reef_ui_menu* menu)
{
    item->item->setMenu(menu ? menu->exporter.get() : nullptr);
}

// 185
// Those that are windows.
int hostStatusItemsSupported()
{
    if (headless)
        return headlessStatusItems;
    return ui::StatusItem::isSupported();
}

// 186
void hostWindowSetStatusItemIdentifier(reef_ui_window* window, const char* identifier)
{
    if (window->statusItem)
        window->statusItem->setIdentifier(identifier ? identifier : "");
}

// 187
void hostWindowSetStatusItemHighlighted(reef_ui_window* window, int highlighted)
{
    if (window->statusItem)
        window->statusItem->setHighlighted(highlighted);
}

// 188
void hostWindowStatusItemState(reef_ui_window* window, double* rect, int* dark, uint32_t* color, int* highlighted)
{
    std::fill(rect, rect + 4, 0.0);
    *dark = 0;
    *color = 0xd9000000;
    *highlighted = 0;
    if (!window->statusItem)
        return;
    const ui::RectF& screenRect = window->statusItem->screenRect();
    const ui::PointF origin = screensOrigin();
    rect[0] = screenRect.x - origin.x;
    rect[1] = screenRect.y - origin.y;
    rect[2] = screenRect.width;
    rect[3] = screenRect.height;
    *dark = window->statusItem->darkAppearance();
    *color = window->statusItem->foregroundColor().argb();
    *highlighted = window->statusItem->isHighlighted();
}

} // namespace cutefish::reef
