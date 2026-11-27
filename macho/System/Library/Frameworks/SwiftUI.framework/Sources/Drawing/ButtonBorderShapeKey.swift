// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonBorderShapeKey (Drawing).

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

enum ButtonBorderShapeKey {
}

extension SwiftUI.ButtonBorderShapeKey: SwiftUI.EnvironmentKey {
    typealias Value = SwiftUI.ButtonBorderShape
    static var defaultValue: Self.Value { .automatic }
}
