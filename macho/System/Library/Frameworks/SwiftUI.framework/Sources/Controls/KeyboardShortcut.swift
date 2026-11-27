// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyboardShortcut (Controls).

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

public struct KeyboardShortcut {
    public var key: SwiftUI.KeyEquivalent
    public var modifiers: SwiftUI.EventModifiers
    public init(_ a0: SwiftUI.KeyEquivalent, modifiers a1: SwiftUI.EventModifiers) { key = a0; modifiers = a1 }
    public static var defaultAction: SwiftUI.KeyboardShortcut { SwiftUI.KeyboardShortcut(.return, modifiers: []) }
    public static var cancelAction: SwiftUI.KeyboardShortcut { SwiftUI.KeyboardShortcut(.escape, modifiers: []) }
    public static func == (_ a0: SwiftUI.KeyboardShortcut, _ a1: SwiftUI.KeyboardShortcut) -> Swift.Bool { a0.key == a1.key && a0.modifiers == a1.modifiers }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(key.character); a0.combine(modifiers.rawValue) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
}

extension SwiftUI.KeyboardShortcut: Swift.Hashable {
}

extension SwiftUI.KeyboardShortcut: Swift.Equatable {
}
