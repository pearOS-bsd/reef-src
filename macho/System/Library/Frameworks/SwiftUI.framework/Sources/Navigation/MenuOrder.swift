// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MenuOrder (Navigation).

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

public struct MenuOrder {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.MenuOrder { SwiftUI.MenuOrder(rawValue: 0) }
    public static var priority: SwiftUI.MenuOrder { SwiftUI.MenuOrder(rawValue: 1) }
    public static var fixed: SwiftUI.MenuOrder { SwiftUI.MenuOrder(rawValue: 2) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.MenuOrder, _ a1: SwiftUI.MenuOrder) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.MenuOrder: Swift.Equatable {
}

extension SwiftUI.MenuOrder: Swift.Hashable {
}
