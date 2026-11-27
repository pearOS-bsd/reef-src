// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextElement (Text).

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

class TextElement: SwiftUI.Element {
    let string: NSAttributedString
    let style: SwiftUI.TextRunStyle
    let lineLimit: Swift.Int?
    let minimumLines: Swift.Int?
    let lineSpacing: CoreGraphics.CGFloat
    let scale: CoreGraphics.CGFloat
    let minimumScaleFactor: CoreGraphics.CGFloat
    /// the width of the string on one line
    lazy var idealWidth: CoreGraphics.CGFloat = string.boundingRect(
        with: CGSize(width: CoreGraphics.CGFloat.greatestFiniteMagnitude, height: .greatestFiniteMagnitude),
        options: .usesLineFragmentOrigin, context: nil).width
    /// the width of the first character followed by an ellipsis (in the first run's attributes)
    lazy var truncatedWidth: CoreGraphics.CGFloat = {
        let first = (string.string as NSString).rangeOfComposedCharacterSequence(at: 0)
        let truncated = string.attributedSubstring(from: first).mutableCopy() as! NSMutableAttributedString
        truncated.append(NSAttributedString(string: "\u{2026}", attributes: string.attributes(at: 0, effectiveRange: nil)))
        return truncated.size().width
    }()
    override func draw(_ a0: SwiftUI.RenderContext) {
        // the string drawn in its drawing rectangle, clipped to the frame when it is truncated
        guard string.length > 0 else { return }
        NSGraphicsContext.saveGraphicsState()
        let (rect, options, clips) = drawing()
        if clips { NSBezierPath(rect: frame).addClip() }
        string.draw(with: rect, options: options, context: nil)
        NSGraphicsContext.restoreGraphicsState()
    }
    init(text a0: SwiftUI.Text, environment a1: SwiftUI.EnvironmentValues) {
        // the string with one paragraph style over all of it, whose line height is the largest run's line advance
        // moved by the leading (tight/loose: 2/13 of the size, rounded)
        let (built, style) = SwiftUI.TextBuilder.string(a0, a1)
        let size = style.traits.size
        var lineHeight = SwiftUI.FontResolver.lineAdvance(size)
        switch style.traits.leading {
        case .tight?: lineHeight -= (size * 2 / 13).rounded()
        case .loose?: lineHeight += (size * 2 / 13).rounded()
        default: break
        }
        let paragraph = SwiftUI.TextBuilder.paragraph(a1, lineHeight: lineHeight)
        let string = NSMutableAttributedString(attributedString: built)
        if string.length > 0 { string.addAttribute(.paragraphStyle, value: paragraph, range: NSRange(location: 0, length: string.length)) }
        self.string = string
        self.style = style
        lineLimit = a1.lineLimit
        minimumLines = a1.lineLimitMinimum
        lineSpacing = a1.lineSpacing
        scale = max(a1.displayScale, 1)
        minimumScaleFactor = a1.minimumScaleFactor
        super.init()
    }
    override var hasTextBaseline: Swift.Bool { true }
    override var spacingKind: SwiftUI.SpacingKind { .text(style.traits.size) }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize {
        // the string laid out in the proposed width, as many lines as it takes, limited by the line limit and the
        // proposed height, at least the minimum lines; rounded up to device pixels
        if string.length == 0 { return .zero }
        let width = a0.width ?? .greatestFiniteMagnitude
        let bounds = string.boundingRect(with: CGSize(width: max(1, width), height: .greatestFiniteMagnitude),
                                         options: .usesLineFragmentOrigin, context: nil)
        let size = style.traits.size
        var lineAdvance = SwiftUI.FontResolver.lineAdvance(size)
        switch style.traits.leading {
        case .tight?: lineAdvance -= (size * 2 / 13).rounded()
        case .loose?: lineAdvance += (size * 2 / 13).rounded()
        default: break
        }
        let advance = lineAdvance + lineSpacing
        let natural = max(((bounds.height + lineSpacing) / advance).rounded(), 1)
        var lines = natural
        if let lineLimit, lineLimit > 0 { lines = min(CoreGraphics.CGFloat(lineLimit), natural) }
        if let height = a0.height, height.isFinite {
            let fit = ((height - SwiftUI.FontResolver.lineHeight(size) + 0.5) / advance).rounded(.down) + 1
            lines = min(fit >= 1 ? fit : 1, lines)
        }
        if let minimumLines, lines <= CoreGraphics.CGFloat(minimumLines) { lines = CoreGraphics.CGFloat(minimumLines) }
        let height = SwiftUI.FontResolver.lineHeight(size) + (lines - 1) * advance
        var measured = bounds.width
        if a0.width != nil && width < bounds.width { measured = min(width, bounds.width) }
        return CGSize(width: (measured * scale - 0.001).rounded(.up) / scale, height: (height * scale - 0.001).rounded(.up) / scale)
    }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { allowsHitTesting && !isHidden && string.length > 0 && frame.contains(a0) }
    override var drawsOwnContent: Swift.Bool { true }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool {
        // the same class exactly, the same limits, spacing, scales, run style and string
        guard type(of: a0) == SwiftUI.TextElement.self, let other = a0 as? SwiftUI.TextElement else { return false }
        if other === self { return true }
        return other.lineLimit == lineLimit && other.minimumLines == minimumLines && other.lineSpacing == lineSpacing
            && other.scale == scale && other.minimumScaleFactor == minimumScaleFactor
            && SwiftUI.GraphCompare.viewEqual(other.style, style) && other.string.isEqual(to: string)
    }
    final func drawing() -> (rect: CGRect, options: NSString.DrawingOptions, clips: Swift.Bool) {
        // truncation when the string is wider than the frame (half a point of slack) or has a line limit, but not when
        // even one character and the ellipsis do not fit (then clipped); a width a little over the frame's is kept;
        // the rectangle starts half the difference between line advance and line height above the frame
        let ideal = idealWidth
        var options: NSString.DrawingOptions = ideal > frame.width + 0.5 || lineLimit != nil ? [.usesLineFragmentOrigin, .truncatesLastVisibleLine] : .usesLineFragmentOrigin
        let clips = truncatedWidth > frame.width + 0.5
        if clips { options = .usesLineFragmentOrigin }
        var width = max(1, frame.width)
        if options == .usesLineFragmentOrigin && ideal > width && ideal <= width + 0.5 { width = ideal }
        if case .linear = style.paint { return (CGRect(x: frame.minX, y: frame.minY, width: width, height: frame.height), options, clips) }
        let size = style.traits.size
        var advance = SwiftUI.FontResolver.lineAdvance(size)
        switch style.traits.leading {
        case .tight?: advance -= (size * 2 / 13).rounded()
        case .loose?: advance += (size * 2 / 13).rounded()
        default: break
        }
        let lineHeight = SwiftUI.FontResolver.lineHeight(size)
        let y = frame.minY + (advance - lineHeight) * -0.5
        return (CGRect(x: frame.minX, y: y, width: width, height: frame.height + (advance - lineHeight) + 1), options, clips)
    }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? {
        // baselines: the ascender below half the space the line height leaves around the font's height
        func firstBaseline() -> CoreGraphics.CGFloat {
            let font = SwiftUI.FontResolver.nsFont(style.traits)
            let lineHeight = SwiftUI.FontResolver.lineHeight(style.traits.size)
            return (lineHeight - (font.ascender - font.descender)) * 0.5 + font.ascender
        }
        if a0 == SwiftUI.VerticalAlignment.firstTextBaseline.key { return firstBaseline() }
        if a0 == SwiftUI.VerticalAlignment.lastTextBaseline.key {
            return a1.height - SwiftUI.FontResolver.lineHeight(style.traits.size) + firstBaseline()
        }
        return nil
    }
    deinit {}
}
