// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlSizeKey (Environment).

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

enum ControlSizeKey {
}

extension SwiftUI.ControlSizeKey: SwiftUI.EnvironmentKey {
    typealias Value = SwiftUI.ControlSize
    static var defaultValue: Self.Value { _skeleton() }
}
