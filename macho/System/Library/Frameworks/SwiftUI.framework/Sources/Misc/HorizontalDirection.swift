// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HorizontalDirection (Misc).

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

@frozen public enum HorizontalDirection {
    case leading
    case trailing
    public var rawValue: Swift.Int8 { _skeleton() }
    public static var allCases: [SwiftUI.HorizontalDirection] { _skeleton() }
    public init?(rawValue a0: Swift.Int8) { _skeleton() }
    public struct Set {
        public let rawValue: Swift.Int8
        public init(rawValue a0: Swift.Int8) { rawValue = a0 }
        public static var leading: SwiftUI.HorizontalDirection.Set { _skeleton() }
        public static var trailing: SwiftUI.HorizontalDirection.Set { _skeleton() }
        public static var all: SwiftUI.HorizontalDirection.Set { _skeleton() }
        public init(_ a0: SwiftUI.HorizontalDirection) { _skeleton() }
    }
}

extension SwiftUI.HorizontalDirection.Set: Swift.OptionSet {
    public typealias Element = SwiftUI.HorizontalDirection.Set
}

extension SwiftUI.HorizontalDirection.Set: Swift.Hashable {
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.HorizontalDirection.Set: Swift.SetAlgebra {
}

extension SwiftUI.HorizontalDirection.Set: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.HorizontalDirection.Set: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.HorizontalDirection.Set: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.HorizontalDirection.Set
}

extension SwiftUI.HorizontalDirection: Swift.Equatable {
}

extension SwiftUI.HorizontalDirection: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.HorizontalDirection: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.HorizontalDirection]
}

extension SwiftUI.HorizontalDirection: Swift.Decodable {
}

extension SwiftUI.HorizontalDirection: Swift.Encodable {
}

extension SwiftUI.HorizontalDirection: Swift.Hashable {
}
