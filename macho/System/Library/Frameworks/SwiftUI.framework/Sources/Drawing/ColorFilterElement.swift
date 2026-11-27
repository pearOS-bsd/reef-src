// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorFilterElement (Drawing).

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

class ColorFilterElement: SwiftUI.WrapperElement {
    var matrix: SwiftUI.ColorMatrix = _skeletonValue()
    override var affectsChildDrawing: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(_ a0: SwiftUI.Element, matrix a1: SwiftUI.ColorMatrix) { _skeleton() }
    deinit {}
}
