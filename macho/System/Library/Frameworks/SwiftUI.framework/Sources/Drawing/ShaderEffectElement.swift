// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShaderEffectElement (Drawing).

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

class ShaderEffectElement: SwiftUI.WrapperElement {
    let shader: SwiftUI.Shader = _skeletonValue()
    let effect: Swift.Int = _skeletonValue()
    let maxSampleOffset: CGSize = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    override var affectsChildDrawing: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(_ a0: SwiftUI.Element, shader a1: SwiftUI.Shader, effect a2: Swift.Int, maxSampleOffset a3: CGSize, environment a4: SwiftUI.EnvironmentValues) { _skeleton() }
    static var reported: Swift.Set<Swift.String> { _skeleton() }
    deinit {}
}
