// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ShadowShapeStyle (Drawing).

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

public struct _ShadowShapeStyle<A> where A: SwiftUI.ShapeStyle {
    public var style: A
    public var shadowStyle: SwiftUI.ShadowStyle
    public init(style a0: A, shadowStyle a1: SwiftUI.ShadowStyle) { _skeleton() }
    func paint(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
}

extension SwiftUI._ShadowShapeStyle: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}

extension SwiftUI._ShadowShapeStyle: SwiftUI.ResolvableShapeStyle {
}
