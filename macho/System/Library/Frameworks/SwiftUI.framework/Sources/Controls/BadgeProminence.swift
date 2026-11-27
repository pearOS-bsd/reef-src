// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BadgeProminence (Controls).

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

public struct BadgeProminence {
    let rawValue: Swift.Int
    public static var decreased: SwiftUI.BadgeProminence { SwiftUI.BadgeProminence(rawValue: 0) }
    public static var standard: SwiftUI.BadgeProminence { SwiftUI.BadgeProminence(rawValue: 1) }
    public static var increased: SwiftUI.BadgeProminence { SwiftUI.BadgeProminence(rawValue: 2) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.BadgeProminence, _ a1: SwiftUI.BadgeProminence) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.BadgeProminence: Swift.Hashable {
}

extension SwiftUI.BadgeProminence: Swift.Equatable {
}
