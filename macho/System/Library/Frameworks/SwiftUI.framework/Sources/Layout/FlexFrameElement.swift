// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FlexFrameElement: the element of frame(minWidth:idealWidth:maxWidth:...) (bounds on the size of its content).

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

class FlexFrameElement: SwiftUI.WrapperElement {
    let minWidth: CoreGraphics.CGFloat?
    let idealWidth: CoreGraphics.CGFloat?
    let maxWidth: CoreGraphics.CGFloat?
    let minHeight: CoreGraphics.CGFloat?
    let idealHeight: CoreGraphics.CGFloat?
    let maxHeight: CoreGraphics.CGFloat?
    let alignment: SwiftUI.Alignment
    override var debugDetail: Swift.String {
        func text(_ value: CoreGraphics.CGFloat?) -> String { value.map { Double($0).description } ?? "-" }
        return " width " + text(minWidth) + "/" + text(idealWidth) + "/" + text(maxWidth)
            + " height " + text(minHeight) + "/" + text(idealHeight) + "/" + text(maxHeight)
    }
    override func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool {
        (a0 == .horizontal ? idealWidth : idealHeight) == nil && content.hasFlexibleIdeal(a0)
    }
    /// a proposed dimension limited to the frame's bounds (max first, then min)
    private static func clamp(_ value: CoreGraphics.CGFloat, _ minimum: CoreGraphics.CGFloat?, _ maximum: CoreGraphics.CGFloat?) -> CoreGraphics.CGFloat {
        var v = value
        if let maximum = maximum { v = min(maximum, v) }
        if let minimum = minimum { v = max(v, minimum) }
        return v
    }
    /// the frame's length along one axis for a proposal and the content's length
    private static func length(_ proposed: CoreGraphics.CGFloat?, _ child: CoreGraphics.CGFloat, _ minimum: CoreGraphics.CGFloat?,
                               _ ideal: CoreGraphics.CGFloat?, _ maximum: CoreGraphics.CGFloat?) -> CoreGraphics.CGFloat {
        guard let proposed = proposed else {
            var v = ideal ?? child
            if let maximum = maximum { v = min(maximum, v) }
            if let minimum = minimum { v = max(v, minimum) }
            return v
        }
        switch (minimum, maximum) {
        case (nil, nil):
            return child
        case let (nil, maximum?):
            // as wide as proposed up to the maximum, never narrower than the content
            return max(child, min(maximum, proposed))
        case let (minimum?, nil):
            return max(minimum, min(max(proposed, minimum), child))
        case let (minimum?, maximum?):
            return max(minimum, min(maximum, proposed))
        }
    }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize {
        let proposal = SwiftUI.ProposedViewSize(width: a0.width.map { Self.clamp($0, minWidth, maxWidth) },
                                                height: a0.height.map { Self.clamp($0, minHeight, maxHeight) })
        let size = content.sizeThatFits(proposal)
        return CGSize(width: Self.length(a0.width, size.width, minWidth, idealWidth, maxWidth),
                      height: Self.length(a0.height, size.height, minHeight, idealHeight, maxHeight))
    }
    override func place(`in` a0: CGRect) {
        frame = a0
        let size = content.sizeThatFits(SwiftUI.ProposedViewSize(width: Self.clamp(a0.width, minWidth, maxWidth),
                                                                 height: Self.clamp(a0.height, minHeight, maxHeight)))
        let x = alignment.horizontal.key.defaultValue(width: a0.width, height: a0.height)
        let y = alignment.vertical.key.defaultValue(width: a0.width, height: a0.height)
        let contentX = content.alignment(alignment.horizontal.key, size: size)
        let contentY = content.alignment(alignment.vertical.key, size: size)
        content.place(in: CGRect(x: a0.minX + x - contentX, y: a0.minY + y - contentY, width: size.width, height: size.height))
    }
    override var spacingKind: SwiftUI.SpacingKind { content.spacingOverride ?? content.spacingKind }
    init(_ a0: SwiftUI.Element, layout a1: SwiftUI._FlexFrameLayout) {
        minWidth = a1.minWidth
        idealWidth = a1.idealWidth
        maxWidth = a1.maxWidth
        minHeight = a1.minHeight
        idealHeight = a1.idealHeight
        maxHeight = a1.maxHeight
        alignment = a1.alignment
        super.init(a0)
    }
    deinit {}
}
