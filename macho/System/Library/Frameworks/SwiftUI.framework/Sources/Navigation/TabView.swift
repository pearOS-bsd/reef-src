// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabView (Navigation).

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

public struct TabView<A, B> where A: Swift.Hashable, B: SwiftUI.View {
    let content: B
    let selection: (SwiftUI.Binding<A>)?
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(selection a0: (SwiftUI.Binding<A>)?, content a1: () -> B) { _skeleton() }
    public init<A1>(selection a0: SwiftUI.Binding<A>, content a1: () -> A1) where B == SwiftUI.TabContentBuilder<A>.Content<A1>, A1: SwiftUI.TabContent { _skeleton() }
    public init<A1>(content a0: () -> A1) where A == Swift.Never, B == SwiftUI.TabContentBuilder<Swift.Never>.Content<A1>, A1: SwiftUI.TabContent { _skeleton() }
}

extension SwiftUI.TabView where A == Swift.Int {
    public init(content a0: () -> B) { _skeleton() }
}

extension SwiftUI.TabView: SwiftUI.PrimitiveView {
}

extension SwiftUI.TabView: SwiftUI.View {
    public typealias Body = Swift.Never
}
