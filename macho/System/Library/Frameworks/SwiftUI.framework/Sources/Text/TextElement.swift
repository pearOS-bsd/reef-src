// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextElement (Text).

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

class TextElement: SwiftUI.Element {
    let string: NSAttributedString = _skeletonValue()
    let style: SwiftUI.TextRunStyle = _skeletonValue()
    let lineLimit: Swift.Int? = _skeletonValue()
    let minimumLines: Swift.Int? = _skeletonValue()
    let lineSpacing: CoreGraphics.CGFloat = _skeletonValue()
    let scale: CoreGraphics.CGFloat = _skeletonValue()
    let minimumScaleFactor: CoreGraphics.CGFloat = _skeletonValue()
    lazy var idealWidth: CoreGraphics.CGFloat = _skeletonValue()
    lazy var truncatedWidth: CoreGraphics.CGFloat = _skeletonValue()
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(text a0: SwiftUI.Text, environment a1: SwiftUI.EnvironmentValues) { _skeleton() }
    override var hasTextBaseline: Swift.Bool { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    final func drawing() -> (rect: CGRect, options: NSString.DrawingOptions, clips: Swift.Bool) { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    deinit {}
}
