// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MinimumScaleFactorKey (Misc).

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

enum MinimumScaleFactorKey {
}

extension SwiftUI.MinimumScaleFactorKey: SwiftUI.EnvironmentKey {
    typealias Value = CoreGraphics.CGFloat
    static var defaultValue: Self.Value { 1.0 }
}
