// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VerticalDirection (Misc).

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

@frozen public enum VerticalDirection {
    case up
    case down
    public var rawValue: Swift.Int8 { _skeleton() }
    public static var allCases: [SwiftUI.VerticalDirection] { _skeleton() }
    public init?(rawValue a0: Swift.Int8) { _skeleton() }
    public struct Set {
        public let rawValue: Swift.Int8
        public init(rawValue a0: Swift.Int8) { _skeleton() }
        public init(_ a0: SwiftUI.VerticalDirection) { _skeleton() }
        public static var up: SwiftUI.VerticalDirection.Set { _skeleton() }
        public static var down: SwiftUI.VerticalDirection.Set { _skeleton() }
        public static var all: SwiftUI.VerticalDirection.Set { _skeleton() }
    }
}

extension SwiftUI.VerticalDirection.Set: Swift.OptionSet {
    public typealias Element = SwiftUI.VerticalDirection.Set
}

extension SwiftUI.VerticalDirection.Set: Swift.Hashable {
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.VerticalDirection.Set: Swift.SetAlgebra {
}

extension SwiftUI.VerticalDirection.Set: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.VerticalDirection.Set: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.VerticalDirection.Set: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.VerticalDirection.Set
}

extension SwiftUI.VerticalDirection: Swift.Equatable {
}

extension SwiftUI.VerticalDirection: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.VerticalDirection: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.VerticalDirection]
}

extension SwiftUI.VerticalDirection: Swift.Decodable {
}

extension SwiftUI.VerticalDirection: Swift.Encodable {
}

extension SwiftUI.VerticalDirection: Swift.Hashable {
}
