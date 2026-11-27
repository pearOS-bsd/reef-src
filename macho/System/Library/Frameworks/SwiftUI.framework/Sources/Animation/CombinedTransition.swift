// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CombinedTransition (Animation).

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

struct CombinedTransition<A, B> where A: SwiftUI.Transition, B: SwiftUI.Transition {
    let first: A
    let second: B
    init(first a0: A, second a1: B) { _skeleton() }
    func body(content a0: SwiftUI.PlaceholderContentView<SwiftUI.CombinedTransition<A, B>>, phase a1: SwiftUI.TransitionPhase) -> SwiftUI.PlaceholderContentView<SwiftUI.CombinedTransition<A, B>> { _skeleton() }
}

extension SwiftUI.CombinedTransition: SwiftUI.Transition {
    typealias Body = SwiftUI.PlaceholderContentView<SwiftUI.CombinedTransition<A, B>>
}
