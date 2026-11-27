// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_Oklab.

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

struct Oklab {
    var L: Swift.Float
    var a: Swift.Float
    var b: Swift.Float
    init(L: Swift.Float, a: Swift.Float, b: Swift.Float) {
        self.L = L
        self.a = a
        self.b = b
    }
    /// the Oklab value of a linear sRGB colour
    init(_ c: SwiftUI.Color.Resolved) {
        let l = cbrtf(0.4122214615 * c.linearRed + 0.5363325477 * c.linearGreen + 0.0514459945 * c.linearBlue)
        let m = cbrtf(0.2119034976 * c.linearRed + 0.6806995272 * c.linearGreen + 0.1073969603 * c.linearBlue)
        let s = cbrtf(0.0883024633 * c.linearRed + 0.2817188501 * c.linearGreen + 0.6299787163 * c.linearBlue)
        L = 0.2104542553 * l + 0.7936177850 * m - 0.0040720468 * s
        a = 1.9779984951 * l - 2.4285922050 * m + 0.4505937099 * s
        b = 0.0259040371 * l + 0.7827717662 * m - 0.8086757660 * s
    }
    /// back to linear sRGB, each component clamped to 0...1, opaque
    var resolved: SwiftUI.Color.Resolved {
        let l_ = L + 0.3963377774 * a + 0.2158037573 * b
        let m_ = L - 0.1055613458 * a - 0.0638541728 * b
        let s_ = L - 0.0894841775 * a - 1.2914855480 * b
        let l = l_ * l_ * l_, m = m_ * m_ * m_, s = s_ * s_ * s_
        func clamp(_ v: Swift.Float) -> Swift.Float { v <= 0 ? 0 : min(1, v) }
        return SwiftUI.Color.Resolved(linearRed: clamp(4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s),
                                      linearGreen: clamp(-1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s),
                                      linearBlue: clamp(-0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s), opacity: 1)
    }
}
