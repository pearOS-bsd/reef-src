// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Grid (Layout).

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

public struct Grid<A> where A: SwiftUI.View {
    public var _tree: SwiftUI._VariadicView.Tree<SwiftUI.GridLayout, A>
    public init(alignment a0: SwiftUI.Alignment, horizontalSpacing a1: CoreGraphics.CGFloat?, verticalSpacing a2: CoreGraphics.CGFloat?, content a3: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Grid: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Grid: SwiftUI.PrimitiveView {
}
