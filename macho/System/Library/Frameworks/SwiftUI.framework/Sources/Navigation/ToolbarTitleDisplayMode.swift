// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarTitleDisplayMode (Navigation).

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

public struct ToolbarTitleDisplayMode {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.ToolbarTitleDisplayMode { SwiftUI.ToolbarTitleDisplayMode(rawValue: 0) }
    public static var large: SwiftUI.ToolbarTitleDisplayMode { SwiftUI.ToolbarTitleDisplayMode(rawValue: 1) }
    public static var inlineLarge: SwiftUI.ToolbarTitleDisplayMode { SwiftUI.ToolbarTitleDisplayMode(rawValue: 2) }
    public static var inline: SwiftUI.ToolbarTitleDisplayMode { SwiftUI.ToolbarTitleDisplayMode(rawValue: 3) }
}
