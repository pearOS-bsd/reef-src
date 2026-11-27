// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ModifierContent (Views).

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

struct ModifierContent {
    let build: (SwiftUI.BuildContext) -> [SwiftUI.Element]
    let context: SwiftUI.BuildContext
    /// build the content in its own context, under an index of the caller's path, with the caller's environment and canvas
    func callAsFunction(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        var context = self.context
        context.path.append(AnyHashable(a0.path.count))
        context.environment = a0.environment
        context.canvas = a0.canvas
        return build(context)
    }
}
