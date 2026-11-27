// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollVisibilityModifier (Lists).

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

struct ScrollVisibilityModifier {
    let threshold: Swift.Double
    let action: (Swift.Bool) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ScrollVisibilityModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.ScrollVisibilityModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
