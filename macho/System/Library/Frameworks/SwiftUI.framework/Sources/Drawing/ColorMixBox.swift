// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ColorMixBox (Drawing).

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

class ColorMixBox: SwiftUI.AnyColorBox {
    let base: SwiftUI.AnyColorBox
    let other: SwiftUI.AnyColorBox
    let fraction: Swift.Double
    let colorSpace: SwiftUI.Gradient.ColorSpace
    override func isEqual(to a0: SwiftUI.AnyColorBox) -> Swift.Bool {
        guard let o = a0 as? Self else { return false }
        return base.isEqual(to: o.base) && other.isEqual(to: o.other) && fraction == o.fraction && colorSpace == o.colorSpace
    }
    override func resolve(`in` a0: SwiftUI.EnvironmentValues) -> SwiftUI.Color.Resolved {
        // the fraction clamped to 0...1; perceptual: between the Oklab values, device: between the sRGB components;
        // the opacity between the two opacities
        let a = base.resolve(in: a0), b = other.resolve(in: a0)
        let t = Float(fraction <= 0 ? 0 : min(1, fraction))
        var mixed: SwiftUI.Color.Resolved
        if colorSpace == .perceptual {
            let la = SwiftUI.Oklab(a), lb = SwiftUI.Oklab(b)
            mixed = SwiftUI.Oklab(L: la.L + (lb.L - la.L) * t, a: la.a + (lb.a - la.a) * t, b: la.b + (lb.b - la.b) * t).resolved
        } else {
            func mix(_ x: Swift.Float, _ y: Swift.Float) -> Swift.Float {
                let ex = SwiftUI.Color.Resolved.encoded(x), ey = SwiftUI.Color.Resolved.encoded(y)
                return SwiftUI.Color.Resolved.linear(ex + (ey - ex) * t)
            }
            mixed = SwiftUI.Color.Resolved(linearRed: mix(a.linearRed, b.linearRed), linearGreen: mix(a.linearGreen, b.linearGreen), linearBlue: mix(a.linearBlue, b.linearBlue), opacity: 1)
        }
        mixed.opacity = a.opacity + (b.opacity - a.opacity) * t
        return mixed
    }
    init(base a0: SwiftUI.AnyColorBox, other a1: SwiftUI.AnyColorBox, fraction a2: Swift.Double, colorSpace a3: SwiftUI.Gradient.ColorSpace) {
        base = a0
        other = a1
        fraction = a2
        colorSpace = a3
        super.init()
    }
    deinit {}
}
