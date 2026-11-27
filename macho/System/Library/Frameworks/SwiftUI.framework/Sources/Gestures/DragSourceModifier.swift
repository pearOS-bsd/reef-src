// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DragSourceModifier (Gestures).

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

struct DragSourceModifier {
    let data: () -> NSItemProvider
    let preview: SwiftUI.AnyView?
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.DragSourceModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.DragSourceModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
