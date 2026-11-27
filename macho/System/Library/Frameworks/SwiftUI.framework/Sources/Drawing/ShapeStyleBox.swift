// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShapeStyleBox (Drawing).

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

class ShapeStyleBox {
    let style: Any
    let secondary: Any?
    let tertiary: Any?
    final func paint(_ a0: SwiftUI.EnvironmentValues, level a1: Swift.Int) -> SwiftUI.Paint { _skeleton() }
    init(_ a0: Any, secondary a1: Any?, tertiary a2: Any?) { style = a0; secondary = a1; tertiary = a2 }
    deinit {}
}
