// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimationContext (Animation).

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

public struct AnimationContext<A> where A: SwiftUI.VectorArithmetic {
    public var state: SwiftUI.AnimationState<A>
    public var isLogicallyComplete: Swift.Bool
    public var environment: SwiftUI.EnvironmentValues
    init(state a0: SwiftUI.AnimationState<A> = _skeletonDefault(), isLogicallyComplete a1: Swift.Bool, environment a2: SwiftUI.EnvironmentValues = _skeletonDefault()) { _skeleton() }
    public func withState<A1>(_ a0: SwiftUI.AnimationState<A1>) -> SwiftUI.AnimationContext<A1> where A1: SwiftUI.VectorArithmetic { _skeleton() }
}
