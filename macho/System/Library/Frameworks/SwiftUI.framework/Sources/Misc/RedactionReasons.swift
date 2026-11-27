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
    public init(rawValue a0: Swift.Int) { _skeleton() }
    public static var placeholder: SwiftUI.RedactionReasons { _skeleton() }
    public static var privacy: SwiftUI.RedactionReasons { _skeleton() }
    public static var invalidated: SwiftUI.RedactionReasons { _skeleton() }
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
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.RedactionReasons: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.RedactionReasons
}
