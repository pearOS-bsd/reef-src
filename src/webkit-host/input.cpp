// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.

#include "web.h"

#include <algorithm>
#include <WebKit/WKEvent.h>

namespace cutefish::reef::web {

// 44
void setComposition(View* view, const char* text, int cursor)
{
    WKViewSetComposition(view->view, text ? text : "", uint32_t(std::max(cursor, 0)));
}

// 45
void confirmComposition(View* view, const char* text) { WKViewConfirmComposition(view->view, text ? text : ""); }
// 46
void cancelComposition(View* view) { WKViewCancelComposition(view->view); }
// 47
void popupChosen(View* view, int index) { WKViewPopupMenuItemSelected(view->view, index); }
// 48
void menuChosen(View* view, int index) { WKViewContextMenuItemSelected(view->view, index); }

} // namespace cutefish::reef::web
