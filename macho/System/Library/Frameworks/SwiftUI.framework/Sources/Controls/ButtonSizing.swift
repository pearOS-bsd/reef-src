// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonSizing (Controls).

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

public struct ButtonSizing {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.ButtonSizing { SwiftUI.ButtonSizing(rawValue: 0) }
    public static var fitted: SwiftUI.ButtonSizing { SwiftUI.ButtonSizing(rawValue: 1) }
    public static var flexible: SwiftUI.ButtonSizing { SwiftUI.ButtonSizing(rawValue: 2) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ButtonSizing, _ a1: SwiftUI.ButtonSizing) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.ButtonSizing: Swift.Hashable {
}

extension SwiftUI.ButtonSizing: Swift.Equatable {
}
