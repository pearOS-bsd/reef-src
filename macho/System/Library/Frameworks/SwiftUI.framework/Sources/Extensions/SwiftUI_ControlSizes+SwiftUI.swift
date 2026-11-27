// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_ControlSizes.

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

/// the metrics of the environment's control size
struct ControlSizes {
    var size: SwiftUI.ControlSize
    init(_ a0: SwiftUI.EnvironmentValues) { size = a0.controlSize }
}
