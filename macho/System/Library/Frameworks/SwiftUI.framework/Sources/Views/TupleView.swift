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
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    var childValues: [Any] { _skeleton() }
    public init(_ a0: A) { _skeleton() }
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
