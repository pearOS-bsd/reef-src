// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PrimitiveButtonStyleConfiguration (Drawing).

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

public struct PrimitiveButtonStyleConfiguration {
    let action: () -> ()
    let buttonRole: SwiftUI.ButtonRole?
    public var label: SwiftUI.PrimitiveButtonStyleConfiguration.Label { _skeleton() }
    public var role: SwiftUI.ButtonRole? { _skeleton() }
    public func trigger() { _skeleton() }
    public struct Label {
        func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    }
}

extension SwiftUI.PrimitiveButtonStyleConfiguration.Label: SwiftUI.PrimitiveView {
}

extension SwiftUI.PrimitiveButtonStyleConfiguration.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}
