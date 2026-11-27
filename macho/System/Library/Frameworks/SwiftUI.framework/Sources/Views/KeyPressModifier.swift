// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyPressModifier (Views).

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

struct KeyPressModifier {
    let phases: SwiftUI.KeyPress.Phases
    let action: (SwiftUI.KeyPress) -> SwiftUI.KeyPress.Result
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.KeyPressModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.KeyPressModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
