// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import SwiftUI
import TipKit

extension View {
    // Tips are not shown: Reef has no tip popover yet.
    public func popoverTip(_ tip: (any Tip)?, isPresented: Binding<Bool>? = nil, attachmentAnchor: PopoverAttachmentAnchor = .rect(.bounds),
                           arrowEdge: Edge? = nil, action: @escaping (Tips.Action) -> Void = { _ in }) -> some View {
        self
    }
}
