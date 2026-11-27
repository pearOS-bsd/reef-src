// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonRole (Controls).

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

public struct ButtonRole {
    let rawValue: Swift.Int
    public static var destructive: SwiftUI.ButtonRole { SwiftUI.ButtonRole(rawValue: 1) }
    public static var cancel: SwiftUI.ButtonRole { SwiftUI.ButtonRole(rawValue: 2) }
    public static var confirm: SwiftUI.ButtonRole { SwiftUI.ButtonRole(rawValue: 3) }
    public static var close: SwiftUI.ButtonRole { SwiftUI.ButtonRole(rawValue: 4) }
    public static func == (_ a0: SwiftUI.ButtonRole, _ a1: SwiftUI.ButtonRole) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
}

extension SwiftUI.ButtonRole: Swift.Equatable {
}

extension SwiftUI.ButtonRole: Swift.Hashable {
}
