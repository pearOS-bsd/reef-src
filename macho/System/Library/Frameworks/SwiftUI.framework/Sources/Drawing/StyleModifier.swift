// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StyleModifier (Drawing).

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

struct StyleModifier<A> {
    let style: A
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    init(style a0: A) { _skeleton() }
}

extension SwiftUI.StyleModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}

extension SwiftUI.StyleModifier: SwiftUI.PrimitiveModifier {
}
