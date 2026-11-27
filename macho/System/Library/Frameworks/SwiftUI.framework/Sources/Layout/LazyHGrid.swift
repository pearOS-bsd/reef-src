// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyHGrid (Layout).

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

public struct LazyHGrid<A> where A: SwiftUI.View {
    let rows: [SwiftUI.GridItem]
    let alignment: SwiftUI.VerticalAlignment
    let spacing: CoreGraphics.CGFloat?
    let content: A
    public init(rows a0: [SwiftUI.GridItem], alignment a1: SwiftUI.VerticalAlignment, spacing a2: CoreGraphics.CGFloat?, pinnedViews a3: SwiftUI.PinnedScrollableViews, content a4: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.LazyHGrid: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.LazyHGrid: SwiftUI.PrimitiveView {
}
