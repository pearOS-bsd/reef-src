// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarContentBox (Navigation).

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

struct ToolbarContentBox<A> where A: SwiftUI.ToolbarContent {
    let content: A
    func collectActions(into a0: inout [SwiftUI.ToolbarActions.Item]) { _skeleton() }
    init(content a0: A) { _skeleton() }
}

extension SwiftUI.ToolbarContentBox: SwiftUI.ToolbarContent {
    typealias Body = Swift.Never
}

extension SwiftUI.ToolbarContentBox: SwiftUI.ToolbarActionSource {
}
