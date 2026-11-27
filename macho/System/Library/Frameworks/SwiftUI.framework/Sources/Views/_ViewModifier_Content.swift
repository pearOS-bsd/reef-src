// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ViewModifier_Content (Views).

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

public struct _ViewModifier_Content<A> where A: SwiftUI.ViewModifier {
    init() {}
}

extension SwiftUI._ViewModifier_Content: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI._ViewModifier_Content: SwiftUI.PrimitiveView {
    /// the content the modifier's body wraps: what ViewBuild.apply put into the context for modifier A
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        guard let content = a0.modifierContents[ObjectIdentifier(A.self)] else { return [] }
        return content(a0)
    }
}
