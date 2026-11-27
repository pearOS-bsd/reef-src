// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Edge (Layout).

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

@frozen public enum Edge {
    case top
    case leading
    case bottom
    case trailing
    public var rawValue: Swift.Int8 {
        switch self {
        case .top: return 0
        case .leading: return 1
        case .bottom: return 2
        case .trailing: return 3
        }
    }
    public init?(rawValue a0: Swift.Int8) {
        switch a0 {
        case 0: self = .top
        case 1: self = .leading
        case 2: self = .bottom
        case 3: self = .trailing
        default: return nil
        }
    }
    public static var allCases: [SwiftUI.Edge] { [.top, .leading, .bottom, .trailing] }
    public struct Set {
        public let rawValue: Swift.Int8
        public init(rawValue a0: Swift.Int8) { rawValue = a0 }
        public init(_ a0: SwiftUI.Edge) { rawValue = 1 << a0.rawValue }
        public static var top: SwiftUI.Edge.Set { SwiftUI.Edge.Set(rawValue: 1) }
        public static var leading: SwiftUI.Edge.Set { SwiftUI.Edge.Set(rawValue: 2) }
        public static var bottom: SwiftUI.Edge.Set { SwiftUI.Edge.Set(rawValue: 4) }
        public static var trailing: SwiftUI.Edge.Set { SwiftUI.Edge.Set(rawValue: 8) }
        public static var all: SwiftUI.Edge.Set { SwiftUI.Edge.Set(rawValue: 15) }
        public static var horizontal: SwiftUI.Edge.Set { SwiftUI.Edge.Set(rawValue: 10) }
        public static var vertical: SwiftUI.Edge.Set { SwiftUI.Edge.Set(rawValue: 5) }
    }
    public enum Corner {
        case topLeading
        case topTrailing
        case bottomLeading
        case bottomTrailing
        public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        public var hashValue: Swift.Int { _hashValue(for: self) }
        public static func == (_ a0: SwiftUI.Edge.Corner, _ a1: SwiftUI.Edge.Corner) -> Swift.Bool { unsafeBitCast(a0, to: UInt8.self) == unsafeBitCast(a1, to: UInt8.self) }
        public struct Style {
            let radius: CoreGraphics.CGFloat
            let concentric: Swift.Bool
            public init(integerLiteral a0: Swift.Int) { _skeleton() }
            public static func fixed(_ a0: CoreGraphics.CGFloat) -> SwiftUI.Edge.Corner.Style { _skeleton() }
            public static var concentric: SwiftUI.Edge.Corner.Style { _skeleton() }
            public static func concentric(minimum a0: SwiftUI.Edge.Corner.Style?) -> SwiftUI.Edge.Corner.Style { _skeleton() }
            public static func == (_ a0: SwiftUI.Edge.Corner.Style, _ a1: SwiftUI.Edge.Corner.Style) -> Swift.Bool { _skeleton() }
            public func hash(into a0: inout Swift.Hasher) { _skeleton() }
            public var hashValue: Swift.Int { _skeleton() }
        }
    }
}

extension SwiftUI.Edge.Set: Swift.OptionSet {
    public typealias Element = SwiftUI.Edge.Set
}

extension SwiftUI.Edge.Set: Swift.SetAlgebra {
}

extension SwiftUI.Edge.Set: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.Edge.Set: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.Edge.Set: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.Edge.Set
}

extension SwiftUI.Edge: Swift.Equatable {
}

extension SwiftUI.Edge: Swift.Hashable {
}

extension SwiftUI.Edge: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.Edge: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.Edge]
}

extension SwiftUI.Edge.Corner.Style: Swift.Hashable {
}

extension SwiftUI.Edge.Corner.Style: Swift.ExpressibleByIntegerLiteral {
    public typealias IntegerLiteralType = Swift.Int
}

extension SwiftUI.Edge.Corner.Style: Swift.Equatable {
}

extension SwiftUI.Edge.Corner: Swift.Equatable {
}

extension SwiftUI.Edge.Corner: Swift.Hashable {
}
