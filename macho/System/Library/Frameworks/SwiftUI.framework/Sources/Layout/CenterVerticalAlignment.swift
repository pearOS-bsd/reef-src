// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CenterVerticalAlignment (Layout).

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

enum CenterVerticalAlignment {
}

extension SwiftUI.CenterVerticalAlignment: SwiftUI.AlignmentID {
    static func defaultValue(`in` a0: SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat { a0.height * 0.5 }
}
