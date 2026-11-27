// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HierarchicalShapeStyle (Drawing).

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

public struct HierarchicalShapeStyle {
    public var id: Swift.UInt32
    public init(id a0: Swift.UInt32) { _skeleton() }
    public static var primary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var secondary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var tertiary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var quaternary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
    public static var quinary: SwiftUI.HierarchicalShapeStyle { _skeleton() }
}

extension SwiftUI.HierarchicalShapeStyle: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}
