// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OpacityColorBox (Drawing).

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

class OpacityColorBox: SwiftUI.AnyColorBox {
    let base: SwiftUI.AnyColorBox
    let opacity: Swift.Double
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool {
        guard let other = a0 as? Self else { return false }
        return base.isEqual(to: other.base) && opacity == other.opacity
    }
    override var nsColor: NSColor {
        let color = base.nsColor
        return color.withAlphaComponent(base.nsColor.alphaComponent * opacity)
    }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved {
        var resolved = base.resolve(in: a0)
        resolved.opacity *= Float(opacity)
        return resolved
    }
    init(base a0: SwiftUI.AnyColorBox, opacity a1: Swift.Double) {
        base = a0
        opacity = a1
        super.init()
    }
    deinit {}
}
