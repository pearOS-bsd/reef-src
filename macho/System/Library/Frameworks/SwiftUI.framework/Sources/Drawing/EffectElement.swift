// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EffectElement (Drawing).

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

final class EffectElement: SwiftUI.WrapperElement {
    var opacity: Swift.Double = _skeletonValue()
    var transform: CGAffineTransform? = _skeletonValue()
    var anchor: SwiftUI.UnitPoint = _skeletonValue()
    var clip: ((CGRect) -> CGPath)? = _skeletonValue()
    var shadow: ((color: SwiftUI.Color.Resolved, radius: CoreGraphics.CGFloat, offset: CGSize))? = _skeletonValue()
    var group: Swift.Bool = _skeletonValue()
    var blendMode: CGBlendMode = _skeletonValue()
    override var affectsChildDrawing: Swift.Bool { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var debugDetail: Swift.String { _skeleton() }
    override var drawOverflow: CoreGraphics.CGFloat { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    func drawOverMaterial(_ a0: SwiftUI.RenderContext, _ a1: SwiftUI.MaterialRegions.Backdrop) { _skeleton() }
    func castShadow(of a0: [CGPath], _ a1: CGContext) { _skeleton() }
    func drawOutsideMaterial(_ a0: SwiftUI.RenderContext, _ a1: SwiftUI.MaterialRegions.Backdrop) { _skeleton() }
    override func takesEvents(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    deinit {}
}
