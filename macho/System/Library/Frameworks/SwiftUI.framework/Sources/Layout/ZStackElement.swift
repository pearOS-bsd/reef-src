// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ZStackElement (Layout).

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
    let alignment: (h: SwiftUI.AlignmentKey, v: SwiftUI.AlignmentKey) = _skeletonValue()
    let items: [SwiftUI.Element] = _skeletonValue()
    weak var liftedCanvas: SwiftUI.ElementCanvas?
    var liftedFrom: Swift.Int = _skeletonValue()
    let front: SwiftUI.FrontElements = _skeletonValue()
    let tail: SwiftUI.StackTail = _skeletonValue()
    init(alignment a0: SwiftUI.Alignment, items a1: [SwiftUI.Element]) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    final func layout(_ a0: SwiftUI.ProposedViewSize) -> (sizes: [CGSize], offsets: [CGPoint], size: CGSize) { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    final func lifts(_ a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    final var coveredViews: [NSView] { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    deinit {}
}

extension SwiftUI.ZStackElement: SwiftUI.LiftingElement {
    func dropLift() { _skeleton() }
    var liftedPart: SwiftUI.Element { _skeleton() }
}
