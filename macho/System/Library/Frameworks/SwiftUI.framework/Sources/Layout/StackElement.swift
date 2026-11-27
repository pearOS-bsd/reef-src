// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StackElement: the element of HStack and VStack (items along an axis, the spacing between them, alignment across).

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

class StackElement: SwiftUI.Element {
    let axis: SwiftUI.Axis
    let alignmentKey: SwiftUI.AlignmentKey
    let spacing: CoreGraphics.CGFloat?
    let items: [SwiftUI.Element]
    var spacingBetween: ((SwiftUI.Element, SwiftUI.Element) -> CoreGraphics.CGFloat?)? = nil
    override var children: [SwiftUI.Element] { items }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    /// the space between each pair of neighbouring items
    final func gaps() -> [CoreGraphics.CGFloat] {
        guard items.count >= 2 else { return [] }
        var result: [CoreGraphics.CGFloat] = []
        result.reserveCapacity(items.count - 1)
        for i in 0..<(items.count - 1) {
            result.append(gap(items[i], items[i + 1]))
        }
        return result
    }
    private func gap(_ a: SwiftUI.Element, _ b: SwiftUI.Element) -> CoreGraphics.CGFloat {
        if let spacing = spacing { return spacing }
        if let between = spacingBetween, let value = between(a, b) { return value }
        let first = a.spacingOverride ?? a.spacingKind
        let second = b.spacingOverride ?? b.spacingKind
        if case .spacer = first { return 0 }
        if case .spacer = second { return 0 }
        // the kind of the neighbours matters in vertical stacks only
        guard axis == .vertical else { return 8 }
        switch (first, second) {
        case let (.text(x), .text(y)):
            return x == 13 && y == 13 ? 0 : 1.5
        case (.text, .image):
            return 0
        case let (.text(x), _):
            return x * 0.151 / 13 + 8
        case (.image, .text), (.image, .image):
            return 0
        case let (_, .text(y)):
            return y * 4.742 / 13
        default:
            return kindGap(first, second)
        }
    }
    private func kindGap(_ a: SwiftUI.SpacingKind, _ b: SwiftUI.SpacingKind) -> CoreGraphics.CGFloat {
        switch b {
        case .control, .toggleSwitch:
            if case .checkbox = a { return 6 }
            return 8
        case .checkbox:
            switch a {
            case .control, .checkbox, .toggleSwitch: return 6
            default: return 8
            }
        default:
            return 8
        }
    }
    final func childSizes(_ a0: SwiftUI.ProposedViewSize) -> [CGSize] { _skeleton() }
    final func crossPositions(_ a0: [CGSize]) -> (offsets: [CoreGraphics.CGFloat], extent: CoreGraphics.CGFloat) { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize {
        let sizes = childSizes(a0)
        var main: CoreGraphics.CGFloat = 0
        for size in sizes { main += axis == .horizontal ? size.width : size.height }
        var spaces: CoreGraphics.CGFloat = 0
        for gap in gaps() { spaces += gap }
        main += spaces
        let cross = crossPositions(sizes).extent
        return axis == .horizontal ? CGSize(width: main, height: cross) : CGSize(width: cross, height: main)
    }
    override func place(`in` a0: CGRect) { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(axis a0: SwiftUI.Axis, alignment a1: SwiftUI.AlignmentKey, spacing a2: CoreGraphics.CGFloat?, items a3: [SwiftUI.Element]) {
        axis = a0
        alignmentKey = a1
        spacing = a2
        items = a3
        super.init()
        // a Spacer stretches along the axis of the stack it is in
        for item in a3 where item.isSpacer {
            if type(of: item) == SwiftUI.SpacerElement.self, let spacer = item as? SwiftUI.SpacerElement { spacer.axis = a0 }
        }
    }
    deinit {}
}
