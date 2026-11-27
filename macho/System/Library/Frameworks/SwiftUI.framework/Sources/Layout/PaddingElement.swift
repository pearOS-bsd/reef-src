// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PaddingElement: the element of padding(_:) (insets around its content).

import Foundation
import AppKit
import CoreGraphics
import CoreText
import Combine
import Observation
import CoreTransferable
import UniformTypeIdentifiers
import Symbols
import DeveloperToolsSupport
import Metal
import os

class PaddingElement: SwiftUI.WrapperElement {
    let insets: SwiftUI.EdgeInsets
    override var spacingKind: SwiftUI.SpacingKind {
        // vertical padding hides the kind of the content from its stack neighbours
        guard insets.top == 0, insets.bottom == 0 else { return .other }
        return content.spacingOverride ?? content.spacingKind
    }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize {
        let horizontal = insets.leading + insets.trailing
        let vertical = insets.top + insets.bottom
        let size = content.sizeThatFits(SwiftUI.ProposedViewSize(width: a0.width.map { max($0 - horizontal, 0) },
                                                                 height: a0.height.map { max($0 - vertical, 0) }))
        return CGSize(width: size.width + horizontal, height: size.height + vertical)
    }
    override func place(`in` a0: CGRect) {
        frame = a0
        content.place(in: CGRect(x: a0.minX + insets.leading, y: a0.minY + insets.top,
                                 width: max(a0.width - insets.leading - insets.trailing, 0),
                                 height: max(a0.height - insets.top - insets.bottom, 0)))
    }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? {
        // text baselines pass through, moved down by the top inset
        guard a0 == SwiftUI.VerticalAlignment.firstTextBaseline.key || a0 == SwiftUI.VerticalAlignment.lastTextBaseline.key else { return nil }
        let inner = CGSize(width: a1.width - insets.leading - insets.trailing, height: a1.height - insets.top - insets.bottom)
        return content.alignment(a0, size: inner) + insets.top
    }
    init(_ a0: SwiftUI.Element, insets a1: SwiftUI.EdgeInsets) {
        insets = a1
        super.init(a0)
    }
    deinit {}
}
