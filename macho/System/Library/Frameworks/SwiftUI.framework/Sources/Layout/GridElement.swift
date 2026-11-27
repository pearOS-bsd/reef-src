// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GridElement (Layout).

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

class GridElement: SwiftUI.Element {
    let layout: SwiftUI.GridLayout
    let rows: [SwiftUI.Element]
    override var children: [SwiftUI.Element] { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    final var rowGaps: [CoreGraphics.CGFloat] { _skeleton() }
    final func columnWidths(_ a0: CoreGraphics.CGFloat?) -> [CoreGraphics.CGFloat] { _skeleton() }
    final func widenForSpans(_ a0: inout [CoreGraphics.CGFloat]) { _skeleton() }
    final func widenForFullRows(_ a0: inout [CoreGraphics.CGFloat], width a1: CoreGraphics.CGFloat) { _skeleton() }
    final func spanWidth(_ a0: [CoreGraphics.CGFloat], column a1: Swift.Int, span a2: Swift.Int) -> CoreGraphics.CGFloat { _skeleton() }
    final func rowMetrics(_ a0: SwiftUI.Element, widths a1: [CoreGraphics.CGFloat], total a2: CoreGraphics.CGFloat, height a3: CoreGraphics.CGFloat?) -> (height: CoreGraphics.CGFloat, baseline: CoreGraphics.CGFloat?) { _skeleton() }
    final func rowLayout(widths a0: [CoreGraphics.CGFloat], total a1: CoreGraphics.CGFloat, height a2: CoreGraphics.CGFloat?) -> [(height: CoreGraphics.CGFloat, baseline: CoreGraphics.CGFloat?)] { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    init(layout a0: SwiftUI.GridLayout, rows a1: [SwiftUI.Element]) {
        layout = a0
        rows = a1
        super.init()
    }
    deinit {}
}
