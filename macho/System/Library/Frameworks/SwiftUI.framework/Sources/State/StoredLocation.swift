// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StoredLocation (State).

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

class StoredLocation<A>: SwiftUI.AnyLocation<A> {
    var value: A
    let signal: SwiftUI.GraphSignal = SwiftUI.GraphSignal()
    /// the value without making the reader depend on it
    final var peek: A { value }
    /// a new value: the rules that read the old one update (on the main thread)
    override func set(_ a0: A, transaction a1: SwiftUI.Transaction) {
        guard Thread.isMainThread else {
            DispatchQueue.main.async { self.set(a0, transaction: a1) }
            return
        }
        let same = SwiftUI.GraphCompare.equal(value, a0)
        value = a0
        if same { return }
        signal.changed(a1.animation != nil ? a1 : SwiftUI.TransactionStack.current)
    }
    init(_ a0: A) {
        value = a0
        super.init()
    }
    override func get() -> A {
        signal.noteRead()
        return value
    }
    deinit {}
}
