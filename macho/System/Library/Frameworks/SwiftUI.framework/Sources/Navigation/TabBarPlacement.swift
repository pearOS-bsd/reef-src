// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabBarPlacement (Navigation).

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

public struct TabBarPlacement {
    let rawValue: Swift.Int
    public static var topBar: SwiftUI.TabBarPlacement { _skeleton() }
    public static var sidebar: SwiftUI.TabBarPlacement { SwiftUI.TabBarPlacement(rawValue: 1) }
    public static var bottomBar: SwiftUI.TabBarPlacement { _skeleton() }
    public static var pinnedSidebar: SwiftUI.TabBarPlacement { SwiftUI.TabBarPlacement(rawValue: 3) }
    public static var ornament: SwiftUI.TabBarPlacement { SwiftUI.TabBarPlacement(rawValue: 4) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.TabBarPlacement, _ a1: SwiftUI.TabBarPlacement) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.TabBarPlacement: Swift.Hashable {
}

extension SwiftUI.TabBarPlacement: Swift.Equatable {
}
