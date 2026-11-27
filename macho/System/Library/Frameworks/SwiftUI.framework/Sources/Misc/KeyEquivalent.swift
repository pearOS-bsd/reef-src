// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyEquivalent (Misc).

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

public struct KeyEquivalent {
    public var character: Swift.Character
    public init(extendedGraphemeClusterLiteral a0: Swift.Character) { character = a0 }
    public static var upArrow: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F700}") }
    public static var downArrow: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F701}") }
    public static var leftArrow: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F702}") }
    public static var rightArrow: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F703}") }
    public static var escape: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{1B}") }
    public static var delete: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{8}") }
    public static var deleteForward: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F728}") }
    public static var home: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F729}") }
    public static var end: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F72B}") }
    public static var pageUp: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F72C}") }
    public static var pageDown: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F72D}") }
    public static var clear: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\u{F739}") }
    public static var tab: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\t") }
    public static var space: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent(" ") }
    public static var `return`: SwiftUI.KeyEquivalent { SwiftUI.KeyEquivalent("\r") }
    public static func == (_ a0: SwiftUI.KeyEquivalent, _ a1: SwiftUI.KeyEquivalent) -> Swift.Bool { a0.character == a1.character }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(character) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public init(_ a0: Swift.Character) { character = a0 }
}

extension SwiftUI.KeyEquivalent: Swift.Hashable {
}

extension SwiftUI.KeyEquivalent: Swift.ExpressibleByExtendedGraphemeClusterLiteral {
    public typealias ExtendedGraphemeClusterLiteralType = Swift.Character
}

extension SwiftUI.KeyEquivalent: Swift.Equatable {
}

extension SwiftUI.KeyEquivalent: Swift.ExpressibleByUnicodeScalarLiteral {
    public typealias UnicodeScalarLiteralType = Swift.Character
}
