// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DisclosureGroupStyleConfiguration (Drawing).

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

public struct DisclosureGroupStyleConfiguration {
    @SwiftUI.Binding public var isExpanded: Swift.Bool
    public var label: SwiftUI.DisclosureGroupStyleConfiguration.Label { _skeleton() }
    public var content: SwiftUI.DisclosureGroupStyleConfiguration.Content { _skeleton() }
    public struct Label {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
    public struct Content {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.DisclosureGroupStyleConfiguration.Label: SwiftUI.PrimitiveView {
}

extension SwiftUI.DisclosureGroupStyleConfiguration.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.DisclosureGroupStyleConfiguration.Content: SwiftUI.PrimitiveView {
}

extension SwiftUI.DisclosureGroupStyleConfiguration.Content: SwiftUI.View {
    public typealias Body = Swift.Never
}
