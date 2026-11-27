// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimatableSlot (Animation).

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

class AnimatableSlot<A> where A: SwiftUI.VectorArithmetic {
    var target: A = _skeletonValue()
    var presented: A = _skeletonValue()
    var running: (SwiftUI.RunningAnimation<A>)? = _skeletonValue()
    weak var node: SwiftUI.BodyNode?
    init(_ a0: A) { _skeleton() }
    final func update(_ a0: A, transaction a1: SwiftUI.Transaction) -> A { _skeleton() }
    final func tick() -> Swift.Bool { _skeleton() }
    final var isRunning: Swift.Bool { _skeleton() }
    deinit {}
}

extension SwiftUI.AnimatableSlot: SwiftUI.AnimatingSlot {
}
