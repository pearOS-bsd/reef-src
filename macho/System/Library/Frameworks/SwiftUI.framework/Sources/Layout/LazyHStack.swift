// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyHStack (Layout).

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

public struct LazyHStack<A> where A: SwiftUI.View {
    let tree: SwiftUI._VariadicView.Tree<SwiftUI._HStackLayout, A>
    let pinnedViews: SwiftUI.PinnedScrollableViews
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(alignment a0: SwiftUI.VerticalAlignment, spacing a1: CoreGraphics.CGFloat?, pinnedViews a2: SwiftUI.PinnedScrollableViews, content a3: () -> A) { _skeleton() }
}

extension SwiftUI.LazyHStack: SwiftUI.PrimitiveView {
}

extension SwiftUI.LazyHStack: SwiftUI.View {
    public typealias Body = Swift.Never
}
