// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimatedTransition (Animation).

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

struct AnimatedTransition<A> where A: SwiftUI.Transition {
    let base: A
    let animation: SwiftUI.Animation?
    func body(content a0: SwiftUI.PlaceholderContentView<SwiftUI.AnimatedTransition<A>>, phase a1: SwiftUI.TransitionPhase) -> SwiftUI.PlaceholderContentView<SwiftUI.AnimatedTransition<A>> { _skeleton() }
    init(base a0: A, animation a1: SwiftUI.Animation?) { _skeleton() }
}

extension SwiftUI.AnimatedTransition: SwiftUI.Transition {
    typealias Body = SwiftUI.PlaceholderContentView<SwiftUI.AnimatedTransition<A>>
}
