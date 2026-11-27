// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonStyleConfiguration (Drawing).

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

public struct ButtonStyleConfiguration {
    let pressed: Swift.Bool
    let buttonRole: SwiftUI.ButtonRole?
    public var label: SwiftUI.ButtonStyleConfiguration.Label { _skeleton() }
    public var isPressed: Swift.Bool { _skeleton() }
    public var role: SwiftUI.ButtonRole? { _skeleton() }
    public struct Label {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.ButtonStyleConfiguration.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ButtonStyleConfiguration.Label: SwiftUI.PrimitiveView {
}
