// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableColumn (Navigation).

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

public struct TableColumn<A, B, C, D> where A: Swift.Identifiable, B: Foundation.SortComparator, C: SwiftUI.View, D: SwiftUI.View {
    let label: D
    let content: (A) -> C
    let comparator: B?
    var minWidth: CoreGraphics.CGFloat?
    var idealWidth: CoreGraphics.CGFloat?
    var maxWidth: CoreGraphics.CGFloat?
    init(label a0: D, comparator a1: B?, content a2: @escaping (A) -> C) { _skeleton() }
    public var tableColumnBody: Swift.Never { _skeleton() }
    public func width(_ a0: CoreGraphics.CGFloat?) -> SwiftUI.TableColumn<A, B, C, D> { _skeleton() }
    public func width(min a0: CoreGraphics.CGFloat?, ideal a1: CoreGraphics.CGFloat?, max a2: CoreGraphics.CGFloat?) -> SwiftUI.TableColumn<A, B, C, D> { _skeleton() }
    func tableColumns() -> [SwiftUI.TableColumnEntry] { _skeleton() }
}

extension SwiftUI.TableColumn where B == Swift.Never, D == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: @escaping (A) -> C) { _skeleton() }
    public init<A1>(_ a0: A1, content a1: @escaping (A) -> C) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.Text, content a1: @escaping (A) -> C) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, value a1: Swift.KeyPath<A, Swift.String>) where C == SwiftUI.Text { _skeleton() }
    public init<A1>(_ a0: A1, value a1: Swift.KeyPath<A, Swift.String>) where C == SwiftUI.Text, A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.Text, value a1: Swift.KeyPath<A, Swift.String>) where C == SwiftUI.Text { _skeleton() }
}

extension SwiftUI.TableColumn where B == Foundation.KeyPathComparator<A>, D == SwiftUI.Text {
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, value a1: Swift.KeyPath<A, A1>, content a2: @escaping (A) -> C) where A1: Swift.Comparable { _skeleton() }
    public init<A1, B1>(_ a0: A1, value a1: Swift.KeyPath<A, B1>, content a2: @escaping (A) -> C) where A1: Swift.StringProtocol, B1: Swift.Comparable { _skeleton() }
    public init<A1>(_ a0: SwiftUI.Text, value a1: Swift.KeyPath<A, A1>, content a2: @escaping (A) -> C) where A1: Swift.Comparable { _skeleton() }
    public init<A1, B1>(_ a0: SwiftUI.LocalizedStringKey, value a1: Swift.KeyPath<A, A1>, comparator a2: B1, content a3: @escaping (A) -> C) where A1 == B1.Compared, B1: Foundation.SortComparator { _skeleton() }
    public init<A1, B1>(_ a0: SwiftUI.Text, value a1: Swift.KeyPath<A, A1>, comparator a2: B1, content a3: @escaping (A) -> C) where A1 == B1.Compared, B1: Foundation.SortComparator { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, value a1: Swift.KeyPath<A, Swift.String>, comparator a2: Swift.String.StandardComparator) where C == SwiftUI.Text { _skeleton() }
    public init<A1>(_ a0: A1, value a1: Swift.KeyPath<A, Swift.String>, comparator a2: Swift.String.StandardComparator) where C == SwiftUI.Text, A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.Text, value a1: Swift.KeyPath<A, Swift.String>, comparator a2: Swift.String.StandardComparator) where C == SwiftUI.Text { _skeleton() }
}

extension SwiftUI.TableColumn where A == B.Compared, D == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, sortUsing a1: B, content a2: @escaping (A) -> C) { _skeleton() }
    public init<A1>(_ a0: A1, sortUsing a1: B, content a2: @escaping (A) -> C) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.Text, sortUsing a1: B, content a2: @escaping (A) -> C) { _skeleton() }
}

extension SwiftUI.TableColumn: SwiftUI.TableColumnContent {
    public typealias TableRowValue = A
    public typealias TableColumnSortComparator = B
    public typealias TableColumnBody = Swift.Never
}

extension SwiftUI.TableColumn: SwiftUI.TableColumnsSource {
}
