// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollPositionModifier (Layout).

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

struct ScrollPositionModifier {
    let position: SwiftUI.Binding<SwiftUI.ScrollPosition>
    let anchor: SwiftUI.UnitPoint?
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    class Applied {
        var position: SwiftUI.ScrollPosition? = _skeletonValue()
        init() { _skeleton() }
        deinit {}
    }
}

extension SwiftUI.ScrollPositionModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.ScrollPositionModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
