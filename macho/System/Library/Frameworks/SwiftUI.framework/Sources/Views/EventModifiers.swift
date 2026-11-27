// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EventModifiers (Views).

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

public struct EventModifiers {
    public let rawValue: Swift.Int
    public init(rawValue a0: Swift.Int) { rawValue = a0 }
    public static var capsLock: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 1) }
    public static var shift: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 2) }
    public static var control: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 4) }
    public static var option: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 8) }
    public static var command: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 16) }
    public static var numericPad: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 32) }
    public static var function: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 128) }
    public static var all: SwiftUI.EventModifiers { SwiftUI.EventModifiers(rawValue: 191) }
    init(_ a0: NSEvent.ModifierFlags) { _skeleton() }
}

extension SwiftUI.EventModifiers: Swift.OptionSet {
    public typealias Element = SwiftUI.EventModifiers
}

extension SwiftUI.EventModifiers: Swift.Hashable {
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.EventModifiers: Swift.SetAlgebra {
}

extension SwiftUI.EventModifiers: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.EventModifiers: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.EventModifiers: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.EventModifiers
}
