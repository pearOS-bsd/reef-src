// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OuterStylesByKindKey (Drawing).

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

enum OuterStylesByKindKey {
    static var defaultValue: [SwiftUI.StyleKind: [Any]] { _skeleton() }
}

extension SwiftUI.OuterStylesByKindKey: SwiftUI.EnvironmentKey {
    typealias Value = [SwiftUI.StyleKind: [Any]]
}
