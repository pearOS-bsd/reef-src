// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HorizontalEdge (Layout).

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

@frozen public enum HorizontalEdge {
    case leading
    case trailing
    public static var allCases: [SwiftUI.HorizontalEdge] { _skeleton() }
    public var rawValue: Swift.Int8 { _skeleton() }
    public init?(rawValue a0: Swift.Int8) { _skeleton() }
    public struct Set {
        public let rawValue: Swift.Int8
        public init(rawValue a0: Swift.Int8) { _skeleton() }
        public static var leading: SwiftUI.HorizontalEdge.Set { _skeleton() }
        public static var trailing: SwiftUI.HorizontalEdge.Set { _skeleton() }
        public static var all: SwiftUI.HorizontalEdge.Set { _skeleton() }
    }
}

extension SwiftUI.HorizontalEdge.Set: Swift.OptionSet {
    public typealias Element = SwiftUI.HorizontalEdge.Set
}

extension SwiftUI.HorizontalEdge.Set: Swift.SetAlgebra {
}

extension SwiftUI.HorizontalEdge.Set: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.HorizontalEdge.Set: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.HorizontalEdge.Set: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.HorizontalEdge.Set
}

extension SwiftUI.HorizontalEdge: Swift.Equatable {
}

extension SwiftUI.HorizontalEdge: Swift.Hashable {
}

extension SwiftUI.HorizontalEdge: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.HorizontalEdge: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.HorizontalEdge]
}

extension SwiftUI.HorizontalEdge: Swift.Decodable {
}

extension SwiftUI.HorizontalEdge: Swift.Encodable {
}
