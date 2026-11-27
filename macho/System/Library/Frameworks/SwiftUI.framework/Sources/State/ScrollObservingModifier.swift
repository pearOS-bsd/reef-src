// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollObservingModifier (State).

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

struct ScrollObservingModifier {
    let observer: any SwiftUI.ScrollObserving
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    static func firstScroll(`in` a0: [SwiftUI.Element]) -> SwiftUI.ScrollElement? { _skeleton() }
}

extension SwiftUI.ScrollObservingModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.ScrollObservingModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
