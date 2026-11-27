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
    let getter: () -> A = _skeletonValue()
    let setter: (A, SwiftUI.Transaction) -> () = _skeletonValue()
    init(get a0: @escaping () -> A, set a1: @escaping (A, SwiftUI.Transaction) -> ()) { _skeleton() }
    override func set(_ a0: A, transaction a1: SwiftUI.Transaction) { _skeleton() }
    override func get() -> A { _skeleton() }
    deinit {}
}
