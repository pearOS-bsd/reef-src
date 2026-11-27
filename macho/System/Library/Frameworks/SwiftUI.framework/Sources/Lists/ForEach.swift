// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ForEach (Lists).

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

public struct ForEach<A, B, C> where A: Swift.RandomAccessCollection, B: Swift.Hashable {
    public var data: A
    public var content: (A.Element) -> C
    let id: Swift.KeyPath<A.Element, B>
    init(data a0: A, id a1: Swift.KeyPath<A.Element, B>, content a2: @escaping (A.Element) -> C) { _skeleton() }
    public static func create(_ a0: A, id a1: Swift.KeyPath<A.Element, B>, content a2: @escaping (A.Element) -> C) -> SwiftUI.ForEach<A, B, C> { _skeleton() }
    public init<A1>(subviews a0: A1, content a1: @escaping (SwiftUI.Subview) -> C) where A == SwiftUI.ForEachSubviewCollection<C>, B == SwiftUI.Subview.ID, C: SwiftUI.View, A1: SwiftUI.View { _skeleton() }
}

extension SwiftUI.ForEach where A == Swift.Range<Swift.Int>, B == Swift.Int, C: SwiftUI.View {
    public init(_ a0: Swift.Range<Swift.Int>, content a1: @escaping (Swift.Int) -> C) { _skeleton() }
}

extension SwiftUI.ForEach where C: SwiftUI.View {
    public init(_ a0: A, id a1: Swift.KeyPath<A.Element, B>, content a2: @escaping (A.Element) -> C) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    var identifiedChildren: [(Swift.AnyHashable, Any)] { _skeleton() }
}

extension SwiftUI.ForEach where B == A.Element.ID, C: SwiftUI.View, A.Element: Swift.Identifiable {
    public init(_ a0: A, content a1: @escaping (A.Element) -> C) { _skeleton() }
}

extension SwiftUI.ForEach where B == A.Element.ID, A.Element: Swift.Identifiable {
    public static func create(_ a0: A, content a1: @escaping (A.Element) -> C) -> SwiftUI.ForEach<A, B, C> { _skeleton() }
}

extension SwiftUI.ForEach where C: SwiftUI.TabContent {
    public var body: Swift.Never { _skeleton() }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
}

extension SwiftUI.ForEach where B == A.Element.ID, C: SwiftUI.TabContent, A.Element: Swift.Identifiable {
    public init<A1>(_ a0: A, content a1: @escaping (A.Element) -> C) where A1 == C.TabValue { _skeleton() }
}

extension SwiftUI.ForEach where C: SwiftUI.TableRowContent {
    public var tableRowBody: Swift.Never { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
    public init<A1>(_ a0: A, id a1: Swift.KeyPath<A.Element, B>, content a2: @escaping (A.Element) -> C) where A1 == C.TableRowValue { _skeleton() }
}

extension SwiftUI.ForEach where B == A.Element.ID, C: SwiftUI.TableRowContent, A.Element == C.TableRowValue {
    public init(_ a0: A, content a1: @escaping (A.Element) -> C) { _skeleton() }
}

extension SwiftUI.ForEach: SwiftUI.View where C: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ForEach: SwiftUI.PrimitiveView where C: SwiftUI.View {
}

extension SwiftUI.ForEach: SwiftUI.LazyRowSource where C: SwiftUI.View {
}

extension SwiftUI.ForEach: SwiftUI.MenuContributor where C: SwiftUI.View {
}

extension SwiftUI.ForEach: SwiftUI.ForEachChildren where C: SwiftUI.View {
}

extension SwiftUI.ForEach: SwiftUI.DynamicViewContent where C: SwiftUI.View {
    public typealias Data = A
}

extension SwiftUI.ForEach: SwiftUI.TabContent where C: SwiftUI.TabContent {
    public typealias TabValue = C.TabValue
    public typealias _IdentifiedView = SwiftUI.ForEach<A, B, C>
}

extension SwiftUI.ForEach: SwiftUI.TabPagesProviding where C: SwiftUI.TabContent {
}

extension SwiftUI.ForEach: SwiftUI.TableRowContent where C: SwiftUI.TableRowContent {
    public typealias TableRowValue = C.TableRowValue
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.ForEach: SwiftUI.DynamicTableRowContent where C: SwiftUI.TableRowContent {
}

extension SwiftUI.ForEach: SwiftUI.TableRowsSource where C: SwiftUI.TableRowContent {
}
