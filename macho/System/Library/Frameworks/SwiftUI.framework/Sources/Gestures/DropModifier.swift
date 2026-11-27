// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DropModifier (Gestures).

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

struct DropModifier {
    let types: [UniformTypeIdentifiers.UTType]
    let delegate: any SwiftUI.DropDelegate
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.DropModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.DropModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
