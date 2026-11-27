// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LabeledControlGroupContent (Text).

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

public struct LabeledControlGroupContent<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let content: A
    let label: B
    init(content a0: A, label a1: B) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.LabeledControlGroupContent: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.LabeledControlGroupContent: SwiftUI.PrimitiveView {
}
