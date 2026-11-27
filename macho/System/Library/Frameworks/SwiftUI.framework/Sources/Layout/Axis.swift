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
    public var rawValue: Swift.Int8 {
        switch self {
        case .horizontal: return 0
        case .vertical: return 1
        }
    }
    public static var allCases: [SwiftUI.Axis] { [.horizontal, .vertical] }
    public init?(rawValue a0: Swift.Int8) {
        switch a0 {
        case 0: self = .horizontal
        case 1: self = .vertical
        default: return nil
        }
    }
    public struct Set {
        public let rawValue: Swift.Int8
        public static var horizontal: SwiftUI.Axis.Set { SwiftUI.Axis.Set(rawValue: 1) }
        public static var vertical: SwiftUI.Axis.Set { SwiftUI.Axis.Set(rawValue: 2) }
        public init(rawValue a0: Swift.Int8) { rawValue = a0 }
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
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
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
