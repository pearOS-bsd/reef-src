// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RedactionReasons (Misc).

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

public struct RedactionReasons {
    public let rawValue: Swift.Int
    public init(rawValue a0: Swift.Int) { rawValue = a0 }
    public static var placeholder: SwiftUI.RedactionReasons { SwiftUI.RedactionReasons(rawValue: 1) }
    public static var privacy: SwiftUI.RedactionReasons { SwiftUI.RedactionReasons(rawValue: 2) }
    public static var invalidated: SwiftUI.RedactionReasons { SwiftUI.RedactionReasons(rawValue: 4) }
}

extension SwiftUI.RedactionReasons: Swift.OptionSet {
    public typealias Element = SwiftUI.RedactionReasons
}

extension SwiftUI.RedactionReasons: Swift.SetAlgebra {
}

extension SwiftUI.RedactionReasons: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.RedactionReasons: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.RedactionReasons: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.RedactionReasons
}
