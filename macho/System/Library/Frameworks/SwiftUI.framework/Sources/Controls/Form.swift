// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Form (Controls).

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

public struct Form<A> where A: SwiftUI.View {
    let content: A
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(content a0: () -> A) { _skeleton() }
}

extension SwiftUI.Form: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Form: SwiftUI.PrimitiveView {
}
