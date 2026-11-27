// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StyleTextModifier (Text).

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

class StyleTextModifier<A>: SwiftUI.AnyTextModifier where A: SwiftUI.ShapeStyle {
    let style: A = _skeletonValue()
    final func paint(_ a0: SwiftUI.EnvironmentValues) -> SwiftUI.Paint { _skeleton() }
    init(_ a0: A) { _skeleton() }
    deinit {}
}

extension SwiftUI.StyleTextModifier: SwiftUI.AnyStyleTextModifier {
}
