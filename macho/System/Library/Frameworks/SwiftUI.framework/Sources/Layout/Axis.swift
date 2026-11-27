// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Axis (Layout).

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

@frozen public enum Axis {
    case horizontal
    case vertical
    public var rawValue: Swift.Int8 { _skeleton() }
    public static var allCases: [SwiftUI.Axis] { _skeleton() }
    public init?(rawValue a0: Swift.Int8) { _skeleton() }
    public struct Set {
        public let rawValue: Swift.Int8
        public static var horizontal: SwiftUI.Axis.Set { _skeleton() }
        public static var vertical: SwiftUI.Axis.Set { _skeleton() }
        public init(rawValue a0: Swift.Int8) { _skeleton() }
    }
}

extension SwiftUI.Axis.Set: Swift.OptionSet {
    public typealias Element = SwiftUI.Axis.Set
}

extension SwiftUI.Axis.Set: Swift.SetAlgebra {
}

extension SwiftUI.Axis.Set: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.Axis.Set: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Axis.Set: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.Axis.Set
}

extension SwiftUI.Axis: Swift.Equatable {
}

extension SwiftUI.Axis: Swift.Hashable {
}

extension SwiftUI.Axis: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.Axis: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.Axis]
}
