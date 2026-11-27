// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GestureModifier (Gestures).

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

struct GestureModifier<A> where A: SwiftUI.Gesture {
    let gesture: A
    var simultaneous: Swift.Bool
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    init(gesture a0: A, simultaneous a1: Swift.Bool) { _skeleton() }
}

extension SwiftUI.GestureModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.GestureModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
