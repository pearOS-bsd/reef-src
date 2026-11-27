// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// RepeatProgressKey (Controls).

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

enum RepeatProgressKey {
}

extension SwiftUI.RepeatProgressKey: SwiftUI.AnimationStateKey {
    typealias Value = (count: Swift.Int, start: Swift.Double)
    static var defaultValue: Self.Value { _skeleton() }
}
