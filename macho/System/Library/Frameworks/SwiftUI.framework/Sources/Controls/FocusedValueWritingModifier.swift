// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusedValueWritingModifier (Controls).

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

struct FocusedValueWritingModifier {
    var isScene: Swift.Bool
    let write: (inout SwiftUI.FocusedValues) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.FocusedValueWritingModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}

extension SwiftUI.FocusedValueWritingModifier: SwiftUI.PrimitiveModifier {
}
