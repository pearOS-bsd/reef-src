// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabRole (Navigation).

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

public struct TabRole {
    let rawValue: Swift.Int
    public static var search: SwiftUI.TabRole { SwiftUI.TabRole(rawValue: 1) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.TabRole, _ a1: SwiftUI.TabRole) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.TabRole: Swift.Hashable {
}

extension SwiftUI.TabRole: Swift.Equatable {
}
