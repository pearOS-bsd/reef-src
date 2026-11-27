// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LabelStyleConfiguration (Text).

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

public struct LabelStyleConfiguration {
    public var title: SwiftUI.LabelStyleConfiguration.Title { _skeleton() }
    public var icon: SwiftUI.LabelStyleConfiguration.Icon { _skeleton() }
    public struct Title {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
    public struct Icon {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.LabelStyleConfiguration.Icon: SwiftUI.PrimitiveView {
}

extension SwiftUI.LabelStyleConfiguration.Icon: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.LabelStyleConfiguration.Title: SwiftUI.PrimitiveView {
}

extension SwiftUI.LabelStyleConfiguration.Title: SwiftUI.View {
    public typealias Body = Swift.Never
}
