// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "views.h"

namespace cutefish::reef {

unsigned modifiersOf(uint32_t modifiers)
{
    return modifiers & (ui::ShiftModifier | ui::ControlModifier | ui::AltModifier | ui::SuperModifier);
}

int buttonOf(ui::MouseButton button)
{
    switch (button) {
    case ui::MouseButton::Left:
        return 0;
    case ui::MouseButton::Right:
        return 1;
    case ui::MouseButton::Middle:
        return 2;
    case ui::MouseButton::Back:
        return 3;
    case ui::MouseButton::Forward:
        return 4;
    }
    return 0;
}

void BaseView::paint(ui::Painter& painter)
{
    m_window->root->updateScale();
    if (!callbacks.draw)
        return;
    const std::vector<ui::RectF> rects = damage(painter);
    if (rects.empty())
        return;
    beginDrawing();
    reef_ui_gc gc;
    gc.canvas = std::make_unique<ui::Canvas2D>(painter);
    if (rects.size() > 1 && callbacks.drawRects) {
        std::vector<double> numbers;
        for (const ui::RectF& rect : rects)
            numbers.insert(numbers.end(), {rect.x, rect.y, rect.width, rect.height});
        callbacks.drawRects(m_window->context, &gc, numbers.data(), unsigned(rects.size()));
    } else {
        ui::RectF all = rects.front();
        for (const ui::RectF& rect : rects)
            all = united(all, rect);
        callbacks.draw(m_window->context, &gc, all.x, all.y, all.width, all.height);
    }
    endDrawing();
}

} // namespace cutefish::reef
