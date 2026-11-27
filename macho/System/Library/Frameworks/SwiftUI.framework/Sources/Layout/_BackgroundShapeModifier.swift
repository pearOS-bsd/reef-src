// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _BackgroundShapeModifier (Layout).

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

public struct _BackgroundShapeModifier<A, B> where A: SwiftUI.ShapeStyle, B: SwiftUI.Shape {
    public var style: A
    public var shape: B
    public var fillStyle: SwiftUI.FillStyle
    public init(style a0: A, shape a1: B, fillStyle a2: SwiftUI.FillStyle) { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._BackgroundShapeModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._BackgroundShapeModifier: SwiftUI.PrimitiveModifier {
}
