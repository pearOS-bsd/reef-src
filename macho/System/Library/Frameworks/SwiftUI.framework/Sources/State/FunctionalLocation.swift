// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FunctionalLocation (State).

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

class FunctionalLocation<A>: SwiftUI.AnyLocation<A> {
    let getter: () -> A
    let setter: (A, SwiftUI.Transaction) -> ()
    init(get a0: @escaping () -> A, set a1: @escaping (A, SwiftUI.Transaction) -> ()) {
        getter = a0
        setter = a1
        super.init()
    }
    override func set(_ a0: A, transaction a1: SwiftUI.Transaction) { setter(a0, a1) }
    override func get() -> A { getter() }
    deinit {}
}
