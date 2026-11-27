// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MixedColorBox (Drawing).

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

class MixedColorBox: SwiftUI.AnyColorBox {
    let base: SwiftUI.AnyColorBox
    let white: Swift.Double
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool {
        // the base only (the white amount is not compared)
        guard let other = a0 as? Self else { return false }
        return base.isEqual(to: other.base)
    }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved {
        // each sRGB component moved 15 % of the way to white (the stored amount is not used), opacity kept
        let c = base.resolve(in: a0)
        func mixed(_ v: Swift.Float) -> Swift.Float {
            let e = SwiftUI.Color.Resolved.encoded(v)
            return SwiftUI.Color.Resolved.linear(e + (1 - e) * 0.15)
        }
        return SwiftUI.Color.Resolved(linearRed: mixed(c.linearRed), linearGreen: mixed(c.linearGreen), linearBlue: mixed(c.linearBlue), opacity: c.opacity)
    }
    init(base a0: SwiftUI.AnyColorBox, white a1: Swift.Double) {
        base = a0
        white = a1
        super.init()
    }
    deinit {}
}
