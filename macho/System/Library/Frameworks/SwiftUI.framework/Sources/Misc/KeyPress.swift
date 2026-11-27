// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyPress (Misc).

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

public struct KeyPress {
    public let phase: SwiftUI.KeyPress.Phases
    public let key: SwiftUI.KeyEquivalent
    public let characters: Swift.String
    public let modifiers: SwiftUI.EventModifiers
    init(event a0: NSEvent, phase a1: SwiftUI.KeyPress.Phases) { _skeleton() }
    public struct Phases {
        public let rawValue: Swift.Int
        public static var all: SwiftUI.KeyPress.Phases { _skeleton() }
        public var description: Swift.String { _skeleton() }
        public init(rawValue a0: Swift.Int) { _skeleton() }
        public static var down: SwiftUI.KeyPress.Phases { _skeleton() }
        public static var `repeat`: SwiftUI.KeyPress.Phases { _skeleton() }
        public static var up: SwiftUI.KeyPress.Phases { _skeleton() }
    }
    public enum Result {
        case handled
        case ignored
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public static func == (_ a0: SwiftUI.KeyPress.Result, _ a1: SwiftUI.KeyPress.Result) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.KeyPress.Phases: Swift.OptionSet {
    public typealias Element = SwiftUI.KeyPress.Phases
}

extension SwiftUI.KeyPress.Phases: Swift.Hashable {
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.KeyPress.Phases: Swift.SetAlgebra {
}

extension SwiftUI.KeyPress.Phases: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.KeyPress.Phases: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.KeyPress.Phases: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.KeyPress.Phases
}

extension SwiftUI.KeyPress.Phases: Swift.CustomStringConvertible {
}

extension SwiftUI.KeyPress.Result: Swift.Equatable {
}

extension SwiftUI.KeyPress.Result: Swift.Hashable {
}
