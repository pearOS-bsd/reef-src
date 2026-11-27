// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EmptyView (Views).

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

public struct EmptyView {
    public init() {}
}

extension SwiftUI.EmptyView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.EmptyView: SwiftUI.PrimitiveView {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { [] }
}

extension SwiftUI.EmptyView: SwiftUI.LazyRowSource {
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
}

extension SwiftUI.EmptyView: SwiftUI.MenuContributor {
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
}
