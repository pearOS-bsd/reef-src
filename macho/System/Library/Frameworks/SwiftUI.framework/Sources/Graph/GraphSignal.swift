// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GraphSignal (Graph).

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

class GraphSignal {
    var readers: [Swift.ObjectIdentifier: SwiftUI.GraphSignal.Reader] = [:]
    /// the current rule depends on this signal
    final func noteRead() {
        guard let rule = SwiftUI.GraphRule.current, readers[ObjectIdentifier(rule)] == nil else { return }
        readers[ObjectIdentifier(rule)] = Reader(rule: rule)
        rule.sources.append(self)
    }
    /// invalidate every rule that read this signal, on the main thread
    final func changed(_ a0: SwiftUI.Transaction) {
        let transaction = (a0.animation != nil || a0.disablesAnimations) ? a0 : SwiftUI.TransactionStack.current
        guard Thread.isMainThread else {
            DispatchQueue.main.async { self.changed(transaction) }
            return
        }
        readers = readers.filter { $0.value.rule != nil }
        let rules = readers.values.compactMap { $0.rule }
        for rule in rules { rule.invalidate(transaction) }
    }
    init() {}
    deinit {}
    struct Reader {
        weak var rule: SwiftUI.GraphRule?
    }
}
