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
    public static var sidebar: SwiftUI.TabBarPlacement { _skeleton() }
    public static var bottomBar: SwiftUI.TabBarPlacement { _skeleton() }
    public static var pinnedSidebar: SwiftUI.TabBarPlacement { _skeleton() }
    public static var ornament: SwiftUI.TabBarPlacement { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.TabBarPlacement, _ a1: SwiftUI.TabBarPlacement) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.TabBarPlacement: Swift.Hashable {
}

extension SwiftUI.TabBarPlacement: Swift.Equatable {
}
