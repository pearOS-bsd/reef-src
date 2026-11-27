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
    override var spacingKind: SwiftUI.SpacingKind {
        // one item: its kind; a row: the kind of its first item that is not a Spacer, when that is a control
        if items.count == 1 { return items[0].spacingOverride ?? items[0].spacingKind }
        guard axis == .horizontal else { return .other }
        for item in items where !item.isSpacer {
            let kind = item.spacingOverride ?? item.spacingKind
            switch kind {
            case .control, .checkbox, .toggleSwitch: return kind
            default: return .other
            }
        }
        return .other
    }
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
    /// the size of every item for a proposal: ideal sizes when they fit, else the space is shared out by layout priority
    /// (highest first; inside a priority, the least flexible items first), Spacers last
    final func childSizes(_ a0: SwiftUI.ProposedViewSize) -> [CGSize] {
        let horizontal = axis == .horizontal
        let mainProposal = horizontal ? a0.width : a0.height
        let crossProposal = horizontal ? a0.height : a0.width
        func proposal(_ main: CoreGraphics.CGFloat?) -> SwiftUI.ProposedViewSize {
            horizontal ? SwiftUI.ProposedViewSize(width: main, height: crossProposal)
                       : SwiftUI.ProposedViewSize(width: crossProposal, height: main)
        }
        func mainLength(_ size: CGSize) -> CoreGraphics.CGFloat { horizontal ? size.width : size.height }
        // the sizes with the main axis left to the items
        let ideals = items.map { $0.sizeThatFits(proposal(nil)) }
        guard let available = mainProposal else { return ideals }
        let spaces = gaps().reduce(0, +)
        let idealTotal = ideals.reduce(0) { $0 + mainLength($1) }
        if abs(idealTotal + spaces - available) < 0.5 { return ideals }
        var remaining = available - spaces
        var sizes = [CGSize](repeating: .zero, count: items.count)
        let minimums = items.map { mainLength($0.sizeThatFits(proposal(0))) }
        let maximums = items.map { mainLength($0.sizeThatFits(proposal(.infinity))) }
        var spacers: [Int] = []
        for (i, item) in items.enumerated() where type(of: item) == SwiftUI.SpacerElement.self { spacers.append(i) }
        var others: [Int] = []
        for (i, item) in items.enumerated() where type(of: item) != SwiftUI.SpacerElement.self { others.append(i) }
        let spacerMinimum = spacers.reduce(0) { $0 + minimums[$1] }
        let priorities = Set(others.map { items[$0].layoutPriority }).sorted(by: >)
        for priority in priorities {
            var group = others.filter { items[$0].layoutPriority == priority }
            // what the items of lower priority and the Spacers need at least
            let lower = others.filter { items[$0].layoutPriority < priority }
            let reserved = lower.reduce(spacerMinimum) { $0 + minimums[$1] }
            group.sort { (maximums[$0] - minimums[$0]) < (maximums[$1] - minimums[$1]) }
            var left = remaining - reserved
            var count = CoreGraphics.CGFloat(group.count)
            var used: CoreGraphics.CGFloat = 0
            for i in group {
                let share = max(left / count, 0)
                let size = items[i].sizeThatFits(proposal(share))
                sizes[i] = size
                left -= mainLength(size)
                used += mainLength(size)
                count -= 1
            }
            remaining -= used
        }
        var count = CoreGraphics.CGFloat(spacers.count)
        for i in spacers {
            let share = max(remaining / count, 0)
            let size = items[i].sizeThatFits(proposal(share))
            sizes[i] = size
            remaining -= mainLength(size)
            count -= 1
        }
        return sizes
    }
    /// where each item sits across the axis so that their alignment guides line up, and the extent they cover
    final func crossPositions(_ a0: [CGSize]) -> (offsets: [CoreGraphics.CGFloat], extent: CoreGraphics.CGFloat) {
        var guides: [CoreGraphics.CGFloat] = []
        var low = CoreGraphics.CGFloat.infinity
        var high = -CoreGraphics.CGFloat.infinity
        for (item, size) in zip(items, a0) {
            let guide = item.alignment(alignmentKey, size: size)
            let length = axis == .horizontal ? size.height : size.width
            guides.append(guide)
            low = min(low, -guide)
            high = max(high, length - guide)
        }
        if guides.isEmpty { return ([], 0) }
        return (guides.map { -$0 - low }, high - low)
    }
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
    override func place(`in` a0: CGRect) {
        frame = a0
        let sizes = childSizes(SwiftUI.ProposedViewSize(width: a0.width, height: a0.height))
        let cross = crossPositions(sizes)
        let spaces = gaps()
        var main: CoreGraphics.CGFloat
        let crossLength: CoreGraphics.CGFloat
        var crossStart: CoreGraphics.CGFloat
        if axis == .horizontal {
            main = a0.minX; crossLength = a0.height; crossStart = a0.minY
        } else {
            main = a0.minY; crossLength = a0.width; crossStart = a0.minX
        }
        // the alignment's fraction of the cross length (leading 0, center 0.5, trailing 1)
        let value = alignmentKey.defaultValue(width: 1, height: 1)
        let fraction: CoreGraphics.CGFloat = value.isFinite ? (value > 0 ? min(value, 1) : 0) : 0.5
        crossStart += (crossLength - cross.extent) * fraction
        for (i, item) in items.enumerated() {
            let size = sizes[i]
            let length: CoreGraphics.CGFloat
            if axis == .horizontal {
                item.place(in: CGRect(x: main, y: cross.offsets[i] + crossStart, width: size.width, height: size.height))
                length = size.width
            } else {
                item.place(in: CGRect(x: cross.offsets[i] + crossStart, y: main, width: size.width, height: size.height))
                length = size.height
            }
            main += length + (i < spaces.count ? spaces[i] : 0)
        }
    }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) {
        // in zIndex order (the same order as ZStackElement's children)
        for item in items.sorted(by: { $0.zIndex < $1.zIndex }) where !item.isHidden { item.draw(a0) }
    }
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
