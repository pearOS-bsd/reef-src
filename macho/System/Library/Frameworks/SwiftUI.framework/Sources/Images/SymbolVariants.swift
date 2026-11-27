// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SymbolVariants (Images).

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

public struct SymbolVariants {
    let rawValue: Swift.Int
    func name(of a0: Swift.String) -> Swift.String { _skeleton() }
    public static var fill: SwiftUI.SymbolVariants { SwiftUI.SymbolVariants(rawValue: 8) }
    public static var slash: SwiftUI.SymbolVariants { SwiftUI.SymbolVariants(rawValue: 16) }
    public var circle: SwiftUI.SymbolVariants { _skeleton() }
    public var square: SwiftUI.SymbolVariants { _skeleton() }
    public var rectangle: SwiftUI.SymbolVariants { _skeleton() }
    public var fill: SwiftUI.SymbolVariants { _skeleton() }
    public var slash: SwiftUI.SymbolVariants { _skeleton() }
    public func contains(_ a0: SwiftUI.SymbolVariants) -> Swift.Bool { _skeleton() }
    public static var none: SwiftUI.SymbolVariants { SwiftUI.SymbolVariants(rawValue: 0) }
    public static var circle: SwiftUI.SymbolVariants { SwiftUI.SymbolVariants(rawValue: 1) }
    public static var square: SwiftUI.SymbolVariants { SwiftUI.SymbolVariants(rawValue: 2) }
    public static var rectangle: SwiftUI.SymbolVariants { SwiftUI.SymbolVariants(rawValue: 4) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.SymbolVariants, _ a1: SwiftUI.SymbolVariants) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.SymbolVariants: Swift.Hashable {
}

extension SwiftUI.SymbolVariants: Swift.Equatable {
}
