// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ImageElement (Images).

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

class ImageElement: SwiftUI.Element {
    let image: NSImage? = _skeletonValue()
    let naturalSize: CGSize = _skeletonValue()
    let resizable: Swift.Bool = _skeletonValue()
    let tint: SwiftUI.Paint? = _skeletonValue()
    let isSymbol: Swift.Bool = _skeletonValue()
    var capHeight: CoreGraphics.CGFloat = _skeletonValue()
    var symbolSize: CGSize? = _skeletonValue()
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    init(image a0: NSImage?, naturalSize a1: CGSize, resizable a2: Swift.Bool, tint a3: SwiftUI.Paint?, isSymbol a4: Swift.Bool) { _skeleton() }
    deinit {}
}
