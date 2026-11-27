// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _InsettableBackgroundShapeModifier (Layout).

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

public struct _InsettableBackgroundShapeModifier<A, B> where A: SwiftUI.ShapeStyle, B: SwiftUI.InsettableShape {
    public var style: A
    public var shape: B
    public var fillStyle: SwiftUI.FillStyle
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(style a0: A, shape a1: B, fillStyle a2: SwiftUI.FillStyle) { _skeleton() }
}

extension SwiftUI._InsettableBackgroundShapeModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._InsettableBackgroundShapeModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
