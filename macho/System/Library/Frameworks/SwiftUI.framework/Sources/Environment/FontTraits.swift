// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FontTraits (Environment).

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

struct FontTraits {
    var size: CoreGraphics.CGFloat
    var weight: SwiftUI.Font.Weight?
    var italic: Swift.Bool
    var design: SwiftUI.Font.Design?
    var monospaced: Swift.Bool
    var monospacedDigit: Swift.Bool
    var name: Swift.String?
    var width: SwiftUI.Font.Width?
    var leading: SwiftUI.Font.Leading?
}
