// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to Swift_Optional.

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

extension Swift.Optional where Wrapped: SwiftUI.View {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    var wrappedPart: Any? { _skeleton() }
}

extension Swift.Optional where Wrapped: SwiftUI.ToolbarContent {
    func collectActions(into a0: inout [SwiftUI.ToolbarActions.Item]) { _skeleton() }
}

extension Swift.Optional where Wrapped: SwiftUI.TableRowContent {
    public var tableRowBody: Swift.Never { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
}

extension Swift.Optional: SwiftUI.View where Wrapped: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension Swift.Optional: SwiftUI.PrimitiveView where Wrapped: SwiftUI.View {
}

extension Swift.Optional: SwiftUI.LazyRowSource where Wrapped: SwiftUI.View {
}

extension Swift.Optional: SwiftUI.MenuContributor where Wrapped: SwiftUI.View {
}

extension Swift.Optional: SwiftUI.OptionalViewParts where Wrapped: SwiftUI.View {
}

extension Swift.Optional: SwiftUI.ToolbarContent where Wrapped: SwiftUI.ToolbarContent {
}

extension Swift.Optional: SwiftUI.ToolbarActionSource where Wrapped: SwiftUI.ToolbarContent {
}

extension Swift.Optional: SwiftUI.TableRowContent where Wrapped: SwiftUI.TableRowContent {
    public typealias TableRowValue = Wrapped.TableRowValue
    public typealias TableRowBody = Swift.Never
}

extension Swift.Optional: SwiftUI.TableRowsSource where Wrapped: SwiftUI.TableRowContent {
}
