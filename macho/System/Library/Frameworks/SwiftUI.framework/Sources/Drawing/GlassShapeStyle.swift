// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GlassShapeStyle (Drawing).

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

struct GlassShapeStyle {
    let glass: SwiftUI.Glass
    func paint(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
}

extension SwiftUI.GlassShapeStyle: SwiftUI.ResolvableShapeStyle {
}

extension SwiftUI.GlassShapeStyle: SwiftUI.ShapeStyle {
    typealias Resolved = Swift.Never
}
