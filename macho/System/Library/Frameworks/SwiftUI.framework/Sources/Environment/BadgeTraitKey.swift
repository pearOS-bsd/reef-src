// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BadgeTraitKey (Environment).

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

struct BadgeTraitKey {
}

extension SwiftUI.BadgeTraitKey: SwiftUI._ViewTraitKey {
    typealias Value = SwiftUI.Text?
    static var defaultValue: Self.Value { nil }
}
