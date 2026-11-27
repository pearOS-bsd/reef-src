// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSHostingSizingOptions (AppKitBridging).

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

public struct NSHostingSizingOptions {
    public let rawValue: Swift.Int
    public static var standardBounds: SwiftUI.NSHostingSizingOptions { _skeleton() }
    public init(rawValue a0: Swift.Int) { _skeleton() }
    public static var minSize: SwiftUI.NSHostingSizingOptions { _skeleton() }
    public static var intrinsicContentSize: SwiftUI.NSHostingSizingOptions { _skeleton() }
    public static var maxSize: SwiftUI.NSHostingSizingOptions { _skeleton() }
    public static var preferredContentSize: SwiftUI.NSHostingSizingOptions { _skeleton() }
}

extension SwiftUI.NSHostingSizingOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.NSHostingSizingOptions
}

extension SwiftUI.NSHostingSizingOptions: Swift.SetAlgebra {
}

extension SwiftUI.NSHostingSizingOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.NSHostingSizingOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.NSHostingSizingOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.NSHostingSizingOptions
}
