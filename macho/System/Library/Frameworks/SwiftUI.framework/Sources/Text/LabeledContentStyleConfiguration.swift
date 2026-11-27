// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LabeledContentStyleConfiguration (Text).

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

public struct LabeledContentStyleConfiguration {
    public let label: SwiftUI.LabeledContentStyleConfiguration.Label
    public let content: SwiftUI.LabeledContentStyleConfiguration.Content
    public struct Label {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
    public struct Content {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.LabeledContentStyleConfiguration.Label: SwiftUI.PrimitiveView {
}

extension SwiftUI.LabeledContentStyleConfiguration.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.LabeledContentStyleConfiguration.Content: SwiftUI.PrimitiveView {
}

extension SwiftUI.LabeledContentStyleConfiguration.Content: SwiftUI.View {
    public typealias Body = Swift.Never
}
