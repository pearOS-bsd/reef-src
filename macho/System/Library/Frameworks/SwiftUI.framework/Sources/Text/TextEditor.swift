// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextEditor (Text).

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

public struct TextEditor {
    let text: SwiftUI.Binding<Swift.String>
    public init(text a0: SwiftUI.Binding<Swift.String>) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.TextEditor: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.TextEditor: SwiftUI.PrimitiveView {
}
