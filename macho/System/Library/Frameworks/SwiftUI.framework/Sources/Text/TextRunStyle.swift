// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextRunStyle (Text).

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

struct TextRunStyle {
    var traits: SwiftUI.FontTraits
    var paint: SwiftUI.Paint
    var kerning: CoreGraphics.CGFloat
    var tracking: CoreGraphics.CGFloat
    var underline: (Swift.Bool, SwiftUI.Color?)
    var strikethrough: (Swift.Bool, SwiftUI.Color?)
    var baselineOffset: CoreGraphics.CGFloat
    init(environment a0: SwiftUI.EnvironmentValues) {
        // the font's traits (Font.body when none, at the control size's size: mini 9, small 11, else 13), then the text
        // modifiers of the environment (bold, italic, monospaced digits, weight, design, scale of the base size)
        var traits = SwiftUI.FontTraits(size: 13, weight: nil, italic: false, design: nil, monospaced: false, monospacedDigit: false,
                                        name: nil, width: nil, leading: nil)
        var base: CoreGraphics.CGFloat
        if let font = a0[SwiftUI.FontKey.self] {
            font.provider.apply(to: &traits)
            base = traits.size
        } else {
            SwiftUI.Font.body.provider.apply(to: &traits)
            switch SwiftUI.ControlSizes(a0).size {
            case .mini: base = 9
            case .small: base = 11
            default: base = 13
            }
            traits.size = base
        }
        if a0.textBold { traits.weight = .bold }
        if a0.textItalic { traits.italic = true }
        if a0.textMonospacedDigit { traits.monospacedDigit = true }
        if let weight = a0.textWeight { traits.weight = weight }
        if let design = a0.textDesign { traits.design = design }
        if let scale = a0[SwiftUI.TextScaleKey.self] { traits.size = (base * scale.factor).rounded() }
        self.traits = traits
        paint = SwiftUI.PaintResolver.foreground(a0, level: 0)
        if !a0.isEnabled && a0.dimsDisabledText { paint = SwiftUI.PaintResolver.label(2, a0) }
        kerning = 0
        tracking = 0
        underline = (false, nil)
        strikethrough = (false, nil)
        baselineOffset = 0
    }
    mutating func apply(_ a0: SwiftUI.Text.Modifier, environment a1: SwiftUI.EnvironmentValues) {
        switch a0 {
        case .color(let color):
            // a colour of a disabled text is not applied
            if let color, a1.isEnabled { paint = .color(color.provider.resolve(in: a1)) }
        case .font(let font):
            // the font's traits replace the run's, the weight stays when the font has none
            guard let font else { return }
            var fresh = SwiftUI.FontTraits(size: 13, weight: nil, italic: false, design: nil, monospaced: false,
                                           monospacedDigit: false, name: nil, width: nil, leading: nil)
            font.provider.apply(to: &fresh)
            let weight = traits.weight
            traits = fresh
            if fresh.weight == nil { traits.weight = weight }
        case .anyTextModifier(let modifier):
            if let style = modifier as? any SwiftUI.AnyStyleTextModifier {
                if a1.isEnabled { paint = style.paint(a1) }
                return
            }
            guard let attribute = (modifier as? SwiftUI.TextAttributeModifier)?.attribute else { return }
            switch attribute {
            case .bold(let on): if on { traits.weight = .bold }
            case .italic(let on): traits.italic = on
            case .weight(let weight): traits.weight = weight
            case .design(let design): traits.design = design
            case .width(let width): traits.width = width
            case .kerning(let value): kerning = value
            case .tracking(let value): tracking = value
            case .underline(let on, let color): underline = (on, color)
            case .strikethrough(let on, let color): strikethrough = (on, color)
            case .baselineOffset(let value): baselineOffset = value
            case .monospaced(let on): traits.monospaced = on
            case .scale(let factor): traits.size = (factor * traits.size).rounded()
            case .monospacedDigit: traits.monospacedDigit = true
            }
        }
    }
    func attributes(_ a0: SwiftUI.EnvironmentValues, paragraph a1: NSParagraphStyle) -> [NSAttributedString.Key: Any] {
        // font, paragraph, the paint's colour (the first stop of a linear gradient, else clear), kerning plus tracking,
        // single underline/strikethrough with their colours, baseline offset
        var attributes: [NSAttributedString.Key: Any] = [.font: SwiftUI.FontResolver.nsFont(traits), .paragraphStyle: a1]
        switch paint {
        case .color(let color): attributes[.foregroundColor] = color.nsColor
        case .linear(let stops, _, _) where !stops.isEmpty: attributes[.foregroundColor] = stops[0].0.nsColor
        default: attributes[.foregroundColor] = NSColor.clear
        }
        if kerning != 0 || tracking != 0 { attributes[.kern] = kerning + tracking }
        if underline.0 {
            attributes[.underlineStyle] = 1
            if let color = underline.1 { attributes[.underlineColor] = color.provider.resolve(in: a0).nsColor }
        }
        if strikethrough.0 {
            attributes[.strikethroughStyle] = 1
            if let color = strikethrough.1 { attributes[.strikethroughColor] = color.provider.resolve(in: a0).nsColor }
        }
        if baselineOffset != 0 { attributes[.baselineOffset] = baselineOffset }
        return attributes
    }
}
