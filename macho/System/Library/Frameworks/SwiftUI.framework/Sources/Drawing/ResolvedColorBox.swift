// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ResolvedColorBox (Drawing).

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

class ResolvedColorBox: SwiftUI.AnyColorBox {
    let red: Swift.Double
    let green: Swift.Double
    let blue: Swift.Double
    let opacity: Swift.Double
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool {
        guard let other = a0 as? Self else { return false }
        return red == other.red && green == other.green && blue == other.blue && opacity == other.opacity
    }
    override func hash(into a0: inout Swift.Hasher) {
        a0.combine(red)
        a0.combine(green)
        a0.combine(blue)
        a0.combine(opacity)
    }
    override var nsColor: NSColor { NSColor(srgbRed: red, green: green, blue: blue, alpha: opacity) }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved {
        // sRGB components made linear, the opacity as it is
        func linear(_ v: Swift.Double) -> Swift.Float {
            let f = Float(v)
            return f <= 0.04045 ? f / 12.92 : powf((f + 0.055) / 1.055, 2.4)
        }
        return SwiftUI.Color.Resolved(linearRed: linear(red), linearGreen: linear(green), linearBlue: linear(blue), opacity: Float(opacity))
    }
    init(red a0: Swift.Double, green a1: Swift.Double, blue a2: Swift.Double, opacity a3: Swift.Double) {
        red = a0
        green = a1
        blue = a2
        opacity = a3
        super.init()
    }
    deinit {}
}
