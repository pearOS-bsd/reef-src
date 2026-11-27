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
    public init(extendedGraphemeClusterLiteral a0: Swift.Character) { _skeleton() }
    public static var upArrow: SwiftUI.KeyEquivalent { _skeleton() }
    public static var downArrow: SwiftUI.KeyEquivalent { _skeleton() }
    public static var leftArrow: SwiftUI.KeyEquivalent { _skeleton() }
    public static var rightArrow: SwiftUI.KeyEquivalent { _skeleton() }
    public static var escape: SwiftUI.KeyEquivalent { _skeleton() }
    public static var delete: SwiftUI.KeyEquivalent { _skeleton() }
    public static var deleteForward: SwiftUI.KeyEquivalent { _skeleton() }
    public static var home: SwiftUI.KeyEquivalent { _skeleton() }
    public static var end: SwiftUI.KeyEquivalent { _skeleton() }
    public static var pageUp: SwiftUI.KeyEquivalent { _skeleton() }
    public static var pageDown: SwiftUI.KeyEquivalent { _skeleton() }
    public static var clear: SwiftUI.KeyEquivalent { _skeleton() }
    public static var tab: SwiftUI.KeyEquivalent { _skeleton() }
    public static var space: SwiftUI.KeyEquivalent { _skeleton() }
    public static var `return`: SwiftUI.KeyEquivalent { _skeleton() }
    public static func == (_ a0: SwiftUI.KeyEquivalent, _ a1: SwiftUI.KeyEquivalent) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public init(_ a0: Swift.Character) { _skeleton() }
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
