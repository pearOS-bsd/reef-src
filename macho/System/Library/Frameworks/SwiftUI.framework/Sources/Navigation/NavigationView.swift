// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationView (Navigation).

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

public struct NavigationView<A> where A: SwiftUI.View {
    let content: A
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(content a0: () -> A) { _skeleton() }
}

extension SwiftUI.NavigationView: SwiftUI.PrimitiveView {
}

extension SwiftUI.NavigationView: SwiftUI.View {
    public typealias Body = Swift.Never
}
