// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HierarchicalShapeStyleModifier (Drawing).

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

public struct HierarchicalShapeStyleModifier<A> where A: SwiftUI.ShapeStyle {
    public var base: A
    public var level: Swift.Int
    func paint(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
    public init(base a0: A, level a1: Swift.Int) { _skeleton() }
}

extension SwiftUI.HierarchicalShapeStyleModifier: SwiftUI.ResolvableShapeStyle {
}

extension SwiftUI.HierarchicalShapeStyleModifier: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}
