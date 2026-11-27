// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BezierAnimation (Animation).

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

struct BezierAnimation {
    let duration: Swift.Double
    let curve: SwiftUI.UnitCurve
    func animate<A>(value a0: A, time a1: Swift.Double, context a2: inout SwiftUI.AnimationContext<A>) -> A? where A: SwiftUI.VectorArithmetic { _skeleton() }
}

extension SwiftUI.BezierAnimation: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.BezierAnimation: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}

extension SwiftUI.BezierAnimation: SwiftUI.CustomAnimation {
}
