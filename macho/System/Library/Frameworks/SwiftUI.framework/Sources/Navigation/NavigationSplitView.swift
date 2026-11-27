// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationSplitView (Navigation).

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

public struct NavigationSplitView<A, B, C> where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View {
    let sidebar: A
    let content: B?
    let detail: C
    let columnVisibility: (SwiftUI.Binding<SwiftUI.NavigationSplitViewVisibility>)?
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    init(sidebar a0: A, content a1: B?, detail a2: C, columnVisibility a3: (SwiftUI.Binding<SwiftUI.NavigationSplitViewVisibility>)?) { _skeleton() }
    public init(sidebar a0: () -> A, content a1: () -> B, detail a2: () -> C) { _skeleton() }
    public init(columnVisibility a0: SwiftUI.Binding<SwiftUI.NavigationSplitViewVisibility>, sidebar a1: () -> A, content a2: () -> B, detail a3: () -> C) { _skeleton() }
    public init(sidebar a0: () -> A, detail a1: () -> C) where B == SwiftUI.EmptyView { _skeleton() }
    public init(columnVisibility a0: SwiftUI.Binding<SwiftUI.NavigationSplitViewVisibility>, sidebar a1: () -> A, detail a2: () -> C) where B == SwiftUI.EmptyView { _skeleton() }
    public init(preferredCompactColumn a0: SwiftUI.Binding<SwiftUI.NavigationSplitViewColumn>, sidebar a1: () -> A, detail a2: () -> C) where B == SwiftUI.EmptyView { _skeleton() }
}

extension SwiftUI.NavigationSplitView: SwiftUI.PrimitiveView {
}

extension SwiftUI.NavigationSplitView: SwiftUI.View {
    public typealias Body = Swift.Never
}
