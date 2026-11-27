// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WrapperElement: an element around one content element (modifiers); passes layout through to its content.

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

class WrapperElement: SwiftUI.Element {
    let content: SwiftUI.Element
    override var children: [SwiftUI.Element] { [content] }
    override func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool {
        flexibleIdealAxes.contains(a0 == .horizontal ? .horizontal : .vertical) || content.hasFlexibleIdeal(a0)
    }
    override var isSpacer: Swift.Bool { content.isSpacer }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { content.sizeThatFits(a0) }
    override func place(`in` a0: CGRect) {
        frame = a0
        content.place(in: a0)
    }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? {
        // the content's own guide when it has one
        guard content.alignmentGuides[a0] != nil || content.explicitAlignment(a0, size: a1) != nil else { return nil }
        return content.alignment(a0, size: a1)
    }
    override var spacingKind: SwiftUI.SpacingKind { content.spacingOverride ?? content.spacingKind }
    init(_ a0: SwiftUI.Element) {
        content = a0
        super.init()
    }
    deinit {}
}
