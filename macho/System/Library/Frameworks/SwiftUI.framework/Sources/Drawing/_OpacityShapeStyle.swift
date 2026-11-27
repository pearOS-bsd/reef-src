// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _OpacityShapeStyle (Drawing).

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

public struct _OpacityShapeStyle<A> where A: SwiftUI.ShapeStyle {
    public var style: A
    public var opacity: Swift.Float
    func paint(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
    public init(style a0: A, opacity a1: Swift.Float) { _skeleton() }
}

extension SwiftUI._OpacityShapeStyle: SwiftUI.ResolvableShapeStyle {
}

extension SwiftUI._OpacityShapeStyle: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}
