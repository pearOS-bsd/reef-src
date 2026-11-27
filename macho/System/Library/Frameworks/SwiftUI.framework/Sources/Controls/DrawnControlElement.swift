// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DrawnControlElement (Controls).

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

class DrawnControlElement: SwiftUI.Element {
    let sizing: (SwiftUI.ProposedViewSize) -> CGSize = _skeletonValue()
    let painter: (CGContext, CGRect, Swift.Bool) -> () = _skeletonValue()
    let interaction: SwiftUI.ButtonInteraction = _skeletonValue()
    let kind: SwiftUI.SpacingKind = _skeletonValue()
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override var drawOverflow: CoreGraphics.CGFloat { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    init(interaction a0: SwiftUI.ButtonInteraction, kind a1: SwiftUI.SpacingKind, sizing a2: @escaping (SwiftUI.ProposedViewSize) -> CGSize, painter a3: @escaping (CGContext, CGRect, Swift.Bool) -> ()) { _skeleton() }
    deinit {}
}

extension SwiftUI.DrawnControlElement: SwiftUI.PressableElement {
    var pressTarget: any SwiftUI.PressTarget { _skeleton() }
    var focusOnClick: Swift.Bool { _skeleton() }
}
