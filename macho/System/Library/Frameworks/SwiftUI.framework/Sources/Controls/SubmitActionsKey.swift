// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SubmitActionsKey (Controls).

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

enum SubmitActionsKey {
    static var defaultValue: [() -> ()] { _skeleton() }
}

extension SwiftUI.SubmitActionsKey: SwiftUI.EnvironmentKey {
    typealias Value = [() -> ()]
}
