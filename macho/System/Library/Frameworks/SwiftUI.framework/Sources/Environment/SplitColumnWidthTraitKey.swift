// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SplitColumnWidthTraitKey (Environment).

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

struct SplitColumnWidthTraitKey {
    static func width(of a0: SwiftUI.Element) -> CoreGraphics.CGFloat? { _skeleton() }
}

extension SwiftUI.SplitColumnWidthTraitKey: SwiftUI._ViewTraitKey {
    typealias Value = CoreGraphics.CGFloat?
    static var defaultValue: Self.Value { nil }
}
