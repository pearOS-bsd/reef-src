// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContextMenu (Navigation).

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

public struct ContextMenu<A> where A: SwiftUI.View {
    let menuItems: A
    public init(menuItems a0: () -> A) { _skeleton() }
}

extension SwiftUI.ContextMenu: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ContextMenu: SwiftUI.PrimitiveView {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}
