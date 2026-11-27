// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FrameElement: the element of frame(width:height:alignment:) (a fixed size, the content aligned in it).

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

class FrameElement: SwiftUI.WrapperElement {
    let width: CoreGraphics.CGFloat?
    let height: CoreGraphics.CGFloat?
    let alignment: SwiftUI.Alignment
    override var spacingKind: SwiftUI.SpacingKind {
        if (width == nil && height == nil) || content.isSpacer { return content.spacingOverride ?? content.spacingKind }
        return .other
    }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize {
        if let width = width, let height = height { return CGSize(width: width, height: height) }
        let size = content.sizeThatFits(SwiftUI.ProposedViewSize(width: width ?? a0.width, height: height ?? a0.height))
        return CGSize(width: width ?? size.width, height: height ?? size.height)
    }
    /// where the content goes in a rectangle: its alignment guides on the frame's
    private func contentFrame(in rect: CGRect) -> CGRect {
        let size = content.sizeThatFits(SwiftUI.ProposedViewSize(width: rect.width, height: rect.height))
        let x = alignment.horizontal.key.defaultValue(width: rect.width, height: rect.height)
        let y = alignment.vertical.key.defaultValue(width: rect.width, height: rect.height)
        let contentX = content.alignment(alignment.horizontal.key, size: size)
        let contentY = content.alignment(alignment.vertical.key, size: size)
        return CGRect(x: rect.minX + x - contentX, y: rect.minY + y - contentY, width: size.width, height: size.height)
    }
    override func place(`in` a0: CGRect) {
        frame = a0
        content.place(in: contentFrame(in: a0))
    }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? {
        // text baselines: the content's, where the content sits in the frame
        guard a0 == SwiftUI.VerticalAlignment.firstTextBaseline.key || a0 == SwiftUI.VerticalAlignment.lastTextBaseline.key else { return nil }
        let inner = contentFrame(in: CGRect(origin: .zero, size: a1))
        return inner.minY + content.alignment(a0, size: inner.size)
    }
    override func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool {
        // a fixed dimension is not flexible
        (a0 == .horizontal ? width : height) == nil && content.hasFlexibleIdeal(a0)
    }
    init(_ a0: SwiftUI.Element, width a1: CoreGraphics.CGFloat?, height a2: CoreGraphics.CGFloat?, alignment a3: SwiftUI.Alignment) {
        width = a1
        height = a2
        alignment = a3
        super.init(a0)
    }
    deinit {}
}
