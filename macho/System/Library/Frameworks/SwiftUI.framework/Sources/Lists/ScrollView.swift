// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollView (Lists).

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

public struct ScrollView<A> where A: SwiftUI.View {
    public var content: A
    public var axes: SwiftUI.Axis.Set
    public var showsIndicators: Swift.Bool
    public init(_ a0: SwiftUI.Axis.Set, showsIndicators a1: Swift.Bool, content a2: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ScrollView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ScrollView: SwiftUI.PrimitiveView {
}
