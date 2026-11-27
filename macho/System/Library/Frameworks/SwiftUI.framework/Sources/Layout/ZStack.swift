// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ZStack (Layout).

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

public struct ZStack<A> where A: SwiftUI.View {
    public var _tree: SwiftUI._VariadicView.Tree<SwiftUI._ZStackLayout, A>
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _tree._buildElements(a0) }
    public init(alignment a0: SwiftUI.Alignment, @SwiftUI.ViewBuilder content a1: () -> A) {
        _tree = SwiftUI._VariadicView.Tree(root: SwiftUI._ZStackLayout(alignment: a0), content: a1())
    }
}

extension SwiftUI.ZStack: SwiftUI.PrimitiveView {
}

extension SwiftUI.ZStack: SwiftUI.View {
    public typealias Body = Swift.Never
}
