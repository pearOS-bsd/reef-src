// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TupleView (Views).

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

public struct TupleView<A> {
    public var value: A
    /// the elements of each child view, under its index among the tuple's values
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        var elements: [SwiftUI.Element] = []
        for (index, child) in Mirror(reflecting: value).children.enumerated() {
            guard let view = child.value as? any SwiftUI.View else { continue }
            var context = a0
            context.path.append(AnyHashable(index))
            elements.append(contentsOf: SwiftUI.TupleView<A>.build(view, context))
        }
        return elements
    }
    private static func build<V: SwiftUI.View>(_ a0: V, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        SwiftUI.ViewBuild.build(a0, a1)
    }
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    var childValues: [Any] { Mirror(reflecting: value).children.map { $0.value } }
    public init(_ a0: A) { value = a0 }
}

extension SwiftUI.TupleView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.TupleView: SwiftUI.PrimitiveView {
}

extension SwiftUI.TupleView: SwiftUI.LazyRowSource {
}

extension SwiftUI.TupleView: SwiftUI.MenuContributor {
}

extension SwiftUI.TupleView: SwiftUI.TupleChildren {
}
