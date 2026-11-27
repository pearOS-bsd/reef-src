// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CalendarKey (Misc).

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

enum CalendarKey {
}

extension SwiftUI.CalendarKey: SwiftUI.EnvironmentKey {
    typealias Value = Foundation.Calendar
    static var defaultValue: Self.Value { .autoupdatingCurrent }
}
