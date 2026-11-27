// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GeometryReader (Layout).

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

public struct GeometryReader<A> where A: SwiftUI.View {
    public var content: (SwiftUI.GeometryProxy) -> A
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(content a0: @escaping (SwiftUI.GeometryProxy) -> A) { _skeleton() }
}

extension SwiftUI.GeometryReader: SwiftUI.PrimitiveView {
}

extension SwiftUI.GeometryReader: SwiftUI.View {
    public typealias Body = Swift.Never
}
