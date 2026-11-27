// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProjectionElement (Misc).

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

class ProjectionElement: SwiftUI.WrapperElement {
    let transform: (CGSize) -> CGAffineTransform = _skeletonValue()
    let anchor: () -> SwiftUI.UnitPoint = _skeletonValue()
    override var affectsChildDrawing: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(_ a0: SwiftUI.Element, transform a1: @escaping (CGSize) -> CGAffineTransform, anchor a2: @escaping () -> SwiftUI.UnitPoint) { _skeleton() }
    deinit {}
}
