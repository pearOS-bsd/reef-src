// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CustomAnimation (Animation).

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

public protocol CustomAnimation: Swift.Hashable {
    func velocity<A1>(value a0: A1, time a1: Swift.Double, context a2: SwiftUI.AnimationContext<A1>) -> A1? where A1: SwiftUI.VectorArithmetic
    func animate<A1>(value a0: A1, time a1: Swift.Double, context a2: inout SwiftUI.AnimationContext<A1>) -> A1? where A1: SwiftUI.VectorArithmetic
    func shouldMerge<A1>(previous a0: SwiftUI.Animation, value a1: A1, time a2: Swift.Double, context a3: inout SwiftUI.AnimationContext<A1>) -> Swift.Bool where A1: SwiftUI.VectorArithmetic
}

extension SwiftUI.CustomAnimation {
    public func velocity<A1>(value a0: A1, time a1: Swift.Double, context a2: SwiftUI.AnimationContext<A1>) -> A1? where A1: SwiftUI.VectorArithmetic { _skeleton() }
    public func shouldMerge<A1>(previous a0: SwiftUI.Animation, value a1: A1, time a2: Swift.Double, context a3: inout SwiftUI.AnimationContext<A1>) -> Swift.Bool where A1: SwiftUI.VectorArithmetic { _skeleton() }
}
