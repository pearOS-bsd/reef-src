// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Section (Controls).

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

public struct Section<A, B, C> {
    let parent: A
    let content: B
    let footer: C
    var isExpanded: (SwiftUI.Binding<Swift.Bool>)?
    var isCollapsible: Swift.Bool
    init(parent a0: A, content a1: B, footer a2: C, isExpanded a3: (SwiftUI.Binding<Swift.Bool>)?) { _skeleton() }
    var contentPart: Any { _skeleton() }
    public static func create(isExpanded a0: (SwiftUI.Binding<Swift.Bool>)?, content a1: B, header a2: A, footer a3: C) -> SwiftUI.Section<A, B, C> { _skeleton() }
    var headerPart: Any { _skeleton() }
}

extension SwiftUI.Section where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View {
    public init(header a0: A, footer a1: C, content a2: () -> B) { _skeleton() }
    public init(content a0: () -> B, header a1: () -> A, footer a2: () -> C) { _skeleton() }
    public func collapsible(_ a0: Swift.Bool) -> some SwiftUI.View { _skeleton() }
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
}

extension SwiftUI.Section where A: SwiftUI.View, B: SwiftUI.View, C == SwiftUI.EmptyView {
    public init(header a0: A, content a1: () -> B) { _skeleton() }
    public init(content a0: () -> B, header a1: () -> A) { _skeleton() }
    public init(isExpanded a0: SwiftUI.Binding<Swift.Bool>, content a1: () -> B, header a2: () -> A) { _skeleton() }
}

extension SwiftUI.Section where A == SwiftUI.EmptyView, B: SwiftUI.View, C: SwiftUI.View {
    public init(footer a0: C, content a1: () -> B) { _skeleton() }
    public init(content a0: () -> B, footer a1: () -> C) { _skeleton() }
}

extension SwiftUI.Section where A == SwiftUI.EmptyView, B: SwiftUI.View, C == SwiftUI.EmptyView {
    public init(content a0: () -> B) { _skeleton() }
}

extension SwiftUI.Section where A == SwiftUI.Text, B: SwiftUI.View, C == SwiftUI.EmptyView {
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> B) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, isExpanded a1: SwiftUI.Binding<Swift.Bool>, content a2: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, isExpanded a1: SwiftUI.Binding<Swift.Bool>, content a2: () -> B) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Section where A: SwiftUI.TableRowContent, B: SwiftUI.TableRowContent, C: SwiftUI.TableRowContent {
    public var tableRowBody: Swift.Never { _skeleton() }
    public init<A1, B1>(content a0: () -> B, header a1: () -> B1) where A == SwiftUI.TableHeaderRowContent<A1, B1>, C == SwiftUI.EmptyTableRowContent<A1>, A1 == B.TableRowValue, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: B1, content a1: () -> B) where A == SwiftUI.TableHeaderRowContent<A1, SwiftUI.Text>, C == SwiftUI.EmptyTableRowContent<A1>, A1 == B.TableRowValue, B1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> B) where A == SwiftUI.TableHeaderRowContent<A1, SwiftUI.Text>, C == SwiftUI.EmptyTableRowContent<A1>, A1 == B.TableRowValue { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
}

extension SwiftUI.Section: SwiftUI.View where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Section: SwiftUI.LazyRowSource where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View {
}

extension SwiftUI.Section: SwiftUI.PrimitiveView where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View {
}

extension SwiftUI.Section: SwiftUI.MenuContributor where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View {
}

extension SwiftUI.Section: SwiftUI.SectionParts {
}

extension SwiftUI.Section: SwiftUI.TableRowContent where A: SwiftUI.TableRowContent, B: SwiftUI.TableRowContent, C: SwiftUI.TableRowContent {
    public typealias TableRowValue = B.TableRowValue
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.Section: SwiftUI.TableRowsSource where A: SwiftUI.TableRowContent, B: SwiftUI.TableRowContent, C: SwiftUI.TableRowContent {
}
