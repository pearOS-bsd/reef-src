// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// List (Lists).

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

public struct List<A, B> where A: Swift.Hashable, B: SwiftUI.View {
    let content: B
    let selection: SwiftUI.ListSelection?
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    init(content a0: B, selection a1: SwiftUI.ListSelection?) { _skeleton() }
    static func multiple(_ a0: (SwiftUI.Binding<Swift.Set<A>>)?) -> SwiftUI.ListSelection? { _skeleton() }
    static func single(_ a0: (SwiftUI.Binding<A?>)?) -> SwiftUI.ListSelection? { _skeleton() }
    static func required(_ a0: SwiftUI.Binding<A>) -> SwiftUI.ListSelection { _skeleton() }
    public init(selection a0: (SwiftUI.Binding<Swift.Set<A>>)?, content a1: () -> B) { _skeleton() }
    public init(selection a0: (SwiftUI.Binding<A?>)?, content a1: () -> B) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<A>, content a1: () -> B) { _skeleton() }
    public init<A1, B1>(_ a0: A1, selection a1: (SwiftUI.Binding<Swift.Set<A>>)?, rowContent a2: @escaping (A1.Element) -> B1) where B == SwiftUI.ForEach<A1, A1.Element.ID, B1>, A1: Swift.RandomAccessCollection, B1: SwiftUI.View, A1.Element: Swift.Identifiable { _skeleton() }
    public init<A1, B1>(_ a0: A1, selection a1: (SwiftUI.Binding<A?>)?, rowContent a2: @escaping (A1.Element) -> B1) where B == SwiftUI.ForEach<A1, A1.Element.ID, B1>, A1: Swift.RandomAccessCollection, B1: SwiftUI.View, A1.Element: Swift.Identifiable { _skeleton() }
    public init<A1, B1, C1>(_ a0: A1, id a1: Swift.KeyPath<A1.Element, B1>, selection a2: (SwiftUI.Binding<Swift.Set<A>>)?, rowContent a3: @escaping (A1.Element) -> C1) where B == SwiftUI.ForEach<A1, B1, C1>, A1: Swift.RandomAccessCollection, B1: Swift.Hashable, C1: SwiftUI.View { _skeleton() }
    public init<A1, B1, C1>(_ a0: A1, id a1: Swift.KeyPath<A1.Element, B1>, selection a2: (SwiftUI.Binding<A?>)?, rowContent a3: @escaping (A1.Element) -> C1) where B == SwiftUI.ForEach<A1, B1, C1>, A1: Swift.RandomAccessCollection, B1: Swift.Hashable, C1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: A1, children a1: Swift.KeyPath<A1.Element, A1?>, selection a2: (SwiftUI.Binding<Swift.Set<A>>)?, rowContent a3: @escaping (A1.Element) -> B1) where B == SwiftUI.OutlineGroup<A1, A1.Element.ID, B1, B1, SwiftUI.DisclosureGroup<B1, SwiftUI.OutlineSubgroupChildren>>, A1: Swift.RandomAccessCollection, B1: SwiftUI.View, A1.Element: Swift.Identifiable { _skeleton() }
    public init<A1, B1>(_ a0: A1, children a1: Swift.KeyPath<A1.Element, A1?>, selection a2: (SwiftUI.Binding<A?>)?, rowContent a3: @escaping (A1.Element) -> B1) where B == SwiftUI.OutlineGroup<A1, A1.Element.ID, B1, B1, SwiftUI.DisclosureGroup<B1, SwiftUI.OutlineSubgroupChildren>>, A1: Swift.RandomAccessCollection, B1: SwiftUI.View, A1.Element: Swift.Identifiable { _skeleton() }
}

extension SwiftUI.List where A == Swift.Never {
    public init(content a0: () -> B) { _skeleton() }
    public init<A1, B1>(_ a0: A1, rowContent a1: @escaping (A1.Element) -> B1) where B == SwiftUI.ForEach<A1, A1.Element.ID, B1>, A1: Swift.RandomAccessCollection, B1: SwiftUI.View, A1.Element: Swift.Identifiable { _skeleton() }
    public init<A1, B1, C1>(_ a0: A1, id a1: Swift.KeyPath<A1.Element, B1>, rowContent a2: @escaping (A1.Element) -> C1) where B == SwiftUI.ForEach<A1, B1, C1>, A1: Swift.RandomAccessCollection, B1: Swift.Hashable, C1: SwiftUI.View { _skeleton() }
    public init<A1>(_ a0: Swift.Range<Swift.Int>, rowContent a1: @escaping (Swift.Int) -> A1) where B == SwiftUI.ForEach<Swift.Range<Swift.Int>, Swift.Int, A1>, A1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: A1, children a1: Swift.KeyPath<A1.Element, A1?>, rowContent a2: @escaping (A1.Element) -> B1) where B == SwiftUI.OutlineGroup<A1, A1.Element.ID, B1, B1, SwiftUI.DisclosureGroup<B1, SwiftUI.OutlineSubgroupChildren>>, A1: Swift.RandomAccessCollection, B1: SwiftUI.View, A1.Element: Swift.Identifiable { _skeleton() }
    public init<A1, B1, C1>(_ a0: A1, id a1: Swift.KeyPath<A1.Element, B1>, children a2: Swift.KeyPath<A1.Element, A1?>, rowContent a3: @escaping (A1.Element) -> C1) where B == SwiftUI.OutlineGroup<A1, B1, C1, C1, SwiftUI.DisclosureGroup<C1, SwiftUI.OutlineSubgroupChildren>>, A1: Swift.RandomAccessCollection, B1: Swift.Hashable, C1: SwiftUI.View { _skeleton() }
}

extension SwiftUI.List: SwiftUI.PrimitiveView {
}

extension SwiftUI.List: SwiftUI.View {
    public typealias Body = Swift.Never
}
