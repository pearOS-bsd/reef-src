// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RunningAnimation (Animation).

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

class RunningAnimation<A> where A: SwiftUI.VectorArithmetic {
    let animation: SwiftUI.Animation = _skeletonValue()
    let start: Swift.Double = _skeletonValue()
    var from: A = _skeletonValue()
    var delta: A = _skeletonValue()
    var context: SwiftUI.AnimationContext<A> = _skeletonValue()
    var completions: [SwiftUI.AnimationCompletionTicket] = _skeletonValue()
    var reportedLogical: Swift.Bool = _skeletonValue()
    final func value(at a0: Swift.Double) -> A? { _skeleton() }
    final func finish(removed a0: Swift.Bool) { _skeleton() }
    final func run(_ a0: SwiftUI.AnimationCompletionCriteria) { _skeleton() }
    init(animation a0: SwiftUI.Animation, start a1: Swift.Double, from a2: A, delta a3: A, completions a4: [SwiftUI.AnimationCompletionTicket]) { _skeleton() }
    deinit {}
}
