// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HSplitView (Navigation).

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

public struct HSplitView<A> where A: SwiftUI.View {
    let content: A
    public init(content a0: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.HSplitView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.HSplitView: SwiftUI.PrimitiveView {
}
