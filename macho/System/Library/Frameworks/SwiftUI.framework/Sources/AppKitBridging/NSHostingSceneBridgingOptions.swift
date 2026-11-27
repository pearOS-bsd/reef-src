// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSHostingSceneBridgingOptions (AppKitBridging).

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

public struct NSHostingSceneBridgingOptions {
    public let rawValue: Swift.Int
    public static var all: SwiftUI.NSHostingSceneBridgingOptions { _skeleton() }
    public init(rawValue a0: Swift.Int) { _skeleton() }
    public static var title: SwiftUI.NSHostingSceneBridgingOptions { _skeleton() }
    public static var toolbars: SwiftUI.NSHostingSceneBridgingOptions { _skeleton() }
}

extension SwiftUI.NSHostingSceneBridgingOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.NSHostingSceneBridgingOptions
}

extension SwiftUI.NSHostingSceneBridgingOptions: Swift.SetAlgebra {
}

extension SwiftUI.NSHostingSceneBridgingOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.NSHostingSceneBridgingOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.NSHostingSceneBridgingOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.NSHostingSceneBridgingOptions
}
