// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyStackElement (Layout).

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

class LazyStackElement: SwiftUI.Element {
    let axis: SwiftUI.Axis = _skeletonValue()
    let alignment: SwiftUI.AlignmentKey = _skeletonValue()
    let spacing: CoreGraphics.CGFloat? = _skeletonValue()
    let pinned: SwiftUI.PinnedScrollableViews = _skeletonValue()
    let rows: [SwiftUI.LazyRow] = _skeletonValue()
    weak var host: SwiftUI.ViewHost?
    let scope: SwiftUI.BodyNode = _skeletonValue()
    let state: SwiftUI.LazyStackState = _skeletonValue()
    var built: [Swift.Int: [SwiftUI.Element]] = _skeletonValue()
    var stack: SwiftUI.StackElement = _skeletonValue()
    var targets: Swift.Set<Swift.Int> = _skeletonValue()
    var pinnedFrames: [Swift.Int: [CGRect]] = _skeletonValue()
    var restackedSinceRefresh: Swift.Bool = _skeletonValue()
    final func pinViews() { _skeleton() }
    final func needsRows() -> Swift.Bool { _skeleton() }
    final func restack() { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    override func liveSizesChanged() -> Swift.Bool { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    final func length(of a0: Swift.Int, across a1: CoreGraphics.CGFloat?) -> CoreGraphics.CGFloat { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    final func placeStack(`in` a0: CGRect) -> CGSize { _skeleton() }
    final func row(id a0: Swift.AnyHashable) -> SwiftUI.Element? { _skeleton() }
    final func extents(`in` a0: CGRect) -> [Swift.ClosedRange<CoreGraphics.CGFloat>] { _skeleton() }
    final func visibleBounds() -> (Swift.ClosedRange<CoreGraphics.CGFloat>)? { _skeleton() }
    final func firstElement() -> SwiftUI.Element? { _skeleton() }
    final func sectionExtents(_ a0: [Swift.ClosedRange<CoreGraphics.CGFloat>]) -> [[Swift.AnyHashable]: Swift.ClosedRange<CoreGraphics.CGFloat>] { _skeleton() }
    final var pinnedElements: [SwiftUI.Element] { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    init(axis a0: SwiftUI.Axis, alignment a1: SwiftUI.AlignmentKey, spacing a2: CoreGraphics.CGFloat?, pinned a3: SwiftUI.PinnedScrollableViews, rows a4: [SwiftUI.LazyRow], scope a5: SwiftUI.BodyNode, state a6: SwiftUI.LazyStackState, host a7: SwiftUI.ViewHost) { _skeleton() }
    deinit {}
}
