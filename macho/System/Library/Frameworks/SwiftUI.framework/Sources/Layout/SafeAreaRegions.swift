// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SafeAreaRegions (Layout).

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

public struct SafeAreaRegions {
    public let rawValue: Swift.UInt
    public static var all: SwiftUI.SafeAreaRegions { _skeleton() }
    public init(rawValue a0: Swift.UInt) { rawValue = a0 }
    public static var container: SwiftUI.SafeAreaRegions { SwiftUI.SafeAreaRegions(rawValue: 1) }
    public static var keyboard: SwiftUI.SafeAreaRegions { SwiftUI.SafeAreaRegions(rawValue: 2) }
}

extension SwiftUI.SafeAreaRegions: Swift.OptionSet {
    public typealias Element = SwiftUI.SafeAreaRegions
}

extension SwiftUI.SafeAreaRegions: Swift.SetAlgebra {
}

extension SwiftUI.SafeAreaRegions: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt
}

extension SwiftUI.SafeAreaRegions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.SafeAreaRegions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.SafeAreaRegions
}
