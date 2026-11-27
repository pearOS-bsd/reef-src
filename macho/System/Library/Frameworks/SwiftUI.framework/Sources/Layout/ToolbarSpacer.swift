// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarSpacer (Layout).

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

public struct ToolbarSpacer {
    let sizing: SwiftUI.SpacerSizing
    let placement: SwiftUI.ToolbarItemPlacement
    public init(_ a0: SwiftUI.SpacerSizing, placement a1: SwiftUI.ToolbarItemPlacement) { _skeleton() }
}

extension SwiftUI.ToolbarSpacer: SwiftUI.ToolbarContent {
    public typealias Body = Swift.Never
}

extension SwiftUI.ToolbarSpacer: SwiftUI.CustomizableToolbarContent {
}
