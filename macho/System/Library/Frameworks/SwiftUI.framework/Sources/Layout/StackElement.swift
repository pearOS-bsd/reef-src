// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StackElement (Layout).

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
    let axis: SwiftUI.Axis = _skeletonValue()
    let alignmentKey: SwiftUI.AlignmentKey = _skeletonValue()
    let spacing: CoreGraphics.CGFloat? = _skeletonValue()
    let items: [SwiftUI.Element] = _skeletonValue()
    var spacingBetween: ((SwiftUI.Element, SwiftUI.Element) -> CoreGraphics.CGFloat?)? = _skeletonValue()
    override var children: [SwiftUI.Element] { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    final func gaps() -> [CoreGraphics.CGFloat] { _skeleton() }
    final func childSizes(_ a0: SwiftUI.ProposedViewSize) -> [CGSize] { _skeleton() }
    final func crossPositions(_ a0: [CGSize]) -> (offsets: [CoreGraphics.CGFloat], extent: CoreGraphics.CGFloat) { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(axis a0: SwiftUI.Axis, alignment a1: SwiftUI.AlignmentKey, spacing a2: CoreGraphics.CGFloat?, items a3: [SwiftUI.Element]) { _skeleton() }
    deinit {}
}
