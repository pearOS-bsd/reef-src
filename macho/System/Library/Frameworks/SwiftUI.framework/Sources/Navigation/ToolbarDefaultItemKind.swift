// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarDefaultItemKind (Navigation).

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

public struct ToolbarDefaultItemKind {
    let rawValue: Swift.Int
    public static var sidebarToggle: SwiftUI.ToolbarDefaultItemKind { SwiftUI.ToolbarDefaultItemKind(rawValue: 0) }
    public static var title: SwiftUI.ToolbarDefaultItemKind { SwiftUI.ToolbarDefaultItemKind(rawValue: 1) }
    public static func == (_ a0: SwiftUI.ToolbarDefaultItemKind, _ a1: SwiftUI.ToolbarDefaultItemKind) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.ToolbarDefaultItemKind: Swift.Equatable {
}
