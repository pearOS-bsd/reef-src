// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PrimitiveButtonStyleBox (Drawing).

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

class PrimitiveButtonStyleBox: SwiftUI.ButtonStyleBox {
    let make: (SwiftUI.PrimitiveButtonStyleConfiguration) -> any SwiftUI.View
    init<A>(_ a0: A) where A: SwiftUI.PrimitiveButtonStyle {
        make = { a0.makeBody(configuration: $0) }
        super.init(style: a0)
    }
    deinit {}
}
