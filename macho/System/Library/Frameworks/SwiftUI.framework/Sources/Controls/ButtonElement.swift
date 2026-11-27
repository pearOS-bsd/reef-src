// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonElement (Controls).

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

class ButtonElement: SwiftUI.WrapperElement {
    let interaction: SwiftUI.ButtonInteraction = _skeletonValue()
    let kind: SwiftUI.BuiltinButtonKind? = _skeletonValue()
    let plate: ((CGContext, CGRect, Swift.Bool) -> ())? = _skeletonValue()
    let pressedOpacity: CoreGraphics.CGFloat = _skeletonValue()
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override var affectsChildDrawing: Swift.Bool { _skeleton() }
    override var drawOverflow: CoreGraphics.CGFloat { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(_ a0: SwiftUI.Element, interaction a1: SwiftUI.ButtonInteraction, kind a2: SwiftUI.BuiltinButtonKind?, plate a3: ((CGContext, CGRect, Swift.Bool) -> ())?, pressedOpacity a4: CoreGraphics.CGFloat) { _skeleton() }
    deinit {}
}

extension SwiftUI.ButtonElement: SwiftUI.PressableElement {
    var pressTarget: any SwiftUI.PressTarget { _skeleton() }
    var focusOnClick: Swift.Bool { _skeleton() }
}
