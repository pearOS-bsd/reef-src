// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FrameResizeDirection (Layout).

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

public enum FrameResizeDirection {
    case inward
    case outward
    public init?(rawValue a0: Swift.Int8) {
        switch a0 {
        case 0: self = .inward
        case 1: self = .outward
        default: return nil
        }
    }
    public var rawValue: Swift.Int8 {
        switch self {
        case .inward: return 0
        case .outward: return 1
        }
    }
    public struct Set {
        public let rawValue: Swift.Int8
        public static var inward: SwiftUI.FrameResizeDirection.Set { _skeleton() }
        public static var outward: SwiftUI.FrameResizeDirection.Set { _skeleton() }
        public static var all: SwiftUI.FrameResizeDirection.Set { _skeleton() }
        public init(rawValue a0: Swift.Int8) { rawValue = a0 }
    }
}

extension SwiftUI.FrameResizeDirection.Set: Swift.OptionSet {
    public typealias Element = SwiftUI.FrameResizeDirection.Set
}

extension SwiftUI.FrameResizeDirection.Set: Swift.Hashable {
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.FrameResizeDirection.Set: Swift.SetAlgebra {
}

extension SwiftUI.FrameResizeDirection.Set: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.FrameResizeDirection.Set: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.FrameResizeDirection.Set: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.FrameResizeDirection.Set
}

extension SwiftUI.FrameResizeDirection: Swift.Equatable {
}

extension SwiftUI.FrameResizeDirection: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.FrameResizeDirection: Swift.Hashable {
}
