// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GroupBoxStyleConfiguration (Drawing).

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

public struct GroupBoxStyleConfiguration {
    public let label: SwiftUI.GroupBoxStyleConfiguration.Label
    public let content: SwiftUI.GroupBoxStyleConfiguration.Content
    public struct Label {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
    public struct Content {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.GroupBoxStyleConfiguration.Label: SwiftUI.PrimitiveView {
}

extension SwiftUI.GroupBoxStyleConfiguration.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.GroupBoxStyleConfiguration.Content: SwiftUI.PrimitiveView {
}

extension SwiftUI.GroupBoxStyleConfiguration.Content: SwiftUI.View {
    public typealias Body = Swift.Never
}
