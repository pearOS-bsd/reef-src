// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SwitchToggleStyle (Drawing).

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

public struct SwitchToggleStyle {
    var tint: SwiftUI.Color?
    public init() { _skeleton() }
    public init(tint a0: SwiftUI.Color) { _skeleton() }
    public func makeBody(configuration a0: SwiftUI.ToggleStyleConfiguration) -> SwiftUI.ToggleStyleConfiguration.Label { _skeleton() }
}

extension SwiftUI.SwitchToggleStyle: SwiftUI.ToggleStyle {
    public typealias Body = SwiftUI.ToggleStyleConfiguration.Label
}
