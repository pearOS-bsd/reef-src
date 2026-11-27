// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CanvasElement (Drawing).

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

class CanvasElement: SwiftUI.Element {
    let renderer: (inout SwiftUI.GraphicsContext, CGSize) -> () = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(renderer a0: @escaping (inout SwiftUI.GraphicsContext, CGSize) -> (), environment a1: SwiftUI.EnvironmentValues) { _skeleton() }
    deinit {}
}
