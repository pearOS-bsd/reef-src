// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextTrackingKey (Text).

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

enum TextTrackingKey {
}

extension SwiftUI.TextTrackingKey: SwiftUI.EnvironmentKey {
    typealias Value = CoreGraphics.CGFloat
    static var defaultValue: Self.Value { 0 }
}
