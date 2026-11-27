// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_PaintResolver.

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

enum PaintResolver {
    static func resolve<A>(_ a0: A, _ a1: SwiftUI.EnvironmentValues, level a2: Swift.Int) -> SwiftUI.Paint where A: SwiftUI.ShapeStyle { _skeleton() }
    static func label(_ a0: Swift.Int, _ a1: SwiftUI.EnvironmentValues) -> SwiftUI.Paint { _skeleton() }
    static func stops(_ a0: SwiftUI.Gradient, _ a1: SwiftUI.EnvironmentValues) -> [(SwiftUI.Color.Resolved, CoreGraphics.CGFloat)] { _skeleton() }
    static func backgroundLevel(_ a0: Swift.Int, _ a1: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved { _skeleton() }
    static func materialPaint(_ a0: SwiftUI.Material, _ a1: SwiftUI.EnvironmentValues) -> SwiftUI.Paint { _skeleton() }
    static func foreground(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
}
