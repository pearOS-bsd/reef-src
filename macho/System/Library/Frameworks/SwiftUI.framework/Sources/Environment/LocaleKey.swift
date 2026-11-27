// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LocaleKey (Environment).

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

enum LocaleKey {
    static var defaultValue: Foundation.Locale { Foundation.Locale.autoupdatingCurrent }
}

extension SwiftUI.LocaleKey: SwiftUI.EnvironmentKey {
    typealias Value = Foundation.Locale
}
