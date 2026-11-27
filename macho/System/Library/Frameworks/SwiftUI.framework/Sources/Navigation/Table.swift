// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Table (Navigation).

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

public struct Table<A, B, C> where A == B.TableRowValue, B: SwiftUI.TableRowContent, C: SwiftUI.TableColumnContent, B.TableRowValue == C.TableRowValue {
    let rows: B
    let columns: C
    let selection: SwiftUI.TableSelection?
    let sortOrder: Any?
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    init(rows a0: B, columns a1: C, selection a2: SwiftUI.TableSelection?, sortOrder a3: Any?) { _skeleton() }
    static func single(_ a0: SwiftUI.Binding<A.ID?>) -> SwiftUI.TableSelection { _skeleton() }
    static func multiple(_ a0: SwiftUI.Binding<Swift.Set<A.ID>>) -> SwiftUI.TableSelection { _skeleton() }
    public init<A1>(_ a0: A1, columns a1: () -> C) where B == SwiftUI.TableForEachContent<A1>, A1: Swift.RandomAccessCollection, C.TableRowValue == A1.Element { _skeleton() }
    public init<A1>(_ a0: A1, selection a1: SwiftUI.Binding<A.ID?>, columns a2: () -> C) where B == SwiftUI.TableForEachContent<A1>, A1: Swift.RandomAccessCollection, C.TableRowValue == A1.Element { _skeleton() }
    public init<A1>(_ a0: A1, selection a1: SwiftUI.Binding<Swift.Set<A.ID>>, columns a2: () -> C) where B == SwiftUI.TableForEachContent<A1>, A1: Swift.RandomAccessCollection, C.TableRowValue == A1.Element { _skeleton() }
    public init<A1, B1>(_ a0: A1, sortOrder a1: SwiftUI.Binding<[B1]>, columns a2: () -> C) where B == SwiftUI.TableForEachContent<A1>, A1: Swift.RandomAccessCollection, B1: Foundation.SortComparator, C.TableRowValue == A1.Element, A1.Element == B1.Compared { _skeleton() }
    public init<A1, B1>(_ a0: A1, selection a1: SwiftUI.Binding<A.ID?>, sortOrder a2: SwiftUI.Binding<[B1]>, columns a3: () -> C) where B == SwiftUI.TableForEachContent<A1>, A1: Swift.RandomAccessCollection, B1: Foundation.SortComparator, C.TableRowValue == A1.Element, A1.Element == B1.Compared { _skeleton() }
    public init<A1, B1>(_ a0: A1, selection a1: SwiftUI.Binding<Swift.Set<A.ID>>, sortOrder a2: SwiftUI.Binding<[B1]>, columns a3: () -> C) where B == SwiftUI.TableForEachContent<A1>, A1: Swift.RandomAccessCollection, B1: Foundation.SortComparator, C.TableRowValue == A1.Element, A1.Element == B1.Compared { _skeleton() }
    public init(columns a0: () -> C, rows a1: () -> B) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<A.ID?>, columns a1: () -> C, rows a2: () -> B) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<Swift.Set<A.ID>>, columns a1: () -> C, rows a2: () -> B) { _skeleton() }
    public init(of a0: A.Type, columns a1: () -> C, rows a2: () -> B) { _skeleton() }
    public init(of a0: A.Type, selection a1: SwiftUI.Binding<A.ID?>, columns a2: () -> C, rows a3: () -> B) { _skeleton() }
    public init(of a0: A.Type, selection a1: SwiftUI.Binding<Swift.Set<A.ID>>, columns a2: () -> C, rows a3: () -> B) { _skeleton() }
    public init<A1>(sortOrder a0: SwiftUI.Binding<[A1]>, columns a1: () -> C, rows a2: () -> B) where A1: Foundation.SortComparator, C.TableRowValue == A1.Compared { _skeleton() }
    public init<A1>(selection a0: SwiftUI.Binding<Swift.Set<A.ID>>, sortOrder a1: SwiftUI.Binding<[A1]>, columns a2: () -> C, rows a3: () -> B) where A1: Foundation.SortComparator, C.TableRowValue == A1.Compared { _skeleton() }
}

extension SwiftUI.Table: SwiftUI.PrimitiveView {
}

extension SwiftUI.Table: SwiftUI.View {
    public typealias Body = Swift.Never
}
