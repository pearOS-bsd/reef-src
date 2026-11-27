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
    var value: A = _skeletonValue()
    let signal: SwiftUI.GraphSignal = _skeletonValue()
    final var peek: A { _skeleton() }
    override func set(_ a0: A, transaction a1: SwiftUI.Transaction) { _skeleton() }
    init(_ a0: A) { _skeleton() }
    override func get() -> A { _skeleton() }
    deinit {}
}
