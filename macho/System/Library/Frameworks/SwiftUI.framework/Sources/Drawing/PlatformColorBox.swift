// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PlatformColorBox (Drawing).

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

class PlatformColorBox: SwiftUI.AnyColorBox {
    let color: NSColor
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool {
        guard let other = a0 as? Self else { return false }
        return color == other.color
    }
    override var nsColor: NSColor { color }
    init(_ a0: NSColor) {
        color = a0
        super.init()
    }
    deinit {}
}
