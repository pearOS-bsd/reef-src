// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AsyncImage (Images).

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

public struct AsyncImage<A> where A: SwiftUI.View {
    let url: Foundation.URL?
    let content: (SwiftUI.AsyncImagePhase) -> A
    public init(url a0: Foundation.URL?, scale a1: CoreGraphics.CGFloat, transaction a2: SwiftUI.Transaction, content a3: @escaping (SwiftUI.AsyncImagePhase) -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.AsyncImage: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.AsyncImage: SwiftUI.PrimitiveView {
}
