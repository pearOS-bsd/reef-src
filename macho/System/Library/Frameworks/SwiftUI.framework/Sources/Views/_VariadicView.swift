// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _VariadicView (Views).

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

public enum _VariadicView {
    public struct Tree<A, B> where A: SwiftUI._VariadicView_Root {
        public var root: A
        public var content: B
        public init(_ a0: A, content a1: () -> B) { _skeleton() }
        public init(root a0: A, content a1: B) { _skeleton() }
    }
}

extension SwiftUI._VariadicView.Tree where A: SwiftUI._VariadicView_ViewRoot, B: SwiftUI.View {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._VariadicView.Tree: SwiftUI.PrimitiveView where A: SwiftUI._VariadicView_ViewRoot, B: SwiftUI.View {
}

extension SwiftUI._VariadicView.Tree: SwiftUI.View where A: SwiftUI._VariadicView_ViewRoot, B: SwiftUI.View {
    public typealias Body = Swift.Never
}
