// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToggleStyleConfiguration (Drawing).

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

public struct ToggleStyleConfiguration {
    let binding: SwiftUI.Binding<Swift.Bool>
    public var label: SwiftUI.ToggleStyleConfiguration.Label { _skeleton() }
    public var isOn: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
    public struct Label {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.ToggleStyleConfiguration.Label: SwiftUI.PrimitiveView {
}

extension SwiftUI.ToggleStyleConfiguration.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}
