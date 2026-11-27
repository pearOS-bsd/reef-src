// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextAttributeModifier (Graph).

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

class TextAttributeModifier: SwiftUI.AnyTextModifier {
    let attribute: SwiftUI.TextAttributeModifier.Attribute = _skeletonValue()
    init(_ a0: SwiftUI.TextAttributeModifier.Attribute) { _skeleton() }
    deinit {}
    enum Attribute {
        case bold(Swift.Bool)
        case italic(Swift.Bool)
        case weight(SwiftUI.Font.Weight?)
        case design(SwiftUI.Font.Design?)
        case width(SwiftUI.Font.Width?)
        case kerning(CoreGraphics.CGFloat)
        case tracking(CoreGraphics.CGFloat)
        case underline(Swift.Bool, SwiftUI.Color?)
        case strikethrough(Swift.Bool, SwiftUI.Color?)
        case baselineOffset(CoreGraphics.CGFloat)
        case monospaced(Swift.Bool)
        case scale(CoreGraphics.CGFloat)
        case monospacedDigit
    }
}
