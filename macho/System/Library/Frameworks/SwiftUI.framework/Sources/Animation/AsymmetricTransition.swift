// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AsymmetricTransition (Animation).

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

public struct AsymmetricTransition<A, B> where A: SwiftUI.Transition, B: SwiftUI.Transition {
    public var insertion: A
    public var removal: B
    public init(insertion a0: A, removal a1: B) { _skeleton() }
    public func body(content a0: SwiftUI.PlaceholderContentView<SwiftUI.AsymmetricTransition<A, B>>, phase a1: SwiftUI.TransitionPhase) -> some SwiftUI.View { _skeleton() }
}

extension SwiftUI.AsymmetricTransition: SwiftUI.Transition {
}
