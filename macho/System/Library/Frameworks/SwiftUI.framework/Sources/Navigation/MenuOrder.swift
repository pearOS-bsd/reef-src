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
    public static var automatic: SwiftUI.MenuOrder { _skeleton() }
    public static var priority: SwiftUI.MenuOrder { _skeleton() }
    public static var fixed: SwiftUI.MenuOrder { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.MenuOrder, _ a1: SwiftUI.MenuOrder) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.MenuOrder: Swift.Equatable {
}

extension SwiftUI.MenuOrder: Swift.Hashable {
}
