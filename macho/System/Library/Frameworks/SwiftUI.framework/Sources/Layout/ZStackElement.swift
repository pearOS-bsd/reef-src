// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ZStackElement: the element of ZStack (items on top of each other, aligned on one guide; platform views lifted above).

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

class ZStackElement: SwiftUI.Element {
    let alignment: (h: SwiftUI.AlignmentKey, v: SwiftUI.AlignmentKey)
    let items: [SwiftUI.Element]
    weak var liftedCanvas: SwiftUI.ElementCanvas?
    var liftedFrom: Swift.Int = 0
    let front: SwiftUI.FrontElements = SwiftUI.FrontElements()
    let tail: SwiftUI.StackTail = SwiftUI.StackTail()
    init(alignment a0: SwiftUI.Alignment, items a1: [SwiftUI.Element]) {
        alignment = (a0.horizontal.key, a0.vertical.key)
        items = a1
        super.init()
    }
    /// back to front: by zIndex, in declaration order among equals
    override var children: [SwiftUI.Element] { items.sorted { $0.zIndex < $1.zIndex } }
    /// every item at its size for the proposal, their alignment guides on one point; the box they cover
    final func layout(_ a0: SwiftUI.ProposedViewSize) -> (sizes: [CGSize], offsets: [CGPoint], size: CGSize) {
        let sizes = items.map { $0.sizeThatFits(a0) }
        var guides: [CGPoint] = []
        var low = CGPoint(x: CoreGraphics.CGFloat.infinity, y: .infinity)
        var high = CGPoint(x: -CoreGraphics.CGFloat.infinity, y: -.infinity)
        for (item, size) in zip(items, sizes) {
            let x = item.alignment(alignment.h, size: size)
            let y = item.alignment(alignment.v, size: size)
            guides.append(CGPoint(x: x, y: y))
            low = CGPoint(x: min(low.x, -x), y: min(low.y, -y))
            high = CGPoint(x: max(high.x, size.width - x), y: max(high.y, size.height - y))
        }
        if items.isEmpty { return (sizes, [], .zero) }
        let offsets = guides.map { CGPoint(x: -$0.x - low.x, y: -$0.y - low.y) }
        return (sizes, offsets, CGSize(width: high.x - low.x, height: high.y - low.y))
    }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { layout(a0).size }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func place(`in` a0: CGRect) {
        frame = a0
        let result = layout(SwiftUI.ProposedViewSize(width: a0.width, height: a0.height))
        // the box of the items goes where the alignment's fraction of the free space says (center: half of it)
        func fraction(_ key: SwiftUI.AlignmentKey) -> CoreGraphics.CGFloat {
            let value = key.defaultValue(width: 1, height: 1)
            guard value.isFinite else { return 0.5 }
            return value > 0 ? min(value, 1) : 0
        }
        let x = a0.minX + (a0.width - result.size.width) * fraction(alignment.h)
        let y = a0.minY + (a0.height - result.size.height) * fraction(alignment.v)
        for (i, item) in items.enumerated() {
            let offset = result.offsets[i]
            item.place(in: CGRect(x: x + offset.x, y: y + offset.y, width: result.sizes[i].width, height: result.sizes[i].height))
        }
        // Reef then lifts the items above the first platform view onto their own canvas (liftedCanvas, front, tail,
        // liftedFrom); not written yet
        liftedCanvas = nil
        front.items = []
        tail.liftedCanvas = nil
        tail.part.items = []
    }
    final func lifts(_ a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    final var coveredViews: [NSView] { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    deinit {}
}

extension SwiftUI.ZStackElement: SwiftUI.LiftingElement {
    func dropLift() { _skeleton() }
    var liftedPart: SwiftUI.Element { _skeleton() }
}
