// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CustomAnimationBox (Animation).

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

class CustomAnimationBox<A>: SwiftUI.AnimationBoxBase where A: SwiftUI.CustomAnimation {
    let base: A = _skeletonValue()
    init(_ a0: A) { _skeleton() }
    override func isEqual(to a0: SwiftUI.AnimationBoxBase) -> Swift.Bool { _skeleton() }
    override func hash(into a0: inout Swift.Hasher) { _skeleton() }
    override var customBase: any SwiftUI.CustomAnimation { _skeleton() }
    override func animate<A1>(value a0: A1, time a1: Swift.Double, context a2: inout SwiftUI.AnimationContext<A1>) -> A1? where A1: SwiftUI.VectorArithmetic { _skeleton() }
    override func velocity<A1>(value a0: A1, time a1: Swift.Double, context a2: SwiftUI.AnimationContext<A1>) -> A1? where A1: SwiftUI.VectorArithmetic { _skeleton() }
    override func shouldMerge<A1>(previous a0: SwiftUI.Animation, value a1: A1, time a2: Swift.Double, context a3: inout SwiftUI.AnimationContext<A1>) -> Swift.Bool where A1: SwiftUI.VectorArithmetic { _skeleton() }
    deinit {}
}
