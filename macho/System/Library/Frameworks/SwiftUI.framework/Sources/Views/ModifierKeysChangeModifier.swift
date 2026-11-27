// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ModifierKeysChangeModifier (Views).

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

struct ModifierKeysChangeModifier {
    let mask: SwiftUI.EventModifiers
    let initial: Swift.Bool
    let action: (SwiftUI.EventModifiers, SwiftUI.EventModifiers) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ModifierKeysChangeModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.ModifierKeysChangeModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
