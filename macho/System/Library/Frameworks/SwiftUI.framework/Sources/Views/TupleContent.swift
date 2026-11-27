// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TupleContent (Views).

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

public struct TupleContent<each A> {
    public var value: (repeat each A)
    public init(_ a0: repeat each A) { _skeleton() }
    var children: [Any] { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    var childValues: [Any] { _skeleton() }
    func collectActions(into a0: inout [SwiftUI.ToolbarActions.Item]) { _skeleton() }
}

extension SwiftUI.TupleContent: SwiftUI.View where repeat each A: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.TupleContent: SwiftUI.ToolbarContent where repeat each A: SwiftUI.ToolbarContent {
}

extension SwiftUI.TupleContent: SwiftUI.PrimitiveView {
}

extension SwiftUI.TupleContent: SwiftUI.MenuContributor {
}

extension SwiftUI.TupleContent: SwiftUI.TupleChildren {
}

extension SwiftUI.TupleContent: SwiftUI.ToolbarActionSource {
}
