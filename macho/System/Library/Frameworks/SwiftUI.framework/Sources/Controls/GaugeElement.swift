// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GaugeElement (Controls).

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

class GaugeElement: SwiftUI.Element {
    let kind: SwiftUI.GaugeElement.Kind = _skeletonValue()
    let fraction: Swift.Double = _skeletonValue()
    let label: SwiftUI.Element? = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    var gauge: CGRect = _skeletonValue()
    override var children: [SwiftUI.Element] { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    final func drawBar(_ a0: CGContext, _ a1: CoreGraphics.CGFloat, scale a2: CoreGraphics.CGFloat) { _skeleton() }
    final func drawRing(_ a0: CGContext, _ a1: CoreGraphics.CGFloat, capacity a2: Swift.Bool) { _skeleton() }
    init(kind a0: SwiftUI.GaugeElement.Kind, fraction a1: Swift.Double, label a2: SwiftUI.Element?, environment a3: SwiftUI.EnvironmentValues) { _skeleton() }
    deinit {}
    enum Kind {
        case linearCapacity
        case circular
        case circularCapacity
    }
}

extension SwiftUI.GaugeElement.Kind: Swift.Hashable {
}

extension SwiftUI.GaugeElement.Kind: Swift.Equatable {
}
