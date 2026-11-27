// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimationBoxBase (Animation).

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

open class AnimationBoxBase {
    var customBase: any SwiftUI.CustomAnimation { _skeleton() }
    func animate<A>(value a0: A, time a1: Swift.Double, context a2: inout SwiftUI.AnimationContext<A>) -> A? where A: SwiftUI.VectorArithmetic { _skeleton() }
    func velocity<A>(value a0: A, time a1: Swift.Double, context a2: SwiftUI.AnimationContext<A>) -> A? where A: SwiftUI.VectorArithmetic { _skeleton() }
    func shouldMerge<A>(previous a0: SwiftUI.Animation, value a1: A, time a2: Swift.Double, context a3: inout SwiftUI.AnimationContext<A>) -> Swift.Bool where A: SwiftUI.VectorArithmetic { _skeleton() }
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
    func isEqual(to a0: SwiftUI.AnimationBoxBase) -> Swift.Bool { _skeleton() }
    init() { _skeleton() }
    deinit {}
}
