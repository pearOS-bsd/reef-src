// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GroupElementsOfContent (Controls).

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

public struct GroupElementsOfContent<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let subviews: A
    let transform: (SwiftUI.SubviewsCollection) -> B
    init(subviews a0: A, transform a1: @escaping (SwiftUI.SubviewsCollection) -> B) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.GroupElementsOfContent: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.GroupElementsOfContent: SwiftUI.PrimitiveView {
}
