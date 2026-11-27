// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ShapeView (Drawing).

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

public struct _ShapeView<A, B> where A: SwiftUI.Shape, B: SwiftUI.ShapeStyle {
    public var shape: A
    public var style: B
    public var fillStyle: SwiftUI.FillStyle
    public init(shape a0: A, style a1: B) { _skeleton() }
    public init(shape a0: A, style a1: B, fillStyle a2: SwiftUI.FillStyle) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._ShapeView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI._ShapeView: SwiftUI.PrimitiveView {
}

extension SwiftUI._ShapeView: SwiftUI.ShapeView {
    public typealias Content = A
}
