// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SystemColorBox (Drawing).

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

class SystemColorBox: SwiftUI.AnyColorBox {
    let light: (red: Swift.Double, green: Swift.Double, blue: Swift.Double)
    let dark: (red: Swift.Double, green: Swift.Double, blue: Swift.Double)
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool {
        guard let other = a0 as? Self else { return false }
        return light == other.light && dark == other.dark
    }
    override func hash(into a0: inout Swift.Hasher) {
        // the light components only
        a0.combine(light.red)
        a0.combine(light.green)
        a0.combine(light.blue)
    }
    override var nsColor: NSColor {
        // the components of the scheme of the current drawing appearance, opaque
        let c = SwiftUI.SchemeAppearance.scheme(of: NSAppearance.currentDrawing()) == .dark ? dark : light
        return NSColor(srgbRed: c.red / 255, green: c.green / 255, blue: c.blue / 255, alpha: 1)
    }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved {
        // the components of the environment's colour scheme, made linear, opaque
        let c = a0[SwiftUI.ColorSchemeKey.self] == .dark ? dark : light
        func linear(_ v: Swift.Double) -> Swift.Float {
            let f = Float(v / 255)
            return f <= 0.04045 ? f / 12.92 : powf((f + 0.055) / 1.055, 2.4)
        }
        return SwiftUI.Color.Resolved(linearRed: linear(c.red), linearGreen: linear(c.green), linearBlue: linear(c.blue), opacity: 1)
    }
    init(light a0: (red: Swift.Double, green: Swift.Double, blue: Swift.Double), dark a1: (red: Swift.Double, green: Swift.Double, blue: Swift.Double)) {
        light = a0
        dark = a1
        super.init()
    }
    deinit {}
}
