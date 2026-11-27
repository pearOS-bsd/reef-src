// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarItem (Navigation).

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

public struct ToolbarItem<A, B> where B: SwiftUI.View {
    let placement: SwiftUI.ToolbarItemPlacement
    let content: B
    init(placement a0: SwiftUI.ToolbarItemPlacement, content a1: B) { _skeleton() }
    func collectActions(into a0: inout [SwiftUI.ToolbarActions.Item]) { _skeleton() }
}

extension SwiftUI.ToolbarItem where A == Swift.String {
    public init(id a0: Swift.String, placement a1: SwiftUI.ToolbarItemPlacement, showsByDefault a2: Swift.Bool, content a3: () -> B) { _skeleton() }
}

extension SwiftUI.ToolbarItem where A == () {
    public init(placement a0: SwiftUI.ToolbarItemPlacement, content a1: () -> B) { _skeleton() }
}

extension SwiftUI.ToolbarItem: SwiftUI.CustomizableToolbarContent where A == Swift.String {
}

extension SwiftUI.ToolbarItem: SwiftUI.ToolbarContent {
    public typealias Body = Swift.Never
}

extension SwiftUI.ToolbarItem: SwiftUI.ToolbarActionSource {
}
