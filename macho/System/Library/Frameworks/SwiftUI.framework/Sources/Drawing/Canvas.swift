// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Canvas (Drawing).

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

public struct Canvas<A> where A: SwiftUI.View {
    let renderer: (inout SwiftUI.GraphicsContext, CGSize) -> ()
    let symbols: A
    public init(opaque a0: Swift.Bool, colorMode a1: SwiftUI.ColorRenderingMode, rendersAsynchronously a2: Swift.Bool, renderer a3: @escaping (inout SwiftUI.GraphicsContext, CGSize) -> (), symbols a4: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Canvas where A == SwiftUI.EmptyView {
    public init(opaque a0: Swift.Bool, colorMode a1: SwiftUI.ColorRenderingMode, rendersAsynchronously a2: Swift.Bool, renderer a3: @escaping (inout SwiftUI.GraphicsContext, CGSize) -> ()) { _skeleton() }
}

extension SwiftUI.Canvas: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Canvas: SwiftUI.PrimitiveView {
}
