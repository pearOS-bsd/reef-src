// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarItemGroup (Navigation).

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

public struct ToolbarItemGroup<A> where A: SwiftUI.View {
    let placement: SwiftUI.ToolbarItemPlacement
    let content: A
    public init(placement a0: SwiftUI.ToolbarItemPlacement, content a1: () -> A) { _skeleton() }
    func collectActions(into a0: inout [SwiftUI.ToolbarActions.Item]) { _skeleton() }
}

extension SwiftUI.ToolbarItemGroup: SwiftUI.ToolbarContent {
    public typealias Body = Swift.Never
}

extension SwiftUI.ToolbarItemGroup: SwiftUI.ToolbarActionSource {
}
