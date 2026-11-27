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
    let base: SwiftUI.AnyLocation<A> = _skeletonValue()
    let keyPath: Swift.WritableKeyPath<A, B> = _skeletonValue()
    let transaction: SwiftUI.Transaction = _skeletonValue()
    init(base a0: SwiftUI.AnyLocation<A>, keyPath a1: Swift.WritableKeyPath<A, B>, transaction a2: SwiftUI.Transaction) { _skeleton() }
    override func get() -> B { _skeleton() }
    override func set(_ a0: B, transaction a1: SwiftUI.Transaction) { _skeleton() }
    override func isSame(`as` a0: SwiftUI.AnyLocationBase) -> Swift.Bool { _skeleton() }
    deinit {}
}
