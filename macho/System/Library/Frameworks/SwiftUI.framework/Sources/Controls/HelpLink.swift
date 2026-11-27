// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HelpLink (Controls).

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

public struct HelpLink {
    let action: () -> ()
    var _controlSize: SwiftUI.Environment<SwiftUI.ControlSize>
    public init(action a0: @escaping () -> ()) { _skeleton() }
    public init(anchor a0: Swift.String) { _skeleton() }
    public var body: some SwiftUI.View { _skeleton() }
}

extension SwiftUI.HelpLink: SwiftUI.View {
}
