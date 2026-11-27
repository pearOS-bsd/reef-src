// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShapeElement (Drawing).

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

class ShapeElement: SwiftUI.Element {
    var label: Swift.String = _skeletonValue()
    let path: (CGRect) -> CGPath = _skeletonValue()
    let sizing: (SwiftUI.ProposedViewSize) -> CGSize = _skeletonValue()
    let paint: SwiftUI.Paint = _skeletonValue()
    let stroke: SwiftUI.StrokeStyle? = _skeletonValue()
    let eoFill: Swift.Bool = _skeletonValue()
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override var drawOverflow: CoreGraphics.CGFloat { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(path a0: @escaping (CGRect) -> CGPath, sizing a1: @escaping (SwiftUI.ProposedViewSize) -> CGSize, paint a2: SwiftUI.Paint, stroke a3: SwiftUI.StrokeStyle?, eoFill a4: Swift.Bool) { _skeleton() }
    deinit {}
}
