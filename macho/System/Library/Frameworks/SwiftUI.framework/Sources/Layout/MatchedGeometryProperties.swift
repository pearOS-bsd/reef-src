// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MatchedGeometryProperties (Layout).

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

public struct MatchedGeometryProperties {
    public let rawValue: Swift.UInt32
    public init(rawValue a0: Swift.UInt32) { _skeleton() }
    public static var position: SwiftUI.MatchedGeometryProperties { _skeleton() }
    public static var size: SwiftUI.MatchedGeometryProperties { _skeleton() }
    public static var frame: SwiftUI.MatchedGeometryProperties { _skeleton() }
}

extension SwiftUI.MatchedGeometryProperties: Swift.OptionSet {
    public typealias Element = SwiftUI.MatchedGeometryProperties
}

extension SwiftUI.MatchedGeometryProperties: Swift.SetAlgebra {
}

extension SwiftUI.MatchedGeometryProperties: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt32
}

extension SwiftUI.MatchedGeometryProperties: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.MatchedGeometryProperties: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.MatchedGeometryProperties
}
