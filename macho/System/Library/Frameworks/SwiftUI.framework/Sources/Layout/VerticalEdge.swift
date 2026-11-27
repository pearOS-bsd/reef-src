// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VerticalEdge (Layout).

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

@frozen public enum VerticalEdge {
    case top
    case bottom
    public init?(rawValue a0: Swift.Int8) { _skeleton() }
    public static var allCases: [SwiftUI.VerticalEdge] { _skeleton() }
    public var rawValue: Swift.Int8 { _skeleton() }
    public struct Set {
        public let rawValue: Swift.Int8
        public init(rawValue a0: Swift.Int8) { rawValue = a0 }
        public static var top: SwiftUI.VerticalEdge.Set { _skeleton() }
        public static var bottom: SwiftUI.VerticalEdge.Set { _skeleton() }
        public static var all: SwiftUI.VerticalEdge.Set { _skeleton() }
    }
}

extension SwiftUI.VerticalEdge.Set: Swift.OptionSet {
    public typealias Element = SwiftUI.VerticalEdge.Set
}

extension SwiftUI.VerticalEdge.Set: Swift.SetAlgebra {
}

extension SwiftUI.VerticalEdge.Set: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.VerticalEdge.Set: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.VerticalEdge.Set: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.VerticalEdge.Set
}

extension SwiftUI.VerticalEdge: Swift.Equatable {
}

extension SwiftUI.VerticalEdge: Swift.Hashable {
}

extension SwiftUI.VerticalEdge: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.VerticalEdge: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.VerticalEdge]
}

extension SwiftUI.VerticalEdge: Swift.Decodable {
}

extension SwiftUI.VerticalEdge: Swift.Encodable {
}
