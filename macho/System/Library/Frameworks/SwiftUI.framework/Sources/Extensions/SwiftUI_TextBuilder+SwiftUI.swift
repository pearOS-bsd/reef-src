// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_TextBuilder.

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

enum TextBuilder {
    static func string(_ a0: SwiftUI.Text, _ a1: SwiftUI.EnvironmentValues) -> (NSAttributedString, SwiftUI.TextRunStyle) {
        // the attributed string of a text and the style of its largest run (the environment's when it has none)
        let result = NSMutableAttributedString()
        let style = SwiftUI.TextRunStyle(environment: a1)
        var largest: SwiftUI.TextRunStyle? = nil
        append(a0, style: style, environment: a1, into: result, largest: &largest)
        return (result, largest ?? style)
    }
    static func append(_ a0: SwiftUI.Text, style a1: SwiftUI.TextRunStyle, environment a2: SwiftUI.EnvironmentValues, into a3: NSMutableAttributedString, largest a4: inout SwiftUI.TextRunStyle?) {
        // the text's modifiers applied to the inherited style; concatenations and attributed strings recurse, other
        // storages give their plain string, cased by the environment's text case; every run takes the paragraph of the
        // largest run's line height
        var style = a1
        for modifier in a0.modifiers { style.apply(modifier, environment: a2) }
        var string: Swift.String
        switch a0.storage {
        case .verbatim(let verbatim):
            string = verbatim
        case .anyTextStorage(let storage):
            if let pair = storage as? SwiftUI.ConcatenatedTextStorage {
                append(pair.first, style: style, environment: a2, into: a3, largest: &a4)
                append(pair.second, style: style, environment: a2, into: a3, largest: &a4)
                return
            }
            if let attributed = storage as? SwiftUI.AttributedTextStorage {
                appendAttributed(attributed.attributed, style: style, environment: a2, into: a3, largest: &a4)
                return
            }
            string = storage.plainString
        }
        switch a2.textCase {
        case .uppercase?: string = string.uppercased()
        case .lowercase?: string = string.lowercased()
        case nil: break
        }
        if a4 == nil || style.traits.size > a4!.traits.size { a4 = style }
        let paragraph = paragraph(a2, lineHeight: SwiftUI.FontResolver.lineHeight(a4!.traits.size))
        a3.append(NSAttributedString(string: string, attributes: style.attributes(a2, paragraph: paragraph)))
    }
    static func paragraph(_ a0: SwiftUI.EnvironmentValues, lineHeight a1: CoreGraphics.CGFloat) -> NSParagraphStyle {
        // alignment from the multiline alignment and the layout direction, a fixed line height, the line spacing as both
        // line and paragraph spacing, tightening, and truncation only for head/middle with a line limit of 1
        let paragraph = NSMutableParagraphStyle()
        let rightToLeft = a0.layoutDirection == .rightToLeft
        switch a0.multilineTextAlignment {
        case .leading: paragraph.alignment = rightToLeft ? .right : .left
        case .center: paragraph.alignment = .center
        case .trailing: paragraph.alignment = rightToLeft ? .left : .right
        }
        paragraph.minimumLineHeight = a1
        paragraph.maximumLineHeight = a1
        paragraph.lineSpacing = a0.lineSpacing
        paragraph.paragraphSpacing = a0.lineSpacing
        paragraph.allowsDefaultTighteningForTruncation = a0.allowsTightening
        switch a0.truncationMode {
        case .head: paragraph.lineBreakMode = a0.lineLimit == 1 ? .byTruncatingHead : .byWordWrapping
        case .middle: paragraph.lineBreakMode = a0.lineLimit == 1 ? .byTruncatingMiddle : .byWordWrapping
        case .tail: paragraph.lineBreakMode = .byWordWrapping
        }
        return paragraph
    }
    static func appendAttributed(_ a0: Foundation.AttributedString, style a1: SwiftUI.TextRunStyle, environment a2: SwiftUI.EnvironmentValues, into a3: NSMutableAttributedString, largest a4: inout SwiftUI.TextRunStyle?) {
        // each attribute run of the bridged string on the inherited style: SwiftUI font and colour as text modifiers
        // (an AppKit foreground colour only when there is no SwiftUI one, and only when enabled), kerning, tracking,
        // baseline offset, underline and strikethrough styles, the inline presentation intents (strong: weight 0.4,
        // emphasis: italic, code: monospaced, strikethrough); then the run's text is cased, appended like a plain run
        // and given its background colour (SwiftUI's resolved, else AppKit's)
        let bridged = NSAttributedString(a0)
        let string = bridged.string as NSString
        var runs: [([NSAttributedString.Key: Any], NSRange)] = []
        bridged.enumerateAttributes(in: NSRange(location: 0, length: bridged.length), options: []) { attributes, range, _ in
            runs.append((attributes, range))
        }
        func value(_ key: Swift.String, _ attributes: [NSAttributedString.Key: Any]) -> Any? { attributes[NSAttributedString.Key(key)] }
        for (attributes, range) in runs {
            var style = a1
            if let font = value("SwiftUI.Font", attributes) as? SwiftUI.Font { style.apply(.font(font), environment: a2) }
            if let color = value("SwiftUI.ForegroundColor", attributes) as? SwiftUI.Color {
                style.apply(.color(color), environment: a2)
            } else if let color = value("NSColor", attributes) as? NSColor, a2.isEnabled {
                style.paint = .color(SwiftUI.Color.Resolved(color, colorScheme: a2[SwiftUI.ColorSchemeKey.self]))
            }
            if let kern = value("SwiftUI.Kern", attributes) as? CoreGraphics.CGFloat { style.kerning = kern }
            if let tracking = value("SwiftUI.Tracking", attributes) as? CoreGraphics.CGFloat { style.tracking = tracking }
            if let offset = value("SwiftUI.BaselineOffset", attributes) as? CoreGraphics.CGFloat { style.baselineOffset = offset }
            if let line = value("SwiftUI.UnderlineStyle", attributes) as? SwiftUI.Text.LineStyle { style.underline = (true, line.color) }
            if let line = value("SwiftUI.StrikethroughStyle", attributes) as? SwiftUI.Text.LineStyle { style.strikethrough = (true, line.color) }
            if let number = value("NSInlinePresentationIntent", attributes) as? NSNumber {
                let intent = Foundation.InlinePresentationIntent(rawValue: number.uintValue)
                if intent.contains(.stronglyEmphasized) { style.traits.weight = SwiftUI.Font.Weight(value: 0.4) }
                if intent.contains(.emphasized) { style.traits.italic = true }
                if intent.contains(.code) { style.traits.monospaced = true }
                if intent.contains(.strikethrough) { style.strikethrough = (true, nil) }
            }
            let start = a3.length
            var text = string.substring(with: range)
            switch a2.textCase {
            case .uppercase?: text = text.uppercased()
            case .lowercase?: text = text.lowercased()
            case nil: break
            }
            if a4 == nil || style.traits.size > a4!.traits.size { a4 = style }
            let paragraph = paragraph(a2, lineHeight: SwiftUI.FontResolver.lineHeight(a4!.traits.size))
            a3.append(NSAttributedString(string: text, attributes: style.attributes(a2, paragraph: paragraph)))
            let appended = NSRange(location: start, length: a3.length - start)
            if let color = value("SwiftUI.BackgroundColor", attributes) as? SwiftUI.Color {
                a3.addAttribute(NSAttributedString.Key("NSBackgroundColor"), value: color.provider.resolve(in: a2).nsColor, range: appended)
            } else if let color = value("NSBackgroundColor", attributes) as? NSColor {
                a3.addAttribute(NSAttributedString.Key("NSBackgroundColor"), value: color, range: appended)
            }
        }
    }
}
