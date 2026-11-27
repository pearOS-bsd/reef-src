// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OutlineGroup (Controls).

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

public struct OutlineGroup<A, B, C, D, E> where A: Swift.RandomAccessCollection, B: Swift.Hashable {
    let roots: [SwiftUI.OutlineNode]
    var tableRoots: [SwiftUI.TableRowEntry]
    init(roots a0: [SwiftUI.OutlineNode]) { _skeleton() }
    init(tableRoots a0: [SwiftUI.TableRowEntry]) { _skeleton() }
}

extension SwiftUI.OutlineGroup where C: SwiftUI.View, D: SwiftUI.View, E: SwiftUI.View {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init<A1>(_ a0: A, children a1: Swift.KeyPath<A1, A?>, content a2: @escaping (A1) -> C) where B == A1.ID, C == D, E == SwiftUI.DisclosureGroup<C, SwiftUI.OutlineSubgroupChildren>, A1: Swift.Identifiable, A1 == A.Element { _skeleton() }
    public init<A1>(_ a0: A, id a1: Swift.KeyPath<A1, B>, children a2: Swift.KeyPath<A1, A?>, content a3: @escaping (A1) -> C) where C == D, E == SwiftUI.DisclosureGroup<C, SwiftUI.OutlineSubgroupChildren>, A1 == A.Element { _skeleton() }
    static func nodes<A1>(_ a0: [A1], id a1: Swift.KeyPath<A1, B>, children a2: Swift.KeyPath<A1, A?>, content a3: @escaping (A1) -> D) -> [SwiftUI.OutlineNode] { _skeleton() }
    public init<A1>(_ a0: A1, children a1: Swift.KeyPath<A1, A?>, content a2: @escaping (A1) -> C) where B == A1.ID, C == D, E == SwiftUI.DisclosureGroup<C, SwiftUI.OutlineSubgroupChildren>, A1: Swift.Identifiable, A1 == A.Element { _skeleton() }
    public init<A1>(_ a0: A1, id a1: Swift.KeyPath<A1, B>, children a2: Swift.KeyPath<A1, A?>, content a3: @escaping (A1) -> C) where C == D, E == SwiftUI.DisclosureGroup<C, SwiftUI.OutlineSubgroupChildren>, A1 == A.Element { _skeleton() }
}

extension SwiftUI.OutlineGroup where B == A.Element.ID, C: SwiftUI.TableRowContent, C == D, D == E, A.Element == C.TableRowValue {
    public var tableRowBody: Swift.Never { _skeleton() }
    public init<A1>(_ a0: A, children a1: Swift.KeyPath<A1, A?>) where B == A1.ID, C == SwiftUI.TableRow<A1>, D == SwiftUI.TableRow<A1>, E == SwiftUI.TableRow<A1>, A1: Swift.Identifiable, A1 == A.Element { _skeleton() }
}

extension SwiftUI.OutlineGroup: SwiftUI.PrimitiveView where C: SwiftUI.View, D: SwiftUI.View, E: SwiftUI.View {
}

extension SwiftUI.OutlineGroup: SwiftUI.View where C: SwiftUI.View, D: SwiftUI.View, E: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.OutlineGroup: SwiftUI.TableRowContent where B == A.Element.ID, C: SwiftUI.TableRowContent, C == D, D == E, A.Element == C.TableRowValue {
    public typealias TableRowValue = C.TableRowValue
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.OutlineGroup: SwiftUI.TableRowsSource where B == A.Element.ID, C: SwiftUI.TableRowContent, C == D, D == E, A.Element == C.TableRowValue {
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
}
