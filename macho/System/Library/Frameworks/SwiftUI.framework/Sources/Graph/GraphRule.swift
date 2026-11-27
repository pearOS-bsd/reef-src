// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GraphRule (Graph).

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

class GraphRule {
    var sources: [SwiftUI.GraphSignal] = []
    var isDirty: Swift.Bool = false
    weak var host: SwiftUI.ViewHost?
    /// run without a current rule: nothing read is tracked
    static func untracked<A>(_ a0: () -> A) -> A {
        let previous = current
        current = nil
        defer { current = previous }
        return a0()
    }
    /// mark dirty and ask the host for an update, on the main thread
    func invalidate(_ a0: SwiftUI.Transaction) {
        let transaction = (a0.animation != nil || a0.disablesAnimations) ? a0 : SwiftUI.TransactionStack.current
        guard Thread.isMainThread else {
            DispatchQueue.main.async { self.invalidate(transaction) }
            return
        }
        isDirty = true
        host?.setNeedsUpdate(transaction)
    }
    /// run as the current rule: the signals read while it runs become its sources
    func track<A>(_ a0: () -> A) -> A {
        for source in sources { source.readers.removeValue(forKey: ObjectIdentifier(self)) }
        sources = []
        let previous = SwiftUI.GraphRule.current
        SwiftUI.GraphRule.current = self
        isDirty = false
        defer { SwiftUI.GraphRule.current = previous }
        return a0()
    }
    static func reading<A>(`as` a0: SwiftUI.GraphRule, _ a1: () -> A) -> A {
        let previous = current
        current = a0
        defer { current = previous }
        return a1()
    }
    init() {}
    static var current: SwiftUI.GraphRule? = nil
    deinit {}
}
