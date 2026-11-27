// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationSplitViewColumn (Navigation).

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

public struct NavigationSplitViewColumn {
    let rawValue: Swift.Int
    public static var sidebar: SwiftUI.NavigationSplitViewColumn { SwiftUI.NavigationSplitViewColumn(rawValue: 0) }
    public static var content: SwiftUI.NavigationSplitViewColumn { SwiftUI.NavigationSplitViewColumn(rawValue: 1) }
    public static var detail: SwiftUI.NavigationSplitViewColumn { SwiftUI.NavigationSplitViewColumn(rawValue: 2) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.NavigationSplitViewColumn, _ a1: SwiftUI.NavigationSplitViewColumn) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.NavigationSplitViewColumn: Swift.Hashable {
}

extension SwiftUI.NavigationSplitViewColumn: Swift.Equatable {
}
