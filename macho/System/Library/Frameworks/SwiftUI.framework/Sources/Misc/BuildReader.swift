// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BuildReader (Misc).

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

final class BuildReader: SwiftUI.GraphRule {
    weak var node: SwiftUI.BodyNode?
    /// something the build read changed: the node and its ancestors build again (their bodies stay), on the main thread
    override func invalidate(_ a0: SwiftUI.Transaction) {
        let transaction = (a0.animation != nil || a0.disablesAnimations) ? a0 : SwiftUI.TransactionStack.current
        guard Thread.isMainThread else {
            DispatchQueue.main.async { self.invalidate(transaction) }
            return
        }
        var current = node
        while let node = current {
            node.needsBuild = true
            current = node.parentNode
        }
        host?.setNeedsUpdate(transaction)
    }
    deinit {}
}
