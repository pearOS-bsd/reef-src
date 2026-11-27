// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GridRow (Layout).

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

public struct GridRow<A> where A: SwiftUI.View {
    public var alignment: SwiftUI.VerticalAlignment?
    public var content: A
    public init(alignment a0: SwiftUI.VerticalAlignment?, content a1: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.GridRow: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.GridRow: SwiftUI.PrimitiveView {
}
