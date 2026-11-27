// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlActiveStateKey (State).

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

enum ControlActiveStateKey {
}

extension SwiftUI.ControlActiveStateKey: SwiftUI.EnvironmentKey {
    typealias Value = SwiftUI.ControlActiveState
    static var defaultValue: Self.Value { _skeleton() }
}
