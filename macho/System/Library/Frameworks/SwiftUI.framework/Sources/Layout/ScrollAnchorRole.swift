// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollAnchorRole (Layout).

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

public struct ScrollAnchorRole {
    let rawValue: Swift.Int
    public static var initialOffset: SwiftUI.ScrollAnchorRole { SwiftUI.ScrollAnchorRole(rawValue: 0) }
    public static var sizeChanges: SwiftUI.ScrollAnchorRole { SwiftUI.ScrollAnchorRole(rawValue: 1) }
    public static var alignment: SwiftUI.ScrollAnchorRole { SwiftUI.ScrollAnchorRole(rawValue: 2) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.ScrollAnchorRole, _ a1: SwiftUI.ScrollAnchorRole) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.ScrollAnchorRole: Swift.Hashable {
}

extension SwiftUI.ScrollAnchorRole: Swift.Equatable {
}
