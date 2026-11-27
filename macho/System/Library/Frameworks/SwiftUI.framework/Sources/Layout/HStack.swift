// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HStack (Layout).

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

public struct HStack<A> where A: SwiftUI.View {
    public var _tree: SwiftUI._VariadicView.Tree<SwiftUI._HStackLayout, A>
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(alignment a0: SwiftUI.VerticalAlignment, spacing a1: CoreGraphics.CGFloat?, content a2: () -> A) { _skeleton() }
}

extension SwiftUI.HStack: SwiftUI.PrimitiveView {
}

extension SwiftUI.HStack: SwiftUI.View {
    public typealias Body = Swift.Never
}
