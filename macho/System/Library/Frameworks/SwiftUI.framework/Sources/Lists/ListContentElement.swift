// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListContentElement (Lists).

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

class ListContentElement: SwiftUI.Element {
    let kind: SwiftUI.ListKind = _skeletonValue()
    let items: [SwiftUI.ListContentElement.Item] = _skeletonValue()
    let selection: SwiftUI.ListSelection? = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    let alternates: Swift.Bool = _skeletonValue()
    var rowFrames: [(SwiftUI.Element, CGRect)] = _skeletonValue()
    var dragState: SwiftUI.ListDragState? = _skeletonValue()
    var stripesInset: Swift.Bool = _skeletonValue()
    var fillsRows: Swift.Bool = _skeletonValue()
    var hasHeaderBand: Swift.Bool = _skeletonValue()
    final var rows: [SwiftUI.Element] { _skeleton() }
    final var hasDraggableRows: Swift.Bool { _skeleton() }
    final var insertionTypes: [UniformTypeIdentifiers.UTType] { _skeleton() }
    final func rowIndex(at a0: CGPoint) -> Swift.Int? { _skeleton() }
    final func insertionIndex(at a0: CoreGraphics.CGFloat) -> Swift.Int { _skeleton() }
    final func draggedRows(from a0: Swift.Int) -> [Swift.Int] { _skeleton() }
    final func beginDrag(from a0: Swift.Int, event a1: NSEvent) -> Swift.Bool { _skeleton() }
    final func dropAction(_ a0: SwiftUI.ListDragSession?, items a1: [SwiftUI.DropItem], at a2: Swift.Int) -> ((operation: SwiftUI.DropOperation, perform: () -> ()))? { _skeleton() }
    final func drawDropIndicator(_ a0: CGContext) { _skeleton() }
    init(kind a0: SwiftUI.ListKind, elements a1: [SwiftUI.Element], selection a2: SwiftUI.ListSelection?, environment a3: SwiftUI.EnvironmentValues, sectionGaps a4: Swift.Bool) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    final func layout(width a0: CoreGraphics.CGFloat?, place a1: Swift.Bool) -> CGSize { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    final func select(at a0: CGPoint, extending a1: Swift.Bool) { _skeleton() }
    deinit {}
    enum Item {
        case header(SwiftUI.Element)
        case row(SwiftUI.Element)
        case gap(CoreGraphics.CGFloat)
    }
}
