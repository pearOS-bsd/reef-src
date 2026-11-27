// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AlternatingRowBackgroundBehavior (Layout).

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

public struct AlternatingRowBackgroundBehavior {
    let rawValue: Swift.Int
    public static var enabled: SwiftUI.AlternatingRowBackgroundBehavior { SwiftUI.AlternatingRowBackgroundBehavior(rawValue: 1) }
    public static var automatic: SwiftUI.AlternatingRowBackgroundBehavior { SwiftUI.AlternatingRowBackgroundBehavior(rawValue: 0) }
    public static var disabled: SwiftUI.AlternatingRowBackgroundBehavior { SwiftUI.AlternatingRowBackgroundBehavior(rawValue: 2) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.AlternatingRowBackgroundBehavior, _ a1: SwiftUI.AlternatingRowBackgroundBehavior) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.AlternatingRowBackgroundBehavior: Swift.Hashable {
}

extension SwiftUI.AlternatingRowBackgroundBehavior: Swift.Equatable {
}
