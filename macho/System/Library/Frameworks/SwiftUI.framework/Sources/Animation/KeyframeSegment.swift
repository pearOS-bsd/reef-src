// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeSegment (Animation).

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

struct KeyframeSegment<A> where A: SwiftUI.Animatable {
    let to: A
    let duration: Swift.Double
    let curve: SwiftUI.KeyframeSegment<A>.Curve
    static func value(_ a0: [SwiftUI.KeyframeSegment<A>], from a1: A, at a2: Swift.Double) -> A { _skeleton() }
    static func mix<A1>(_ a0: A1, _ a1: A1, _ a2: Swift.Double) -> A1 where A1: SwiftUI.VectorArithmetic { _skeleton() }
    static func catmullRom<A1>(_ a0: A1, _ a1: A1, _ a2: A1, _ a3: A1, _ a4: Swift.Double) -> A1 where A1: SwiftUI.VectorArithmetic { _skeleton() }
    init(to a0: A, duration a1: Swift.Double, curve a2: SwiftUI.KeyframeSegment<A>.Curve) { _skeleton() }
    enum Curve {
        case linear(SwiftUI.UnitCurve)
        case spring(SwiftUI.Spring)
        case cubic
        case move
    }
}
