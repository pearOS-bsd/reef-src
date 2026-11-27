// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HelpLinkPlate (Controls).

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

struct HelpLinkPlate<A> where A: SwiftUI.View {
    let label: A
    let pressed: Swift.Bool
    var _colorScheme: SwiftUI.Environment<SwiftUI.ColorScheme>
    var _isEnabled: SwiftUI.Environment<Swift.Bool>
    var _controlSize: SwiftUI.Environment<SwiftUI.ControlSize>
    static func metrics(_ a0: SwiftUI.ControlSize) -> (side: CoreGraphics.CGFloat, font: CoreGraphics.CGFloat) { _skeleton() }
    var body: some SwiftUI.View { _skeleton() }
}

extension SwiftUI.HelpLinkPlate: SwiftUI.View {
}
