// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextEditorStyle (Text).

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

public protocol TextEditorStyle {
}

extension SwiftUI.TextEditorStyle where Self == SwiftUI.PlainTextEditorStyle {
    public static var plain: SwiftUI.PlainTextEditorStyle { _skeleton() }
}

extension SwiftUI.TextEditorStyle where Self == SwiftUI.AutomaticTextEditorStyle {
    public static var automatic: SwiftUI.AutomaticTextEditorStyle { _skeleton() }
}
