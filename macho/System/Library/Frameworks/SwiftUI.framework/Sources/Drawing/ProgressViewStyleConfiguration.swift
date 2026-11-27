// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProgressViewStyleConfiguration (Drawing).

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

public struct ProgressViewStyleConfiguration {
    public let fractionCompleted: Swift.Double?
    let hasLabel: Swift.Bool
    let hasCurrentValueLabel: Swift.Bool
    public var label: SwiftUI.ProgressViewStyleConfiguration.Label? { _skeleton() }
    public var currentValueLabel: SwiftUI.ProgressViewStyleConfiguration.CurrentValueLabel? { _skeleton() }
    public struct Label {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
    public struct CurrentValueLabel {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.ProgressViewStyleConfiguration.Label: SwiftUI.PrimitiveView {
}

extension SwiftUI.ProgressViewStyleConfiguration.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ProgressViewStyleConfiguration.CurrentValueLabel: SwiftUI.PrimitiveView {
}

extension SwiftUI.ProgressViewStyleConfiguration.CurrentValueLabel: SwiftUI.View {
    public typealias Body = Swift.Never
}
