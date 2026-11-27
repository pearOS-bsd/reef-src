// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VStack (Layout).

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

public struct VStack<A> where A: SwiftUI.View {
    public var _tree: SwiftUI._VariadicView.Tree<SwiftUI._VStackLayout, A>
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _tree._buildElements(a0) }
    public init(alignment a0: SwiftUI.HorizontalAlignment, spacing a1: CoreGraphics.CGFloat?, @SwiftUI.ViewBuilder content a2: () -> A) {
        _tree = SwiftUI._VariadicView.Tree(root: SwiftUI._VStackLayout(alignment: a0, spacing: a1), content: a2())
    }
}

extension SwiftUI.VStack: SwiftUI.PrimitiveView {
}

extension SwiftUI.VStack: SwiftUI.View {
    public typealias Body = Swift.Never
}
