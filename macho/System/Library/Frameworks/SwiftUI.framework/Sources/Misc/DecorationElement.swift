// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DecorationElement (Misc).

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

class DecorationElement: SwiftUI.Element {
    let content: SwiftUI.Element = _skeletonValue()
    let decoration: SwiftUI.Element = _skeletonValue()
    let alignment: SwiftUI.Alignment = _skeletonValue()
    let isOverlay: Swift.Bool = _skeletonValue()
    weak var liftedCanvas: SwiftUI.ElementCanvas?
    override var children: [SwiftUI.Element] { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    final var coveredViews: [NSView] { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool { _skeleton() }
    override var isSpacer: Swift.Bool { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    init(content a0: SwiftUI.Element, decoration a1: SwiftUI.Element, alignment a2: SwiftUI.Alignment, isOverlay a3: Swift.Bool) { _skeleton() }
    deinit {}
}

extension SwiftUI.DecorationElement: SwiftUI.LiftingElement {
    func dropLift() { _skeleton() }
    func lifts(_ a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    var liftedPart: SwiftUI.Element { _skeleton() }
}
