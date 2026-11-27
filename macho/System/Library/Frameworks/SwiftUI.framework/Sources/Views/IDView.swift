// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// IDView (Views).

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

public struct IDView<A, B> where A: SwiftUI.View, B: Swift.Hashable {
    public var content: A
    public var id: B
    public init(_ a0: A, id a1: B) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.IDView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.IDView: SwiftUI.PrimitiveView {
}
