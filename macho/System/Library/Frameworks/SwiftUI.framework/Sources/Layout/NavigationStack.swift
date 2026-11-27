// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationStack (Layout).

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

public struct NavigationStack<A, B> where B: SwiftUI.View {
    let root: B
    let path: SwiftUI.NavigationPathAccess?
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(root a0: () -> B) where A == SwiftUI.NavigationPath { _skeleton() }
    public init(path a0: SwiftUI.Binding<SwiftUI.NavigationPath>, root a1: () -> B) where A == SwiftUI.NavigationPath { _skeleton() }
    public init(path a0: SwiftUI.Binding<A>, root a1: () -> B) where A: Swift.MutableCollection, A: Swift.RandomAccessCollection, A: Swift.RangeReplaceableCollection, A.Element: Swift.Hashable { _skeleton() }
}

extension SwiftUI.NavigationStack: SwiftUI.PrimitiveView {
}

extension SwiftUI.NavigationStack: SwiftUI.View {
    public typealias Body = Swift.Never
}
