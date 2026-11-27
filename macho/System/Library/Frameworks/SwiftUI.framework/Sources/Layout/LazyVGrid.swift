// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyVGrid (Layout).

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

public struct LazyVGrid<A> where A: SwiftUI.View {
    let columns: [SwiftUI.GridItem]
    let alignment: SwiftUI.HorizontalAlignment
    let spacing: CoreGraphics.CGFloat?
    let content: A
    public init(columns a0: [SwiftUI.GridItem], alignment a1: SwiftUI.HorizontalAlignment, spacing a2: CoreGraphics.CGFloat?, pinnedViews a3: SwiftUI.PinnedScrollableViews, content a4: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.LazyVGrid: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.LazyVGrid: SwiftUI.PrimitiveView {
}
