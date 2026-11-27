// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProjectedLocation (State).

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

class ProjectedLocation<A, B>: SwiftUI.AnyLocation<B> {
    let base: SwiftUI.AnyLocation<A>
    let keyPath: Swift.WritableKeyPath<A, B>
    let transaction: SwiftUI.Transaction
    init(base a0: SwiftUI.AnyLocation<A>, keyPath a1: Swift.WritableKeyPath<A, B>, transaction a2: SwiftUI.Transaction) {
        base = a0
        keyPath = a1
        transaction = a2
        super.init()
    }
    override func get() -> B { base.get()[keyPath: keyPath] }
    /// (with the transaction of the binding it was projected from)
    override func set(_ a0: B, transaction a1: SwiftUI.Transaction) {
        var value = base.get()
        value[keyPath: keyPath] = a0
        base.set(value, transaction: transaction)
    }
    override func isSame(`as` a0: SwiftUI.AnyLocationBase) -> Swift.Bool {
        guard let other = a0 as? SwiftUI.ProjectedLocation<A, B> else { return false }
        return keyPath == other.keyPath && base.isSame(as: other.base)
    }
    deinit {}
}
