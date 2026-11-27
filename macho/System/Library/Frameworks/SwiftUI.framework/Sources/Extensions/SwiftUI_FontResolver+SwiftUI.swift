// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_FontResolver.

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

/// Fonts and line metrics of the font traits a text resolved to.
enum FontResolver {
    /// distance between baselines of a font size: the measured values of the system font's text styles, else
    /// 1.23 times the size, rounded
    static func lineAdvance(_ a0: CoreGraphics.CGFloat) -> CoreGraphics.CGFloat {
        let known: [CoreGraphics.CGFloat: CoreGraphics.CGFloat] =
            [10: 13, 11: 14, 12: 15, 13: 16, 15: 20, 17: 22, 22: 26, 26: 32]
        if let advance = known[a0] { return advance }
        return (a0 * 1.23).rounded()
    }

    /// line height of a font size: the measured values of the text styles, else 1.2 times the size rounded up to a
    /// half point
    static func lineHeight(_ a0: CoreGraphics.CGFloat) -> CoreGraphics.CGFloat {
        let known: [CoreGraphics.CGFloat: CoreGraphics.CGFloat] =
            [10: 13, 11: 14, 12: 15, 13: 16, 15: 19, 17: 20.5, 22: 26, 26: 31]
        if let height = known[a0] { return height }
        return ((a0 * 1.2) * 2).rounded(.up) * 0.5
    }

    /// the font: the named font when there is one, else the (monospaced) system font of the weight; italic through
    /// the font manager
    static func nsFont(_ a0: SwiftUI.FontTraits) -> NSFont {
        let weight = NSFont.Weight(a0.weight?.value ?? 0)
        var font: NSFont
        if let name = a0.name, let named = NSFont(name: name, size: a0.size) {
            font = named
        } else if a0.design == .monospaced || a0.monospaced {
            font = NSFont.monospacedSystemFont(ofSize: a0.size, weight: weight)
        } else {
            font = NSFont.systemFont(ofSize: a0.size, weight: weight)
        }
        if a0.italic { font = NSFontManager.shared.convert(font, toHaveTrait: .italicFontMask) }
        return font
    }
}
