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
    init(environment a0: SwiftUI.EnvironmentValues) { _skeleton() }
    func apply(_ a0: SwiftUI.Text.Modifier, environment a1: SwiftUI.EnvironmentValues) { _skeleton() }
    func attributes(_ a0: SwiftUI.EnvironmentValues, paragraph a1: NSParagraphStyle) -> [NSAttributedString.Key: Any] { _skeleton() }
}
